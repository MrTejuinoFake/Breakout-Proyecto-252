#pragma once
#include <SFML/Graphics.hpp>
#include <vector>  // Necesario para listas dinámicas
#include "Brick.hpp" 
#include <array>

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    void processEvents();
    void update();
    void render();

    // Nueva función para crear el nivel
    void initLevel(); 

    sf::RenderWindow window;
    
    // Lista donde guardaremos todos los ladrillos creados
    std::vector<Brick> bricks; 
    sf::Texture brickTextures[3];  // Array para múltiples texturas
    sf::Texture blockTexture;      // Textura principal
};