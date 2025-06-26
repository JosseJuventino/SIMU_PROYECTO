#include <SFML/Graphics.hpp>
#include <iostream>
#include "Grid.h"
#include "Map.h"
#include "PathFinder.h"

int main()
{
    const float CELL = 50.f;
    Map map("maps/map1.txt", CELL);
    int ROWS = map.getRows(), COLS = map.getCols();

    sf::RenderWindow window(
        sf::VideoMode(COLS * CELL, ROWS * CELL + 40),
        "Escape the Grid");
    window.setFramerateLimit(60);

    Grid grid(ROWS, COLS, CELL);

    sf::Texture playerTex;
    if (!playerTex.loadFromFile("assets/sprites/character.png"))
    {
        std::cerr << "ERROR: no pude cargar character.png\n";
        return 1;
    }
    sf::Sprite playerSpr(playerTex);
    float pscale = CELL / float(playerTex.getSize().x);
    playerSpr.setScale(pscale, pscale);

    auto [sr, sc] = map.getStart();
    int pr = sr, pc = sc;
    playerSpr.setPosition(pc * CELL, pr * CELL);

    int itemsCollected = 0;
    sf::Font font;
    if (!font.loadFromFile("assets/fonts/LiberationSans-Regular.ttf"))
    {
        std::cerr << "ERROR: no pude cargar la fuente\n";
        return 1;
    }
    sf::Text hud("", font, 20);
    hud.setFillColor(sf::Color::White);
    hud.setPosition(5, ROWS * CELL + 5);

    auto path = findCollectAllPath(map, {sr, sc}, map.getGoal());
    bool showPath = false;

    sf::RectangleShape button({100, 30});
    button.setFillColor({70, 70, 70});
    button.setOutlineColor(sf::Color::White);
    button.setOutlineThickness(1);
    sf::Vector2f btnPos(COLS * CELL - 110, ROWS * CELL + 5);
    button.setPosition(btnPos);
    sf::Text btnText("Mostrar ruta", font, 16);
    btnText.setFillColor(sf::Color::White);
    btnText.setPosition(btnPos + sf::Vector2f(10, 5));

    sf::RectangleShape stepShape({CELL - 2, CELL - 2});
    stepShape.setFillColor({255, 255, 0, 150});

    int turnCount = 0;
    constexpr int EVENT_PERIOD = 15;
    constexpr int MAX_BATTERY = 100;
    int battery = MAX_BATTERY;

    bool gameOver = false;
    std::string gameMessage = "";

    while (window.isOpen())
    {
        sf::Event e;
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                window.close();

            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f m(e.mouseButton.x, e.mouseButton.y);
                if (button.getGlobalBounds().contains(m))
                    showPath = !showPath;
            }

            if (gameOver && e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::R)
            {
                map.resetFromFile("maps/map1.txt");
                ROWS = map.getRows();
                COLS = map.getCols();

                grid = Grid(ROWS, COLS, CELL);

                std::tie(pr, pc) = map.getStart();
                playerSpr.setPosition(pc * CELL, pr * CELL);

                battery = MAX_BATTERY;
                turnCount = 0;
                itemsCollected = 0;
                path = findCollectAllPath(map, {pr, pc}, map.getGoal());

                hud.setPosition(5, ROWS * CELL + 5);

                btnPos = sf::Vector2f(COLS * CELL - 110, ROWS * CELL + 5);
                button.setPosition(btnPos);
                btnText.setPosition(btnPos + sf::Vector2f(10, 5));

                gameOver = false;
                showPath = false;

                window.clear();
                map.draw(window);
                window.display();
            }

            if (!gameOver && e.type == sf::Event::KeyPressed)
            {
                if (battery <= 0)
                    continue;
                int nr = pr, nc = pc;
                if (e.key.code == sf::Keyboard::Left)
                    nc--;
                if (e.key.code == sf::Keyboard::Right)
                    nc++;
                if (e.key.code == sf::Keyboard::Up)
                    nr--;
                if (e.key.code == sf::Keyboard::Down)
                    nr++;

                bool pass = !map.isWall(nr, nc) || map.hasActiveBridge(pr, pc, nr, nc);
                if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && pass)
                {
                    pr = nr;
                    pc = nc;
                    playerSpr.setPosition(pc * CELL, pr * CELL);
                    battery--;
                    turnCount++;
                    if (turnCount % EVENT_PERIOD == 0)
                        map.updateWind();

                    if (map.isItem(pr, pc))
                    {
                        map.removeItem(pr, pc);
                        itemsCollected++;
                    }

                    if (showPath)
                        path = findPathBFS(map, {pr, pc}, map.getGoal());

                    if (std::make_pair(pr, pc) == map.getGoal())
                    {
                        gameOver = true;
                        gameMessage = "¡GANASTE!";
                    }
                }

                if (battery <= 0 && !gameOver)
                {
                    gameOver = true;
                    gameMessage = "¡SIN BATERÍA!";
                }
            }
        }

        hud.setString(
            "Items: " + std::to_string(itemsCollected) +
            "   Batería: " + std::to_string(battery));

        window.clear(sf::Color::Black);
        grid.draw(window);
        map.draw(window);
        if (showPath)
        {
            for (auto [r, c] : path)
            {
                stepShape.setPosition(c * CELL + 1, r * CELL + 1);
                window.draw(stepShape);
            }
        }
        window.draw(playerSpr);
        window.draw(hud);
        window.draw(button);
        window.draw(btnText);

        if (gameOver)
        {
            sf::Text endText(gameMessage + " - Presiona R", font, 30);
            endText.setFillColor(sf::Color::Red);
            endText.setPosition(COLS * CELL / 2.f - 150, ROWS * CELL / 2.f - 20);
            window.draw(endText);
        }

        window.display();
    }

    return 0;
}
