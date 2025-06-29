#include "Map.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>

Map::Map(const std::string &filename, float cellSize)
    : cellSize_(cellSize)
    , windDir_(Direction::North)
    , turnCounter_(0)
    , windInterval_(5)
{
    // 1) Carga el grid
    std::ifstream in(filename);
    if (!in) throw std::runtime_error("No se pudo abrir mapa: " + filename);
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream ss(line);
        std::vector<int> row;
        int v;
        while (ss >> v) row.push_back(v);
        if (!grid_.empty() && row.size() != grid_[0].size())
            throw std::runtime_error("Columnas inconsistentes en " + filename);
        grid_.push_back(row);
    }
    rows_ = int(grid_.size());
    cols_ = rows_ ? int(grid_[0].size()) : 0;
    if (!rows_ || !cols_) throw std::runtime_error("Mapa mal formateado");

    // 2) Carga texturas
    struct TL { sf::Texture &tex; const char *path; };
    TL loads[] = {
        { floorTex_,   "assets/sprites/floor.png"      },
        { wallTex_,    "assets/sprites/wall.png"       },
        { itemTex_,    "assets/sprites/coin.png"       },
        { goalTex_,    "assets/sprites/finish.png"     },
        { bridgeTex_,  "assets/sprites/sky_bridge.png" }
    };
    for (auto &tl : loads) {
        if (!tl.tex.loadFromFile(tl.path)) {
            std::cerr << "ERROR: no pude cargar " << tl.path << "\n";
            throw std::runtime_error("Fallo loadFromFile");
        }
    }
    float scale = cellSize_ / float(floorTex_.getSize().x);
    floorSpr_.setTexture(floorTex_);   floorSpr_.setScale(scale, scale);
    wallSpr_.setTexture(wallTex_);     wallSpr_.setScale(scale, scale);
    itemSpr_.setTexture(itemTex_);     itemSpr_.setScale(scale, scale);
    goalSpr_.setTexture(goalTex_);     goalSpr_.setScale(scale, scale);
    bridgeSpr_.setTexture(bridgeTex_); bridgeSpr_.setScale(scale, scale);

    // 3) Inicializa nubes
    initClouds(6);
}

void Map::initClouds(int count) {
    std::mt19937 mt{ unsigned(std::time(nullptr)) };
    std::uniform_int_distribution<> dr(0, rows_-1), dc(0, cols_-1);
    clouds_.clear();
    while ((int)clouds_.size() < count) {
        int r = dr(mt), c = dc(mt);
        // sólo sobre muros (valor 1)
        if (grid_[r][c] == 1)
            clouds_.emplace_back(c, r);
    }
}

void Map::changeWind() {
    windDir_ = Direction((int(windDir_) + 1) % 4);
}

void Map::moveClouds() {
    std::vector<sf::Vector2i> next;
    next.reserve(clouds_.size());
    for (auto &p : clouds_) {
        sf::Vector2i q = p;
        switch (windDir_) {
            case Direction::North: q.y--; break;
            case Direction::South: q.y++; break;
            case Direction::East:  q.x++; break;
            case Direction::West:  q.x--; break;
        }
        int r = q.y, c = q.x;
        // Mueve la nube sólo si sigue siendo un muro (1)
        if (r>=0 && r<rows_ && c>=0 && c<cols_ && grid_[r][c] == 1)
            next.push_back(q);
        else
            next.push_back(p);
    }
    clouds_.swap(next);
}

void Map::resetFromFile(const std::string &filename) {
    // 1) Recarga grid_
    grid_.clear();
    std::ifstream in(filename);
    if (!in)
        throw std::runtime_error("No se pudo abrir mapa: " + filename);
    std::string line;
    while (std::getline(in, line)) {
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

    // 2) Reinicia viento y nubes
    turnCounter_ = 0;
    windDir_     = Direction::North;
    initClouds(6);
}

void Map::updateWind() {
    turnCounter_++;
    if (turnCounter_ % windInterval_ != 0) return;
    changeWind();
    moveClouds();
}

bool Map::isCloud(int r, int c) const {
    for (auto &p : clouds_)
        if (p.y == r && p.x == c)
            return true;
    return false;
}

bool Map::isWalkable(int r, int c) const {
    if (r<0 || r>=rows_ || c<0 || c>=cols_) return false;
    int type = grid_[r][c];
    // cualquier cosa que NO sea muro (1) es transitable:
    // césped(0), inicio(2), meta(3), items(4)…
    if (type != 1) return true;
    // o nube sobre muro
    if (isCloud(r,c)) return true;
    return false;
}

void Map::draw(sf::RenderWindow &window) {
    // suelo, muros, items, meta
    for (int r=0;r<rows_;++r) {
        for (int c=0;c<cols_;++c) {
            float x=c*cellSize_, y=r*cellSize_;
            floorSpr_.setPosition(x,y); window.draw(floorSpr_);
            if      (grid_[r][c]==1) { wallSpr_.setPosition(x,y); window.draw(wallSpr_); }
            else if (grid_[r][c]==3) { goalSpr_.setPosition(x,y); window.draw(goalSpr_); }
            else if (grid_[r][c]==4) { itemSpr_.setPosition(x,y); window.draw(itemSpr_); }
        }
    }
    // nubes
     for (auto &p : clouds_) {
        bridgeSpr_.setPosition(p.x * cellSize_, p.y * cellSize_);
        window.draw(bridgeSpr_);
    }
}

bool Map::isItem(int r, int c) const {
    return grid_[r][c] == 4;
}

void Map::removeItem(int r, int c) {
    if (grid_[r][c] == 4) grid_[r][c] = 0;
}

std::pair<int,int> Map::getStart() const {
    for (int r=0;r<rows_;++r)
      for(int c=0;c<cols_;++c)
        if(grid_[r][c]==2) return {r,c};
    return {0,0};
}

std::vector<std::pair<int,int>> Map::getItems() const {
    std::vector<std::pair<int,int>> v;
    for (int r=0;r<rows_;++r)
      for(int c=0;c<cols_;++c)
        if(grid_[r][c]==4) v.emplace_back(r,c);
    return v;
}

std::pair<int,int> Map::getGoal() const {
    for (int r=0;r<rows_;++r)
      for(int c=0;c<cols_;++c)
        if(grid_[r][c]==3) return {r,c};
    return {rows_-1,cols_-1};
}
