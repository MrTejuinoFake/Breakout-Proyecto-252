#pragma once
#include <SFML/Graphics.hpp>
#include <vector>  // Necesario para listas dinámicas
#include "Brick.hpp"
#include "Ball.hpp"
#include "Paddle.hpp"
#include <array>

enum class GameState {
    Menu,
    Playing,
    GameOver
};

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    void processEvents();
    void update();
    void render();
    void renderMenu();
    void resetGame();

    // Nueva función para crear el nivel
    void initLevel(); 

    sf::RenderWindow window;
    GameState state;
    
    // Lista donde guardaremos todos los ladrillos creados
    std::vector<Brick> bricks; 
    sf::Texture brickTextures[5];  // Array para múltiples texturas
    sf::Texture blockTexture;      // Textura principal
    
    // Paddle (base) y pelota
    Paddle paddle;
    Ball ball;
    
    // Reloj para delta time
    sf::Clock clock;
    
    // Fuente para el menú
    sf::Font font;
    sf::Text menuText;
    sf::Text instructionText;
};