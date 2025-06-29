#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "Grid.h"
#include "Map.h"
#include "PathFinder.h"

int main()
{
    srand(unsigned(time(nullptr)));
    const float CELL = 50.f;

    // Carga inicial del mapa
    const std::string MAP_FILE = "maps/map1.txt";
    Map map(MAP_FILE, CELL);
    int ROWS = map.getRows(), COLS = map.getCols();

    sf::RenderWindow window(
        sf::VideoMode(COLS * CELL, ROWS * CELL + 60),
        "Escape the Grid");
    window.setFramerateLimit(60);

    // Grid para las líneas
    Grid grid(ROWS, COLS, CELL);

    // Sprite jugador
    sf::Texture playerTex;
    if (!playerTex.loadFromFile("assets/sprites/character.png")) {
        std::cerr << "ERROR: no pude cargar character.png\n";
        return 1;
    }
    sf::Sprite playerSpr(playerTex);
    float pscale = CELL / float(playerTex.getSize().x);
    playerSpr.setScale(pscale, pscale);

    // Posición inicial del jugador
    auto [sr, sc] = map.getStart();
    int pr = sr, pc = sc;
    playerSpr.setPosition(pc * CELL, pr * CELL);

    // HUD
    int itemsCollected = 0;
    sf::Font font;
    if (!font.loadFromFile("assets/fonts/LiberationSans-Regular.ttf")) {
        std::cerr << "ERROR: no pude cargar la fuente\n";
        return 1;
    }
    sf::Text hud("", font, 20);
    hud.setFillColor(sf::Color::White);
    hud.setPosition(5, ROWS * CELL + 5);

    // Ruta y flag
    std::vector<std::pair<int,int>> path;
    bool showPath = false;

    // Botón Mostrar/Ocultar ruta
    sf::RectangleShape button({100, 30});
    button.setFillColor({70,70,70});
    button.setOutlineColor(sf::Color::White);
    button.setOutlineThickness(1);
    sf::Vector2f btnPos(COLS * CELL - 110, ROWS * CELL + 5);
    button.setPosition(btnPos);
    sf::Text btnText("Mostrar ruta", font, 16);
    btnText.setFillColor(sf::Color::White);
    btnText.setPosition(btnPos + sf::Vector2f(10,5));

    // Texto viento
    sf::Text windText("", font, 20);
    windText.setFillColor(sf::Color::Cyan);
    windText.setPosition(5, ROWS * CELL + 25);

    // Shape para pintar la ruta
    sf::RectangleShape stepShape({CELL - 2, CELL - 2});
    stepShape.setFillColor({255,255,0,150});

    int turnCount = 0;
    constexpr int EVENT_PERIOD = 2;
    constexpr int MAX_BATTERY = 100;
    int battery = MAX_BATTERY;

    bool gameOver = false;
    std::string gameMessage;

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            }

            // CLICK EN BOTÓN
            if (e.type == sf::Event::MouseButtonPressed &&
                e.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f m(e.mouseButton.x, e.mouseButton.y);
                if (button.getGlobalBounds().contains(m)) {
                    showPath = !showPath;
                    if (showPath) {
                        button.setFillColor({150,150,150});
                        btnText.setString("Ocultar ruta");
                        path = findPathBFS(map, {pr,pc}, map.getGoal());
                    } else {
                        button.setFillColor({70,70,70});
                        btnText.setString("Mostrar ruta");
                        path.clear();
                    }
                }
            }

            // RESET con R
            if (gameOver &&
                e.type == sf::Event::KeyPressed &&
                e.key.code == sf::Keyboard::R)
            {
                // 1) Recarga únicamente el grid interno del mapa
                map.resetFromFile(MAP_FILE);

                // 2) Reposiciona jugador e indicadores
                std::tie(pr, pc) = map.getStart();
                playerSpr.setPosition(pc * CELL, pr * CELL);

                battery = MAX_BATTERY;
                turnCount = 0;
                itemsCollected = 0;
                showPath = false;
                path.clear();
                gameOver = false;

                // El grid de líneas no cambia si ROWS/COLS son iguales. 
                // Si cambian, habría que recrearlo:
                ROWS = map.getRows();
                COLS = map.getCols();
                grid = Grid(ROWS, COLS, CELL);

                // Reajusta HUD y botón
                hud.setPosition(5, ROWS * CELL + 5);
                btnPos = { float(COLS * CELL - 110), float(ROWS * CELL + 5) };
                button.setPosition(btnPos);
                btnText.setPosition(btnPos + sf::Vector2f(10,5));
                button.setFillColor({70,70,70});
                btnText.setString("Mostrar ruta");
            }

            // MOVIMIENTO DEL JUGADOR
            if (!gameOver &&
                e.type == sf::Event::KeyPressed)
            {
                int nr = pr, nc = pc;
                bool moved = false;
                switch (e.key.code) {
                    case sf::Keyboard::Left:  nc--; moved = true; break;
                    case sf::Keyboard::Right: nc++; moved = true; break;
                    case sf::Keyboard::Up:    nr--; moved = true; break;
                    case sf::Keyboard::Down:  nr++; moved = true; break;
                    default: break;
                }
                if (moved && battery > 0) {
                    if (map.isWalkable(nr,nc)) {
                        pr = nr; pc = nc;
                        playerSpr.setPosition(pc * CELL, pr * CELL);
                        battery--;
                        turnCount++;
                        if (turnCount % EVENT_PERIOD == 0) {
                            map.updateWind();
                            if (showPath)
                                path = findPathBFS(map, {pr,pc}, map.getGoal());
                        }
                        if (map.isItem(pr,pc)) {
                            map.removeItem(pr,pc);
                            itemsCollected++;
                        }
                        if (showPath)
                            path = findPathBFS(map, {pr,pc}, map.getGoal());
                        if (std::make_pair(pr,pc) == map.getGoal()) {
                            gameOver = true;
                            gameMessage = "GANASTE";
                        }
                    }
                    if (battery <= 0 && !gameOver) {
                        gameOver = true;
                        gameMessage = "SIN BATERIA";
                    }
                }
            }
        }

        // ACTUALIZA HUD y viento
        hud.setString(
            "Items: " + std::to_string(itemsCollected) +
            "   Batería: " + std::to_string(battery)
        );
        const char* dirStr[] = {"Norte","Este","Sur","Oeste"};
        windText.setString("Viento: " + std::string(dirStr[map.getWindDir()]));

        // DIBUJA ESCENA
        window.clear(sf::Color::Black);
        grid.draw(window);
        map.draw(window);

        if (showPath) {
            // recalcula y pinta ruta cada frame
            path = findPathBFS(map, {pr,pc}, map.getGoal());
            for (auto [r,c] : path) {
                stepShape.setPosition(c * CELL + 1, r * CELL + 1);
                window.draw(stepShape);
            }
        }

        window.draw(playerSpr);
        window.draw(hud);
        window.draw(button);
        window.draw(btnText);
        window.draw(windText);

        if (gameOver) {
            sf::Text endText(gameMessage + " - Presiona R", font, 32);
            endText.setStyle(sf::Text::Bold);
            endText.setFillColor(
                gameMessage=="GANASTE" ? sf::Color::Green : sf::Color::Red
            );
            endText.setOutlineColor(sf::Color::Black);
            endText.setOutlineThickness(2);
            auto b = endText.getLocalBounds();
            endText.setOrigin(b.width/2, b.height/2);
            endText.setPosition(COLS * CELL / 2.f, ROWS * CELL / 2.f);
            window.draw(endText);
        }

        window.display();
    }

    return 0;
}
