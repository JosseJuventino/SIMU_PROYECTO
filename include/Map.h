#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>
#include <utility>
#include <SFML/Graphics.hpp>

struct CloudBridge {
    std::pair<int,int> a, b;   // celdas que conecta
    bool active;               // si está levantado
};

class Map {
public:
    Map(const std::string& filename, float cellSize);

    void draw(sf::RenderWindow& window);

    bool isWall(int r, int c) const;
    bool isItem(int r, int c) const;
    void removeItem(int r, int c);

    std::pair<int,int> getStart() const;
    std::pair<int,int> getGoal()  const;
    std::vector<std::pair<int,int>> getItems() const;

    // mecánicas de puente/viento
    void updateWind();
    bool hasActiveBridge(int r1,int c1,int r2,int c2) const;

    int getRows() const { return rows_; }
    int getCols() const { return cols_; }
    void resetFromFile(const std::string& filename);

private:
    int rows_, cols_;
    float cellSize_;
    std::vector<std::vector<int>> grid_;
    struct CloudBridge {
        std::pair<int,int> a, b; // celdas que conecta
        bool active;             // si actualmente está levantado
    };

    // texturas y sprites
    sf::Texture floorTex_, wallTex_, itemTex_, goalTex_;
    sf::Sprite  floorSpr_, wallSpr_, itemSpr_, goalSpr_;
    sf::Texture bridgeTex_;
    sf::Sprite  bridgeSpr_;

    // puentes de nube
    std::vector<CloudBridge> bridges_;
    int windDir_;      // 0=N,1=E,2=S,3=W
    int windCounter_;  // turns counter
};

#endif // MAP_H
