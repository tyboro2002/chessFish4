#pragma once

#include <map>
#include "../game.h"

class OpeningBook {
public:
    void fillOpeningBookManualy();
    void fillOpeningBookFromDatabase(const std::string databasePath);


    std::vector<Action> getBestMoveFromOpeningBook(Board* board){
        size_t hashValue = std::hash<Board>{}(*board);
        if(table.find(hashValue) != table.end()) return table[hashValue];
        return {};
    }

    size_t getCurrentSize() const { return currentSize; }
    size_t getCurrentStoredPositions() const { return currentStoredPositions; }
    size_t getCurrentStoredActions() const { return currentStoredActions; }

    void addPositionToTable(Board* board, Action* action){
        size_t hashValue = std::hash<Board>{}(*board);

        if(table.find(hashValue) != table.end()){
            // Store the entry and update the size
            table[hashValue].push_back(*action);
        }else{
            std::vector<Action> possibilities;
            possibilities.push_back(*action);
            table[hashValue] = possibilities;
            currentStoredPositions++;
        }
        currentSize += sizeof(*action);
        currentStoredActions++;
    }

private:
    std::unordered_map<size_t, std::vector<Action>> table;
    size_t currentSize;
    size_t currentStoredPositions;
    size_t currentStoredActions;
};

