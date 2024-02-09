#pragma once

#include "ChessEngine.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <memory>
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
    std::vector<std::unique_ptr<MCTS_Node>> children;
    /* a node in the MCTS-gameTree */
    MCTS_Node(MCTS_Node* parentArg, const Action* move): parent(parentArg), action(move){};

    [[nodiscard]] double uct() const;
    void visit() {visits++;}
    void updateValue (int flag, int reward) {value += (flag * reward - value) / visits;}
    [[nodiscard]] MCTS_Node* getParent() const {return parent;}
    [[nodiscard]] double getValue() const {return value;}
    [[nodiscard]] const Action* getAction() const {return action;}

    void printMCTSTree(int depth = 0) const {
        for (int i = 0; i < depth; ++i) {
            std::cout << "  ";  // Add indentation based on the depth
        }

        std::cout << "Action: ";
        if (action != nullptr) {
            printActionInline(action);
        } else {
            std::cout << "None";
        }

        std::cout << " | Value: " << value << " | Visits: " << visits << " | children:  " << children.size() <<std::endl;

        // Recursively print children
        for (const auto& child : children) child->printMCTSTree(depth + 1);
    }

    void printChildren() const {
        std::cout << "Children: " << children.size() << std::endl;
        for (const auto& child : children) {
            std::cout << "  Action: ";
            if (child->action != nullptr) {
                printActionInline(child->action);
            } else {
                std::cout << "None";
            }
            std::cout << " | Value: " << child->value << " | Visits: " << child->visits << std::endl;
        }
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
        MCTS_Node root = MCTS_Node(nullptr, nullptr);
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
                printAction(node->getAction());
                movePiece(&iterBoard, node->getAction());
            }

            bool node_turn = iterBoard.whiteToPlay;
            std::cout << "node_turn: " << node_turn << std::endl;

            printFancyBoard(&iterBoard);

            node->printMCTSTree();

            std::cout << "root: " << std::endl;
            root.printChildren(); //stil okay (TODO remove)

            // EXPANSION
            // We generate all the child nodes for the selected node
            if(!isEnded(&iterBoard)){
                //node->children.clear();
                std::cout << "after clearing childs" << std::endl;
                root.printMCTSTree();
                node->printMCTSTree();
                ActionList actionList;
                std::cout << "location of actionList: " << &actionList << " location of node: " << &node << " location of node children: " << &node->children << std::endl;
                std::cout << "creating action list" << std::endl;
                root.printMCTSTree();
                node->printMCTSTree();
                getLegalMoves(&iterBoard, &actionList);
                std::cout << "before adding childs" << std::endl;
                root.printMCTSTree();
                node->printMCTSTree();
                for (int childNumber = 0; childNumber < actionList.count; childNumber++){
                    auto newNode = std::make_unique<MCTS_Node>(node, &actionList.moves[childNumber]);
                    node->children.push_back(std::move(newNode));

                    std::cout << "after adding child: " << childNumber << std::endl;
                    node->printMCTSTree();
                    // The children wil be shuffled to prevent bias
                    // An equivalent solution would be to chose in the selection step a random unexplored child instead of the first
                    std::random_device rd;
                    std::mt19937 rng(rd());
                    //std::shuffle(node->children.begin(), node->children.end(), rng); //TODO place back
                }
            }

            // ROLLOUT
            // we simulate the future evolution of the game
            // for this we let the base agent play for each player.

            int rollout_depth = 0;
            while (!isEnded(&iterBoard) && rollout_depth < max_depth) {
                rollout_depth ++;
                // initiate the base agent to play like the current player
                // base_agent.player = iterBoard.whiteToPlay;
                // make a move
                Action action = base_agent->getPreferredAction(&iterBoard);
                movePiece(&iterBoard, &action);
            }
            //  Om de score van het resultaat te bepalen, moeten we de base_agent weer op onze speler instellen
            //self.base_agent.player = self.player
            //reward = self.base_agent.score(iter_board)
            int reward = evaluateBoard(&iterBoard);

            // NEGAMAX BACKPROPAGATION
            // if it is our turn at the beginning of the rollout, this means that the last move is from our opponent => flag = -1

            int flag = (node_turn == player) ? -1 : 1;
            while (node->getParent()){
                node->visit();
                // update the node value withe running mean
                node->updateValue(flag, reward);
                node = node->getParent();
            }

            /* print out the tree for debugging */
            std::cout << "after itt " << i << " the tree is " << std::endl;
            root.printMCTSTree();
            std::cout << "---------------------------------" << std::endl;
        }

        // make a move with the current MCTS-tree
        // the best move is equal to the child with the highest expected reward
        auto selectedNodeIterator = std::max_element(root.children.begin(), root.children.end(),
                                                     [](const std::unique_ptr<MCTS_Node>& a, const std::unique_ptr<MCTS_Node>& b) {
                                                         return a->getValue() < b->getValue();
                                                     });

        root.printChildren();

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

