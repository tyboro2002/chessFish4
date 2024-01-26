#include "MonteCarloEngine.h"


double MCTS_Node::uct() const{
    /* Upper Confidence Bound for trees formule */
    if (visits > 0 and parent != nullptr) return value + 2 * UCT_CONST * sqrt(2 * log(parent->visits) / visits);
    else return DOUBLE_MAX;
}
