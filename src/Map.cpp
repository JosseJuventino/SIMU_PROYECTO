#include "Map.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

Map::Map(const std::string &filename, float cellSize)
    : cellSize_(cellSize), windDir_(0), windCounter_(0)
{
    // 1) Lee matriz desde fichero
    std::ifstream in(filename);
    if (!in)
        throw std::runtime_error("No se pudo abrir mapa: " + filename);
    std::string line;
    while (std::getline(in, line))
    {
        std::istringstream ss(line);
        std::vector<int> row;
        int v;
        while (ss >> v)
            row.push_back(v);
        if (!grid_.empty() && row.size() != grid_[0].size())
            throw std::runtime_error("Columnas inconsistentes en " + filename);
        grid_.push_back(row);
    }
    rows_ = int(grid_.size());
    cols_ = rows_ ? int(grid_[0].size()) : 0;
    if (!rows_ || !cols_)
        throw std::runtime_error("Mapa mal formateado");

    // 2) Carga texturas
    struct TL
    {
        sf::Texture &t;
        const char *path;
    };
    TL loads[] = {
        {floorTex_, "assets/sprites/floor.png"},
        {wallTex_, "assets/sprites/wall.png"},
        {itemTex_, "assets/sprites/coin.png"},
        {goalTex_, "assets/sprites/finish.png"},
        {bridgeTex_, "assets/sprites/sky_bridge.png"}};
    for (auto &tl : loads)
    {
        if (!tl.t.loadFromFile(tl.path))
        {
            std::cerr << "ERROR: no pude cargar " << tl.path << "\n";
            throw std::runtime_error("Fallo loadFromFile");
        }
    }
    float scale = cellSize_ / float(floorTex_.getSize().x);
    floorSpr_.setTexture(floorTex_);
    floorSpr_.setScale(scale, scale);
    wallSpr_.setTexture(wallTex_);
    wallSpr_.setScale(scale, scale);
    itemSpr_.setTexture(itemTex_);
    itemSpr_.setScale(scale, scale);
    goalSpr_.setTexture(goalTex_);
    goalSpr_.setScale(scale, scale);
    bridgeSpr_.setTexture(bridgeTex_);
    bridgeSpr_.setScale(scale, scale);

    bridges_ = {
        {{2, 2}, {2, 3}, false},
        {{4, 5}, {5, 5}, false},
        {{7, 1}, {7, 2}, false},
    };

    windCounter_ = (5 - 1);  
    updateWind();
}

void Map::draw(sf::RenderWindow &window)
{
    // tiles
    for (int r = 0; r < rows_; ++r)
    {
        for (int c = 0; c < cols_; ++c)
        {
            float x = c * cellSize_, y = r * cellSize_;
            floorSpr_.setPosition(x, y);
            window.draw(floorSpr_);
            switch (grid_[r][c])
            {
            case 1:
                wallSpr_.setPosition(x, y);
                window.draw(wallSpr_);
                break;
            case 3:
                goalSpr_.setPosition(x, y);
                window.draw(goalSpr_);
                break;
            case 4:
                itemSpr_.setPosition(x, y);
                window.draw(itemSpr_);
                break;
            default:
                break;
            }
        }
    }
    // puentes activos
    for (auto &br : bridges_)
    {
        if (!br.active)
            continue;
        float x1 = (br.a.second + br.b.second + 1.f) * cellSize_ / 2.f;
        float y1 = (br.a.first + br.b.first + 1.f) * cellSize_ / 2.f;
        bridgeSpr_.setOrigin(bridgeTex_.getSize().x / 2.f,
                             bridgeTex_.getSize().y / 2.f);
        bridgeSpr_.setPosition(x1, y1);
        // rota horizontal/vertical
        if (br.a.first == br.b.first)
            bridgeSpr_.setRotation(0.f);
        else
            bridgeSpr_.setRotation(90.f);
        window.draw(bridgeSpr_);
    }
}

bool Map::isWall(int r, int c) const
{
    return grid_[r][c] == 1;
}

bool Map::hasActiveBridge(int r1, int c1, int r2, int c2) const
{
    for (auto &br : bridges_)
    {
        if (!br.active)
            continue;
        if ((br.a.first == r1 && br.a.second == c1 && br.b.first == r2 && br.b.second == c2) ||
            (br.b.first == r1 && br.b.second == c1 && br.a.first == r2 && br.a.second == c2))
            return true;
    }
    return false;
}

bool Map::isItem(int r, int c) const
{
    return grid_[r][c] == 4;
}

void Map::removeItem(int r, int c)
{
    if (grid_[r][c] == 4)
        grid_[r][c] = 0;
}

std::pair<int, int> Map::getStart() const
{
    for (int r = 0; r < rows_; ++r)
        for (int c = 0; c < cols_; ++c)
            if (grid_[r][c] == 2)
                return {r, c};
    return {0, 0};
}

std::vector<std::pair<int, int>> Map::getItems() const
{
    std::vector<std::pair<int, int>> items;
    for (int r = 0; r < rows_; ++r)
        for (int c = 0; c < cols_; ++c)
            if (grid_[r][c] == 4)
                items.emplace_back(r, c);
    return items;
}

std::pair<int, int> Map::getGoal() const
{
    for (int r = 0; r < rows_; ++r)
        for (int c = 0; c < cols_; ++c)
            if (grid_[r][c] == 3)
                return {r, c};
    return {rows_ - 1, cols_ - 1};
}

void Map::updateWind()
{
    constexpr int WIND_PERIOD = 5;
    windCounter_ = (windCounter_ + 1) % WIND_PERIOD;
    if (windCounter_ != 0)
        return;
    windDir_ = (windDir_ + 1) % 4;
    for (auto &br : bridges_)
    {
        int dr = br.b.first - br.a.first;
        int dc = br.b.second - br.a.second;
        // paridad: 0,2 -> vertical; 1,3 -> horizontal
        br.active = (windDir_ % 2 == 0 ? (dc == 0) : (dr == 0));
    }
}
