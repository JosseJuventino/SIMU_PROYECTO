#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <string>
#include <utility>

enum class Direction { North, East, South, West };

class Map {
public:
    Map(const std::string& filename, float cellSize);

    void draw(sf::RenderWindow& window);

    // Movimiento: suelo vacío o nube
    bool isWalkable(int r, int c) const;

    // Items
    bool isItem(int r, int c) const;
    void removeItem(int r, int c);

    // Viento
    int  getWindDir() const { return int(windDir_); }
    void updateWind();

    void resetFromFile(const std::string& filename);

    // Posiciones especiales
    std::pair<int,int>               getStart() const;
    std::pair<int,int>               getGoal()  const;
    std::vector<std::pair<int,int>>  getItems() const;

    int getRows() const { return rows_; }
    int getCols() const { return cols_; }

private:
    // Nubes flotantes
    void initClouds(int count);
    void changeWind();
    void moveClouds();
    bool isCloud(int r, int c) const;

    // Datos del mapa
    float                          cellSize_;
    int                            rows_, cols_;
    std::vector<std::vector<int>>  grid_;

    // Texturas y sprites
    sf::Texture  floorTex_, wallTex_, itemTex_, goalTex_, bridgeTex_;
    sf::Sprite   floorSpr_, wallSpr_, itemSpr_, goalSpr_, bridgeSpr_;

    // Estado de nubes
    std::vector<sf::Vector2i>  clouds_;
    Direction                  windDir_;
    int                        turnCounter_;
    int                        windInterval_;
};

#endif // MAP_H
