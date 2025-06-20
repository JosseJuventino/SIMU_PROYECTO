// src/Map.cpp
#include "Map.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

// Constructor lee el fichero
Map::Map(const std::string &filename, float cellSize)
    : cellSize_(cellSize)
{
    std::ifstream in(filename);
    if (!in)
        throw std::runtime_error("No se pudo abrir mapa: " + filename);

    std::string line;
    grid_.clear();
    while (std::getline(in, line))
    {
        std::istringstream ss(line);
        std::vector<int> row;
        int val;
        while (ss >> val)
            row.push_back(val);
        if (!grid_.empty() && row.size() != grid_[0].size())
            throw std::runtime_error("Filas con distinto número de columnas en " + filename);
        grid_.push_back(row);
    }
    rows_ = static_cast<int>(grid_.size());
    cols_ = rows_ > 0 ? static_cast<int>(grid_[0].size()) : 0;
    if (rows_ == 0 || cols_ == 0)
        throw std::runtime_error("Mapa vacío o mal formateado: " + filename);
}

void Map::draw(sf::RenderWindow &window)
{
    sf::RectangleShape cell(sf::Vector2f(cellSize_ - 2, cellSize_ - 2));
    sf::CircleShape itemShape(cellSize_ / 4.f);
    for (int r = 0; r < rows_; ++r)
    {
        for (int c = 0; c < cols_; ++c)
        {
            float x = c * cellSize_ + 1.f;
            float y = r * cellSize_ + 1.f;
            switch (grid_[r][c])
            {
            case 1: // pared
                cell.setPosition(x, y);
                cell.setFillColor(sf::Color(100, 100, 100));
                window.draw(cell);
                break;
            case 3: // meta
                cell.setPosition(x, y);
                cell.setFillColor(sf::Color::Blue);
                window.draw(cell);
                break;
            case 4: // ítem
                itemShape.setPosition(x + cellSize_ / 4.f, y + cellSize_ / 4.f);
                itemShape.setFillColor(sf::Color::Green);
                window.draw(itemShape);
                break;
            default:
                break;
            }
        }
    }
}

bool Map::isWall(int row, int col) const
{
    return grid_[row][col] == 1;
}

bool Map::isItem(int row, int col) const
{
    return grid_[row][col] == 4;
}

void Map::removeItem(int row, int col)
{
    if (grid_[row][col] == 4)
        grid_[row][col] = 0;
}

std::pair<int, int> Map::getStart() const
{
    for (int r = 0; r < rows_; ++r)
        for (int c = 0; c < cols_; ++c)
            if (grid_[r][c] == 2)
                return {r, c};
    return {0, 0};
}

std::pair<int, int> Map::getGoal() const
{
    for (int r = 0; r < rows_; ++r)
        for (int c = 0; c < cols_; ++c)
            if (grid_[r][c] == 3)
                return {r, c};
    return {rows_ - 1, cols_ - 1};
}
