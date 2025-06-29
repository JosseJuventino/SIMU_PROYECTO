#ifndef GRID_H
#define GRID_H

#include <SFML/Graphics.hpp>

class Grid {
public:
    Grid(int rows, int cols, float cellSize);

    void draw(sf::RenderWindow& window);

private:
    int rows_, cols_;
    float cellSize_;
    sf::VertexArray lines_;
};

#endif // GRID_H
