#include "Map.h"
#include <iostream>

Map::Map(int rows, int cols, float cellSize)
: rows_(rows), cols_(cols), cellSize_(cellSize)
{
    int raw[10][10] = {
        {1,1,1,1,1,1,1,1,1,1},
        {1,2,0,0,4,0,0,0,3,1},
        {1,0,1,0,1,0,1,0,0,1},
        {1,0,1,0,1,0,1,1,0,1},
        {1,0,0,0,0,0,0,1,0,1},
        {1,0,1,1,1,1,0,1,0,1},
        {1,0,0,4,0,0,0,1,0,1},
        {1,0,1,1,1,1,0,1,0,1},
        {1,4,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1}
    };
    grid_.resize(rows_, std::vector<int>(cols_, 0));
    for(int r = 0; r < rows_; ++r)
        for(int c = 0; c < cols_; ++c)
            grid_[r][c] = raw[r][c];
}

void Map::draw(sf::RenderWindow& window) {
    sf::RectangleShape cell(sf::Vector2f(cellSize_-2, cellSize_-2));
    sf::CircleShape itemShape(cellSize_/4);
    for(int r = 0; r < rows_; ++r) {
        for(int c = 0; c < cols_; ++c) {
            float x = c * cellSize_ + 1;
            float y = r * cellSize_ + 1;
            switch(grid_[r][c]) {
                case 1: // pared
                    cell.setPosition(x, y);
                    cell.setFillColor(sf::Color(100,100,100));
                    window.draw(cell);
                    break;
                case 3: // meta
                    cell.setPosition(x, y);
                    cell.setFillColor(sf::Color::Blue);
                    window.draw(cell);
                    break;
                case 4: // ítem
                    itemShape.setPosition(x + cellSize_/4, y + cellSize_/4);
                    itemShape.setFillColor(sf::Color::Green);
                    window.draw(itemShape);
                    break;
                default: break;
            }
        }
    }
}

bool Map::isWall(int row, int col) const {
    return grid_[row][col] == 1;
}

bool Map::isItem(int row, int col) const {
    return grid_[row][col] == 4;
}

void Map::removeItem(int row, int col) {
    if(grid_[row][col] == 4)
        grid_[row][col] = 0;
}

std::pair<int,int> Map::getStart() const {
    for(int r=0;r<rows_;++r)
        for(int c=0;c<cols_;++c)
            if(grid_[r][c]==2)
                return {r,c};
    return {0,0};
}

std::pair<int,int> Map::getGoal() const {
    for(int r=0;r<rows_;++r)
        for(int c=0;c<cols_;++c)
            if(grid_[r][c]==3)
                return {r,c};
    return {rows_-1,cols_-1};
}
