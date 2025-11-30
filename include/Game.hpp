#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
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
    void renderGameOver();
    void resetGame();
    void loseLife();

    // Nueva función para crear el nivel
    void initLevel(); 

    sf::RenderWindow window;
    GameState state;
    
    // Sistema de vidas y puntuación
    int lives;
    int score;
    
    // Lista donde guardaremos todos los ladrillos creados
    std::vector<Brick> bricks; 
    sf::Texture brickTextures[5];  // Array para múltiples texturas
    sf::Texture blockTexture;      // Textura principal
    
    // Paddle (base) y pelota
    Paddle paddle;
    Ball ball;
    
    // Reloj para delta time
    sf::Clock clock;
    
    // Fuentes y textos
    sf::Font font;
    sf::Text menuText;
    sf::Text instructionText;
    sf::Text gameOverText;
    sf::Text scoreText;
    sf::Text livesText;
    sf::Text finalScoreText;
    
    // Sistema de audio
    sf::Music backgroundMusic;     // Música durante el juego
    sf::Music menuMusic;          // Música del menú (loop)
    sf::SoundBuffer bounceBuffer;
    sf::Sound bounceSound;
    sf::SoundBuffer gameOverBuffer; // Sonido de game over
    sf::Sound gameOverSound;
};