#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>
#include <utility>
#include <SFML/Graphics.hpp>

class Map {
public:
    Map(const std::string& filename, float cellSize);
    void draw(sf::RenderWindow& window);
    bool isWall(int row, int col) const;
    bool isItem(int row, int col) const;
    void removeItem(int row, int col);
    std::pair<int,int> getStart() const;
    std::pair<int,int> getGoal() const;

    int getRows() const { return rows_; }
    int getCols() const { return cols_; }

private:
    int rows_, cols_;
    float cellSize_;
    std::vector<std::vector<int>> grid_;
};

#endif
