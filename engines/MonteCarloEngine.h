#pragma once

#include "ChessEngine.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <memory>
#include "queue"

#define DOUBLE_MAX std::numeric_limits<double>::max()
#define UCT_CONST 1.41421356237 //sqrt(2) //TODO check

class MCTS_Node {
    // parent of this node (None for the root)
    MCTS_Node* parent = nullptr;
    // action we took to get here from parent
    // we can use this to iteratively reconstruct the board corresponding to this node
    std::optional<Action> action;;
    double value = 0.0; // rolling mean of rewards from this node
    double visits = 0;  // amount of time a node is visited (used for calculation of UCT)

public:
    std::vector<std::unique_ptr<MCTS_Node>> children;
    /* a node in the MCTS-gameTree */
    MCTS_Node(MCTS_Node* parentArg, std::optional<Action> move): parent(parentArg), action(move){};

    inline void visit() {visits++;}
    inline void updateValue (int flag, int reward) {value += (flag * reward - value) / visits;}
    [[nodiscard]] inline MCTS_Node* getParent() const {return parent;}
    [[nodiscard]] inline double getValue() const {return value;}
    [[nodiscard]] inline std::optional<Action> getAction() const {return action;}

    inline void printMCTSTree(int depth = 0) const {
        for (int i = 0; i < depth; ++i) std::cout << "  ";  // Add indentation based on the depth

        std::cout << "Action: ";
        if (action.has_value()) printActionInline(action.value());
        else std::cout << "None";

        std::cout << " | Value: " << value << " | Visits: " << visits << " | children:  " << children.size() <<std::endl;

        // Recursively print children
        for (const auto& child : children) child->printMCTSTree(depth + 1);
    }

    inline void printChildren() const {
        std::cout << "Children: " << children.size() << std::endl;
        for (const auto& child : children) {
            std::cout << "  Action: ";
            if (child->action.has_value()) printActionInline(child->action.value());
            else std::cout << "None";
            std::cout << " | Value: " << child->value << " | Visits: " << child->visits << std::endl;
        }
    }
    [[nodiscard]] inline double uct() const{
        /* Upper Confidence Bound for trees formule */
        if (visits > 0 && parent != nullptr) return value + 2 * UCT_CONST * sqrt(2 * log(parent->visits) / visits);
        else return DOUBLE_MAX;
    }
};

class MonteCarloEngine : public ChessEngine {
public:
    MonteCarloEngine(bool isWhite, int depth, int itt, ChessEngine* chessEngine): player(isWhite), max_depth(depth), iterations(itt), base_agent(chessEngine) {}

    void initialize() override {
        std::srand(std::time(0));
        std::cout << "MonteCarlo Chess Engine initialized.\n";
    }

    Action getPreferredAction(Board* bord) override { //TODO children of the root node seems to change to the other players children
        //create empty root node
        MCTS_Node root = MCTS_Node(nullptr, std::nullopt);
        for (int i = 0; i<iterations; i++){
            Board iterBoard{};
            copyBoard(bord, &iterBoard);

            // SELECTION
            // We choose each time the child with the highest UCT-value
            // as soon as we meet a child that isn't expanded (leaf node)
            // we will expand this node
            MCTS_Node* node = &root;

            while(!node->children.empty() && !isEnded(&iterBoard)){
                auto selectedNodeIterator = std::max_element(node->children.begin(), node->children.end(),
                                                             [](const std::unique_ptr<MCTS_Node>& a, const std::unique_ptr<MCTS_Node>& b) {
                                                                 return a->uct() < b->uct();
                                                             });

                node = selectedNodeIterator->get();
                Action action = (node->getAction().value());
                movePiece(&iterBoard, &action);
            }

            bool node_turn = iterBoard.whiteToPlay;

            // EXPANSION
            // We generate all the child nodes for the selected node
            if(!isEnded(&iterBoard)){
                //node->children.clear();
                ActionList actionList;
                getLegalMoves(&iterBoard, &actionList);
                for (int childNumber = 0; childNumber < actionList.count; childNumber++){
                    auto newNode = std::make_unique<MCTS_Node>(node, actionList.moves[childNumber]);
                    node->children.push_back(std::move(newNode));

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
            while (!isEnded(&iterBoard) && rollout_depth < max_depth) {
                rollout_depth ++;
                // initiate the base agent to play like the current player
                // make a move
                Action action = base_agent->getPreferredAction(&iterBoard);
                movePiece(&iterBoard, &action);
            }
            //  Om de score van het resultaat te bepalen, moeten we de base_agent weer op onze speler instellen
            int reward = evaluateBoard(&iterBoard);

            // NEGAMAX BACKPROPAGATION
            // if it is our turn at the beginning of the rollout, this means that the last move is from our opponent => flag = -1

            int flag = (node_turn == player) ? -1 : 1;
            while (node){
                node->visit();
                // update the node value withe running mean
                node->updateValue(flag, reward);
                node = node->getParent();
            }

            /* print out the tree for debugging */
            //std::cout << "after itt " << i << " the tree is " << std::endl;
            //root.printMCTSTree();
            //std::cout << "---------------------------------" << std::endl;
        }

        // make a move with the current MCTS-tree
        // the best move is equal to the child with the highest expected reward
        auto selectedNodeIterator = std::max_element(root.children.begin(), root.children.end(),
                                                     [](const std::unique_ptr<MCTS_Node>& a, const std::unique_ptr<MCTS_Node>& b) {
                                                         return a->getValue() < b->getValue();
                                                     });


        MCTS_Node* node = selectedNodeIterator->get(); // Dereference the unique_ptr
        return *(node->getAction());

        //std::cout << "MonteCarlo move made.\n";
    }
public:
private:
    bool player; // for which player do we get rewards (true for white false for black)
    int max_depth; // max rollout
    int iterations; // amount of iterations per move
    ChessEngine* base_agent;  // agent that we use as default policy
};

