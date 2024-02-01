#pragma once

#include "ChessEngine.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "queue"

#define DOUBLE_MAX std::numeric_limits<double>::max()
#define UCT_CONST sqrt(2) //TODO check

class MCTS_Node {
    // parent of this node (None for the root)
    MCTS_Node* parent = nullptr;
    // action we took to get here from parent
    // we can use this to iteratively reconstruct the board corresponding to this node
    const Action* action = nullptr;
    double value = 0.0; // rolling mean of rewards from this node
    double visits = 0;  // amount of time a node is visited (used for calculation of UCT)

public:
    std::vector<MCTS_Node> children {};  // the children of this node
    /* a node in the MCTS-gameTree */
    MCTS_Node(MCTS_Node* parentArg,const Action* move): parent(parentArg), action(move){};

    [[nodiscard]] double uct() const;
    void visit() {visits++;}
    void updateValue (int flag, int reward) {value += (flag * reward - value) / visits;}
    [[nodiscard]] MCTS_Node* getParent() const {return parent;}
    [[nodiscard]] double getValue() const {return value;}
    [[nodiscard]] const Action* getAction() const {return action;}
};

class MonteCarloEngine : public ChessEngine {
public:
    MonteCarloEngine(bool isWhite, int depth, int itt, ChessEngine* chessEngine): player(isWhite), max_depth(depth), iterations(itt), base_agent(chessEngine) {}

    void initialize() override {
        std::srand(std::time(0));
        std::cout << "MonteCarlo Chess Engine initialized.\n";
    }

    void makeMove(Board* bord) override {
        //create empty root node
        MCTS_Node root = MCTS_Node(nullptr, nullptr);
        for (int i = 0; i<iterations; i++){
            Board itterBoard;
            copyBoard(bord, &itterBoard);
            // SELECTION
            // We choose each time the child with the highest UCT-value
            // as soon as we meet a child that isn't expanded (leaf node)
            // we will expand this node
            MCTS_Node* node = &root;

            while(!node->children.empty() && !isEnded(&itterBoard)){
                auto selectedNodeIterator = std::max_element(node->children.begin(), node->children.end(),
                                                             [](const MCTS_Node& a, const MCTS_Node& b) {
                                                                 return a.uct() < b.uct();
                                                             });

                node = &(*selectedNodeIterator);
                movePiece(&itterBoard, node->getAction());
            }
            bool node_turn = itterBoard.whiteToPlay;

            // EXPANSION
            // We generate all the child nodes for the selected node
            if(!isEnded(&itterBoard)){
                node->children.clear();
                ActionList actionList;
                getLegalMoves(&itterBoard, &actionList);
                for (int childNumber = 0; childNumber < actionList.count; childNumber++){
                    node->children.emplace_back(node,&actionList.moves[childNumber]);
                    // The children wil be shuffled to prevent bias
                    // An equivalent solution would be to chose in the selection step a random unexplored child instead of the first
                    std::random_device rd;
                    std::mt19937 rng(rd());
                    std::shuffle(node->children.begin(), node->children.end(), rng);
                }
            }

            // ROLLOUT
            // we simulate the future evolution of the game
            // for this we let the base agent play for each player.

            int rollout_depth = 0;
            while (!isEnded(&itterBoard) && rollout_depth < max_depth) {
                rollout_depth += 1;
                // initiate the base agent to play like the current player
                // base_agent.player = itterBoard.whiteToPlay;
                // make a move
                base_agent->makeMove(&itterBoard);
            }
            //  Om de score van het resultaat te bepalen, moeten we de base_agent weer op onze speler instellen
            //self.base_agent.player = self.player
            //reward = self.base_agent.score(iter_board)
            int reward = evaluateBoard(bord);

            // NEGAMAX BACKPROPAGATION
            // if it is our turn at the beginning of the rollout, this means that the last move is from our opponent => flag = -1

            int flag = node_turn == player ? -1 : 1;
            while (node){
                node->visit();
                // update the node value withe running mean
                node->updateValue(flag, reward);
                node = node->getParent();
            }
        }
        // make a move with the current MCTS-tree
        // the best move is equal to the child with the highest expected reward
        auto selectedNodeIterator = std::max_element(root.children.begin(), root.children.end(),
                                                     [](const MCTS_Node& a, const MCTS_Node& b) {
                                                         return a.getValue() < b.getValue();
                                                     });

        MCTS_Node* node = &(*selectedNodeIterator); //TODO check
        //printAction(node->getAction());
        if(node->getAction() != nullptr){
            movePiece(bord, node->getAction());
        }
        //std::cout << "MonteCarlo move made.\n";
    }
public:
private:
    bool player; // for which player do we get rewards (true for white false for black)
    int max_depth; // max rollout
    int iterations; // amount of iterations per move
    ChessEngine* base_agent;  // agent that we use as default policy
};

