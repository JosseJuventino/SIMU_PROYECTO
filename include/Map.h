#ifndef MAP_H
#define MAP_H

#include <vector>
#include <utility>
#include <SFML/Graphics.hpp>

class Map {
public:
    Map(int rows, int cols, float cellSize);
    void draw(sf::RenderWindow& window);
    bool isWall(int row, int col) const;
    bool isItem(int row, int col) const;
    void removeItem(int row, int col);
    std::pair<int,int> getStart() const;
    std::pair<int,int> getGoal() const;

private:
    int rows_, cols_;
    float cellSize_;
    std::vector<std::vector<int>> grid_;
};

#endif
