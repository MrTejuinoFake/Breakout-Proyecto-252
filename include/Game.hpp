#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>  // Necesario para listas dinámicas
#include "Brick.hpp"
#include "Ball.hpp"
#include "Paddle.hpp"
#include "PowerUp.hpp"
#include <array>

enum class GameState {
    Menu,
    Controls,
    Playing,
    GameOver
};

// Partícula para efectos de destrucción
struct Particle {
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;
};

// Estela de la pelota (efecto Tron)
struct BallTrail {
    sf::CircleShape shape;
    float lifetime;
    sf::Vector2f position;
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
    void renderControls();
    void renderGameOver();
    void resetGame();
    void loseLife();
    void initMatrixEffect();    // Inicializar efecto de cascada
    void updateMatrixEffect();  // Actualizar cascada
    void renderMatrixEffect();  // Dibujar cascada
    void processTerminalInput(char c);     // Procesar entrada del menú terminal
    void processControlsInput(char c);     // Procesar entrada del menú de controles
    void processGameOverInput(char c);     // Procesar entrada del menú de game over
    void updateTerminalDisplay();          // Actualizar display del terminal

    // Nueva función para crear el nivel
    void initLevel();
    
    // Efectos visuales
    void createBrickParticles(sf::Vector2f position, sf::Color color);
    void updateParticles(float dt); 

    sf::RenderWindow window;
    GameState state;
    
    // Sistema de vidas y puntuación
    int lives;
    int score;
    
    // Lista donde guardaremos todos los ladrillos creados
    std::vector<Brick> bricks; 
    sf::Texture brickTextures[5];  // Array para múltiples texturas
    sf::Texture blockTexture;      // Textura principal
    sf::Texture yellowBlockTexture; // Textura específica para bloques amarillos (velocidad)
    
    // PowerUps
    std::vector<PowerUp> powerUps;
    sf::Texture powerUpSpeedTexture;
    sf::Texture powerUpExpandTexture;
    sf::Texture powerUpMultiBallTexture;
    sf::Texture powerUpExtraLifeTexture;
    
    // Paddle (base) y pelota
    Paddle paddle;
    Ball ball;
    std::vector<Ball> extraBalls;  // Pelotas adicionales para MultiBall
    float paddleRotation;          // Rotación suave del paddle
    float targetPaddleRotation;    // Rotación objetivo del paddle
    float originalPaddleWidth;     // Para restaurar tamaño del paddle
    float originalPaddleSpeed;     // Para restaurar velocidad del paddle
    int maxLives;                  // Límite máximo de vidas
    
    // Sistema de velocidad del paddle con PowerUp
    bool hasPaddleSpeedPowerUp;    // Si tiene el PowerUp de velocidad
    bool isPaddleSpeedActive;      // Si la velocidad está activada (tecla S)
    
    // Sistema de destello de pelota por velocidad
    sf::CircleShape ballGlow;      // Destello visual de la pelota
    float ballGlowIntensity;       // Intensidad del destello (0-1)
    
    // Reloj para delta time
    sf::Clock clock;
    
    // Sistema de fondos estáticos
    sf::Texture menuBackgroundTexture;     // Fondo del menú
    sf::Texture gameBackgroundTexture;     // Fondo base del juego (azul)
    sf::Texture gameOverBackgroundTexture; // Fondo de game over
    sf::Image windowIcon;                  // Icono de la ventana
    sf::Sprite backgroundSprite;
    int currentLevel;                      // Nivel actual para progresión de color
    
    // Sistema de cascada de letras (efecto Matrix)
    struct MatrixColumn {
        float x;
        float y;
        float speed;
        std::string letters;
        sf::Color color;
    };
    std::vector<MatrixColumn> matrixColumns;
    bool showMatrixEffect;
    sf::Clock matrixClock;
    
    // Sistema de menú interactivo tipo terminal
    std::string currentInput;          // Lo que ha escrito el usuario
    std::string targetPlay;            // "play"
    std::string targetExit;            // "exit"
    std::string targetControls;        // "controls"
    std::string targetInferno;         // "inferno"
    std::vector<bool> playProgress;    // Progreso de "play" (p, l, a, y)
    std::vector<bool> exitProgress;    // Progreso de "exit" (e, x, i, t)
    std::vector<bool> controlsProgress;// Progreso de "controls" (c, o, n, t, r, o, l, s)
    std::vector<bool> infernoProgress; // Progreso de "inferno" (i, n, f, e, r, n, o)
    sf::Text terminalPrompt;           // "C:\\ARKANOID> "
    sf::Text playCommand;
    sf::Text exitCommand;
    sf::Text controlsCommand;
    sf::Text infernoCommand;
    sf::Text titleCommand;
    
    // Modo de juego
    bool isInfernoMode;                // true = modo Inferno, false = modo Normal
    
    // Menú de controles
    std::string currentControlsInput;  // Lo que ha escrito el usuario en controles
    std::vector<bool> returnProgress;  // Progreso de "return" (r, e, t, u, r, n)
    
    // Menú terminal de Game Over
    std::string currentGameOverInput;
    std::string targetReboot;          // "reboot"
    std::vector<bool> rebootProgress;  // Progreso de "reboot" (r, e, b, o, o, t)
    std::vector<bool> gameOverExitProgress; // Progreso de "exit" para game over
    sf::Text gameOverPrompt;
    sf::Text rebootCommand;
    sf::Text gameOverExitCommand;
    
    // Fuentes y textos
    sf::Font font;
    sf::Text menuText;
    sf::Text instructionText;
    sf::Text gameOverText;
    sf::Text scoreText;
    sf::Text livesText;
    sf::Text finalScoreText;
    
    // Sistema de audio
    std::vector<sf::Music*> backgroundMusicTracks;  // 7 músicas de fondo aleatorias
    sf::Music* currentBackgroundMusic;              // Música actual
    int currentTrackIndex;                          // Índice de la música actual
    
    sf::Music lowLifeMusic1;       // Música cuando queda 1 vida (opción 1)
    sf::Music lowLifeMusic2;       // Música cuando queda 1 vida (opción 2)
    sf::Music* currentLowLifeMusic;// Música de vida baja actual
    bool isPlayingLowLifeMusic;    // Si está sonando música de vida baja
    
    sf::Music menuMusic;          // Música del menú (loop)
    sf::SoundBuffer bounceBuffer;
    sf::Sound bounceSound;
    sf::SoundBuffer gameOverBuffer; // Sonido de game over
    sf::Sound gameOverSound;
    sf::SoundBuffer specialBuffer;  // Sonido especial al presionar M
    sf::Sound specialSound;

    // Sistema de control de volumen
    float masterVolume;           // Volumen maestro (0.0f - 1.0f)
    std::string volumeInput;      // Input del usuario para volumen (00-99)
    bool volumeInputMode;         // Si está en modo de cambio de volumen
    float baseVolumeMusic;        // Volúmenes base individuales
    float baseVolumeEffects;
    float baseVolumeMenu;
    float baseVolumeGameOver;

    // Funciones de volumen
    void updateMasterVolume();           // Actualizar todos los volúmenes con el maestro
    void processVolumeInput(char c);     // Procesar input de volumen
    
    // Sistema de efectos visuales
    std::vector<Particle> particles;
    std::vector<BallTrail> ballTrails;  // Estelas de la pelota
    float trailSpawnTimer;              // Timer para crear estelas
    sf::Vector2f screenShakeOffset;
    float screenShakeDuration;
    float screenShakeIntensity;
    
    // Función para actualizar destello de pelota
    void updateBallGlow();
    void updateBallTrails(float dt);
};