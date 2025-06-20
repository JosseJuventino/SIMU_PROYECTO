#include <SFML/Graphics.hpp>
#include "Grid.h"

int main(){
    const int ROWS = 10;
    const int COLS = 10;
    const float CELL = 50.f;
    sf::RenderWindow window(
        sf::VideoMode(COLS * CELL, ROWS * CELL),
        "Escape the Grid"
    );
    window.setFramerateLimit(60);

    Grid grid(ROWS, COLS, CELL);

    // Player: un cuadrado rojo que ocupa una celda
    sf::RectangleShape player(sf::Vector2f(CELL - 2, CELL - 2));
    player.setFillColor(sf::Color::Red);
    int px = 0, py = 0;
    player.setPosition(1.f, 1.f);

    while (window.isOpen()) {
        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed)
                window.close();
            // Movimiento con flechas
            if (evt.type == sf::Event::KeyPressed) {
                if (evt.key.code == sf::Keyboard::Left  && px > 0)       --px;
                if (evt.key.code == sf::Keyboard::Right && px < COLS-1) ++px;
                if (evt.key.code == sf::Keyboard::Up    && py > 0)       --py;
                if (evt.key.code == sf::Keyboard::Down  && py < ROWS-1) ++py;
                player.setPosition(px * CELL + 1, py * CELL + 1);
            }
        }

        window.clear(sf::Color::Black);
        grid.draw(window);
        window.draw(player);
        window.display();
    }

    return 0;
}
