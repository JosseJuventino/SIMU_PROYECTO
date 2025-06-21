#include <SFML/Graphics.hpp>
#include <iostream>
#include "Grid.h"
#include "Map.h"
#include "PathFinder.h"

int main() {
    const float CELL = 50.f;
    // 1) Carga mapa
    Map map("maps/map1.txt", CELL);
    int ROWS = map.getRows(), COLS = map.getCols();

    // 2) Ventana (+40 px HUD)
    sf::RenderWindow window(
        sf::VideoMode(COLS*CELL, ROWS*CELL + 40),
        "Escape the Grid"
    );
    window.setFramerateLimit(60);

    // 3) Grid
    Grid grid(ROWS, COLS, CELL);

    // 4) Sprite jugador
    sf::Texture playerTex;
    if (!playerTex.loadFromFile("assets/sprites/character.png")) {
        std::cerr<<"ERROR: no pude cargar character.png\n";
        return 1;
    }
    sf::Sprite playerSpr(playerTex);
    float pscale = CELL / float(playerTex.getSize().x);
    playerSpr.setScale(pscale, pscale);

    // 5) Inicio
    auto [sr, sc] = map.getStart();
    int pr=sr, pc=sc;
    playerSpr.setPosition(pc*CELL, pr*CELL);

    // 6) HUD
    int itemsCollected=0;
    sf::Font font;
    if(!font.loadFromFile("assets/fonts/LiberationSans-Regular.ttf")){
        std::cerr<<"ERROR: no pude cargar la fuente\n";
        return 1;
    }
    sf::Text hud("", font, 20);
    hud.setFillColor(sf::Color::White);
    hud.setPosition(5, ROWS*CELL+5);

    // 7) Solver TSP
    auto path = findCollectAllPath(map, {sr,sc}, map.getGoal());
    bool showPath=false;

    // 8) Botón
    sf::RectangleShape button({100,30});
    button.setFillColor({70,70,70});
    button.setOutlineColor(sf::Color::White);
    button.setOutlineThickness(1);
    sf::Vector2f btnPos(COLS*CELL-110, ROWS*CELL+5);
    button.setPosition(btnPos);
    sf::Text btnText("Mostrar ruta", font, 16);
    btnText.setFillColor(sf::Color::White);
    btnText.setPosition(btnPos+sf::Vector2f(10,5));

    // 9) Ruta shape
    sf::RectangleShape stepShape({CELL-2, CELL-2});
    stepShape.setFillColor({255,255,0,150});

    // 10) Mecánicas adicionales
    int turnCount=0;
    constexpr int EVENT_PERIOD=15;
    constexpr int MAX_BATTERY=100;
    int battery=MAX_BATTERY;

    // Bucle principal
    while(window.isOpen()){
        sf::Event e;
        while(window.pollEvent(e)){
            if(e.type==sf::Event::Closed)
                window.close();

            // click botón
            if(e.type==sf::Event::MouseButtonPressed
             && e.mouseButton.button==sf::Mouse::Left)
            {
                sf::Vector2f m(e.mouseButton.x,e.mouseButton.y);
                if(button.getGlobalBounds().contains(m))
                    showPath = !showPath;
            }

            // movimiento
            if(e.type==sf::Event::KeyPressed){
                if(battery<=0) continue;
                int nr=pr, nc=pc;
                if(e.key.code==sf::Keyboard::Left)  nc--;
                if(e.key.code==sf::Keyboard::Right) nc++;
                if(e.key.code==sf::Keyboard::Up)    nr--;
                if(e.key.code==sf::Keyboard::Down)  nr++;

                bool pass = !map.isWall(nr,nc) || map.hasActiveBridge(pr,pc,nr,nc);
                if(nr>=0&&nr<ROWS&&nc>=0&&nc<COLS && pass){
                    pr=nr; pc=nc;
                    playerSpr.setPosition(pc*CELL, pr*CELL);
                    battery--;
                    turnCount++;
                    if(turnCount%EVENT_PERIOD==0)
                        map.updateWind();

                    if(map.isItem(pr,pc)){
                        map.removeItem(pr,pc);
                        itemsCollected++;
                    }
                }
            }
        }

        // actualiza HUD
        hud.setString(
            "Items: "+std::to_string(itemsCollected)+
            "   Batería: "+std::to_string(battery)
        );

        // dibujado
        window.clear(sf::Color::Black);
        grid.draw(window);
        map.draw(window);
        if(showPath){
            for(auto [r,c]: path){
                stepShape.setPosition(c*CELL+1, r*CELL+1);
                window.draw(stepShape);
            }
        }
        window.draw(playerSpr);
        window.draw(hud);
        window.draw(button);
        window.draw(btnText);
        window.display();
    }

    return 0;
}
