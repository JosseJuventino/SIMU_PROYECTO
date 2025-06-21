#include "Grid.h"

Grid::Grid(int rows, int cols, float cellSize)
: rows_(rows), cols_(cols), cellSize_(cellSize), lines_(sf::Lines)
{
    // Líneas verticales
    for (int i = 0; i <= cols_; ++i) {
        float x = i * cellSize_;
        lines_.append(sf::Vertex({ x, 0.f }));
        lines_.append(sf::Vertex({ x, rows_ * cellSize_ }));
    }
    // Líneas horizontales
    for (int j = 0; j <= rows_; ++j) {
        float y = j * cellSize_;
        lines_.append(sf::Vertex({ 0.f, y }));
        lines_.append(sf::Vertex({ cols_ * cellSize_, y }));
    }
}

void Grid::draw(sf::RenderWindow& window) {
    window.draw(lines_);
}
