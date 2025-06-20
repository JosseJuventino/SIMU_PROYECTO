#include <SFML/Graphics.hpp>
#include <iostream>
#include "Grid.h"
#include "Map.h"

int main(){
     const float CELL = 50.f;
    Map map("maps/map1.txt", CELL);
    int ROWS = map.getRows();
    int COLS = map.getCols();
    
    sf::RenderWindow window(
        sf::VideoMode(COLS * CELL, ROWS * CELL),
        "Escape the Grid"
    );
    window.setFramerateLimit(60);

    Grid grid(ROWS, COLS, CELL);

    auto [startR, startC] = map.getStart();
    int pr = startR, pc = startC;
    sf::RectangleShape player(sf::Vector2f(CELL-2, CELL-2));
    player.setFillColor(sf::Color::Red);
    player.setPosition(pc * CELL + 1, pr * CELL + 1);

    int itemsCollected = 0;

    while (window.isOpen()) {
        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed)
                window.close();

            if (evt.type == sf::Event::KeyPressed) {
                int nr = pr, nc = pc;
                if (evt.key.code == sf::Keyboard::Left)  nc--;
                if (evt.key.code == sf::Keyboard::Right) nc++;
                if (evt.key.code == sf::Keyboard::Up)    nr--;
                if (evt.key.code == sf::Keyboard::Down)  nr++;

                // colisión pared
                if (nr>=0 && nr<ROWS && nc>=0 && nc<COLS && !map.isWall(nr,nc)) {
                    pr = nr; pc = nc;
                    player.setPosition(pc*CELL+1, pr*CELL+1);

                    // recoger ítem
                    if (map.isItem(pr,pc)) {
                        map.removeItem(pr,pc);
                        itemsCollected++;
                        std::cout << "Ítems recogidos: " << itemsCollected << "\n";
                    }
                }
            }
        }

        window.clear(sf::Color::Black);
        grid.draw(window);
        map.draw(window);
        window.draw(player);
        window.display();
    }
    return 0;
}
