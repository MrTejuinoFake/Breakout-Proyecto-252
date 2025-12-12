#include "Game.hpp"
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <cctype>


void Game::initLevel() {
    // ===============================================
    // CONSTANTES DE CONFIGURACIÓN 
    // ===============================================
    
    // Dimensiones de la cuadrícula de bloques
    const int columns = 16;                  // Número de columnas de bloques
    const int rows = 8;                      // Número de filas de bloques
    
    // Tamaño de cada celda
    const float cellWidth = 60.f;           // Ancho de cada celda en píxeles
    const float cellHeight = 30.f;          // Alto de cada celda en píxeles
    
    // Posición inicial para centrar la cuadrícula
    const float startX = 8.0f;              // Distancia desde el borde izquierdo
    const float startY = 200.f;             // Distancia desde la parte superior
    
    // Probabilidades de tipos de bloques (según nivel)
    int smallBlockChance = 15 + (currentLevel - 1) * 3;     // Más bloques pequeños cada nivel
    int purpleBlockChance = 10 + (currentLevel - 1) * 5;    // Más bloques de 3 golpes
    int redBlockChance = 25 + (currentLevel - 1) * 2;       // Más bloques de 2 golpes
    
    // MODO INFERNO: Aumenta drásticamente ciertos bloques
    if (isInfernoMode) {
        smallBlockChance = 40 + (currentLevel - 1) * 5;     // Muchos más bloques amarillos
        if (smallBlockChance > 70) smallBlockChance = 70;   // Máximo 70%
        
        purpleBlockChance = 35 + (currentLevel - 1) * 8;    // Muchos más bloques morados
        if (purpleBlockChance > 60) purpleBlockChance = 60; // Máximo 60%
        
        redBlockChance = 50 + (currentLevel - 1) * 5;       // Muchos más bloques rojos
        if (redBlockChance > 75) redBlockChance = 75;       // Máximo 75%
    } else {
        // Modo normal
        if (smallBlockChance > 40) smallBlockChance = 40;   // Máximo 40%
        if (purpleBlockChance > 35) purpleBlockChance = 35; // Máximo 35%
        if (redBlockChance > 45) redBlockChance = 45;       // Máximo 45%
    }
    
    // Valores de puntuación
    const int normalBlockPoints = 10;       // Puntos por bloque normal (1 golpe)
    const int redBlockPoints = 20;          // Puntos por bloque rojo (2 golpes)
    const int purpleBlockPoints = 100;      // Puntos por bloque morado (3 golpes)
    const int smallBlockPoints = 50;        // Puntos por bloque pequeño (amarillo)
    
    // Tamaño de bloques pequeños (factor de reducción)
    const float smallBlockSizeFactor = 0.9f; // 90% del tamaño normal
    
    // ===============================================
    // LÓGICA DEL JUEGO 
    // ===============================================

    // Matriz de ocupación
    bool grid[rows][columns] = {false};

    bricks.clear(); 

    // Crear lista de colores disponibles
    std::vector<sf::Color> availableColors = {
        sf::Color(255, 140, 0),   // Naranja
        sf::Color(255, 215, 0),   // Dorado
        sf::Color(50, 205, 50),   // Verde lima
        sf::Color(0, 128, 128),   // Teal
        sf::Color(0, 255, 255),   // Cian
        sf::Color(30, 144, 255),  // Azul dodger
        sf::Color(255, 20, 147),  // Rosa intenso
        sf::Color(224, 224, 224)  // Gris claro
    };

    // BUCLE DE FILAS (Vertical)
    for (int y = 0; y < rows; ++y) {

        
        // Seleccionar color aleatorio de los disponibles
        sf::Color rowColor;
        if (!availableColors.empty()) {
            int randomIndex = std::rand() % availableColors.size();
            rowColor = availableColors[randomIndex];
            
            // Remover el color 
            availableColors.erase(availableColors.begin() + randomIndex);
        } else {
            // Si ya usamos todos los colores, usar uno aleatorio cualquiera
            rowColor = sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256);
        }

        // BUCLE DE COLUMNAS (Horizontal)
        for (int x = 0; x < columns; ++x) {
            
            if (grid[y][x]) continue;

            // Decidir tamaño aleatorio (1, 2, 3 celdas o bloque pequeño)
            int widthInCells;
            bool isSmallBlock = false;
            int blockTypeRandom = std::rand() % 100;
            
            if (blockTypeRandom < smallBlockChance) {  // % bloques pequeños
                widthInCells = 1;
                isSmallBlock = true;
            } else {
                widthInCells = (std::rand() % 2) + 1;  // 1 o 2 celdas normales
            }

            if (x + widthInCells > columns) {
                widthInCells = columns - x; 
            }

            bool fits = true;
            for (int k = 0; k < widthInCells; ++k) {
                if (grid[y][x + k]) {
                    fits = false;
                    break;
                }
            }
            if (!fits) widthInCells = 1;



            // Seleccionar textura según el tipo de bloque
            const sf::Texture* selectedTexture;
            if (isSmallBlock) {
                selectedTexture = &yellowBlockTexture; // Textura específica para bloques amarillos
            } else {
                int textureIndex = std::rand() % 5;
                selectedTexture = &brickTextures[textureIndex]; // Textura aleatoria para otros bloques
            }
            
            // --- CREAR EL LADRILLO ---
            float posX = startX + (x * cellWidth); // Usar startX para centrar
            float posY = startY + y * cellHeight;
            float realWidth = isSmallBlock ? (cellWidth * smallBlockSizeFactor) : (widthInCells * cellWidth);
            float realHeight = isSmallBlock ? (cellHeight * smallBlockSizeFactor) : cellHeight;
            
            // Centrar bloques pequeños
            if (isSmallBlock) {
                posX += (cellWidth - realWidth) / 2.0f;
                posY += (cellHeight - realHeight) / 2.0f;
            } 

            // Decidir aleatoriamente el tipo de bloque
            int hits = 1;
            sf::Color brickColor = rowColor;
            int pointValue = normalBlockPoints;  // Valor por defecto
            bool isIndestructible = false;       // Por defecto no es indestructible
            
            // Calcular probabilidad de bloques indestructibles (a partir del nivel 2)
            int indestructibleChance = 0;
            if (currentLevel >= 2) {
                indestructibleChance = 5 + (currentLevel - 2) * 3;  // 5% en nivel 2, 8% en nivel 3, etc.
                if (indestructibleChance > 25) indestructibleChance = 25;  // Máximo 25%
            }
            
            if (isSmallBlock) {
                brickColor = sf::Color(255, 255, 0);  // Amarillo para bloques pequeños
                pointValue = smallBlockPoints;  // Puntos de bloques pequeños
            } else {
                int random = std::rand() % 100;
                
                // Verificar si es un bloque indestructible
                if (random < indestructibleChance) {
                    isIndestructible = true;
                    hits = 999;  // Valor alto para que no se pueda destruir normalmente
                    brickColor = sf::Color(128, 128, 128);  // Gris para bloques indestructibles
                    pointValue = 0;  // No dan puntos
                }
                else if (random < purpleBlockChance + indestructibleChance) {  // % de probabilidad - BLOQUES MORADOS (3 golpes)
                    hits = 3;
                    brickColor = sf::Color(150, 0, 200);  // Morado intenso
                    pointValue = purpleBlockPoints;  // Puntos por bloque morado
                }
                else if (random < redBlockChance + indestructibleChance) {  // % de probabilidad - BLOQUES ROJOS (2 golpes)
                    hits = 2;
                    brickColor = sf::Color(200, 0, 0);  // Rojo intenso
                    pointValue = redBlockPoints;  // Puntos por bloque rojo
                }
                // El resto son bloques normales de 1 golpe
            }

            bricks.emplace_back(posX, posY, realWidth, realHeight, brickColor, *selectedTexture, hits);
            // Guardar información adicional del bloque
            bricks.back().pointValue = pointValue;
            bricks.back().isSmallBlock = isSmallBlock;
            bricks.back().isIndestructible = isIndestructible;

            // Marcar celdas
            for (int k = 0; k < widthInCells; ++k) {
                grid[y][x + k] = true;
            }
        }
    }
}


// Constructor: Inicializa la ventana
Game::Game() : state(GameState::Menu), lives(3), score(0), masterVolume(0.5f), volumeInputMode(false) {
    // ===============================================
    // CONSTANTES DE TEXTO Y POSICIONES 
    // ===============================================
    
    // Configuración de ventana
    const int windowWidth = 1000;
    const int windowHeight = 800;
    const std::string windowTitle = "COCONOID";
    
    // Tamaños de fuente
    const int titleFontSize = 18;
    const int instructionFontSize = 18;
    const int hudFontSize = 14;
    const int gameOverFontSize = 72;
    const int finalScoreFontSize = 32;
    const int terminalFontSize = 16;
    const int commandFontSize = 14;
    const int progressFontSize = 14;
    
    // Posiciones del menú terminal
    const float terminalTitleY = 120.f;
    const float terminalPromptX = 30.f;
    const float terminalPromptY = 150.f;
    const float commandListY = 250.f;
    const float progressStartY = 320.f;
    
    // Posiciones del HUD en juego
    const float hudY = 65.f;
    const float hudSeparation = 40.f;
    
    // Posiciones de Game Over
    const float gameOverTitleY = 135.f;
    const float gameOverScoreY = 160.f;
    const float gameOverPromptY = 220.f;
    const float gameOverCommandsY = 310.f;
    const float gameOverProgressY = 380.f;
    
    // Inicializar efectos visuales
    screenShakeOffset = sf::Vector2f(0.f, 0.f);
    screenShakeDuration = 0.f;
    screenShakeIntensity = 0.f;
    paddleRotation = 0.f;
    targetPaddleRotation = 0.f;
    
    // Inicializar sistema de power-ups activos
    speedPaddleDuration = 8.f;      // 8 segundos de duración
    expandPaddleDuration = 8.f;     // 8 segundos de duración
    shrinkPaddleDuration = 8.f;     // 8 segundos de duración (desventaja)
    slowPaddleDuration = 8.f;       // 8 segundos de duración (desventaja)
    slowMotionDuration = 6.f;       // 6 segundos de duración (desventaja)
    slowMotionMultiplier = 0.5f;    // 50% de velocidad normal
    
    // Inicializar destello de pelota
    ballGlow.setRadius(ball.getRadius() + 4.f);
    ballGlow.setOrigin(ballGlow.getRadius(), ballGlow.getRadius());
    ballGlow.setFillColor(sf::Color::Transparent);
    ballGlowIntensity = 0.f;
    
    // Inicializar sistema de estelas
    trailSpawnTimer = 0.f;
    
    // Inicializar sistema de música
    currentBackgroundMusic = nullptr;
    currentLowLifeMusic = nullptr;
    isPlayingLowLifeMusic = false;
    
    std::cout << "--- INICIANDO JUEGO ---" << std::endl;
    window.create(sf::VideoMode(windowWidth, windowHeight), windowTitle, sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(120);
    
    // Cargar y establecer icono de la ventana
    if (windowIcon.loadFromFile("assets/images/logo.png")) {
        window.setIcon(windowIcon.getSize().x, windowIcon.getSize().y, windowIcon.getPixelsPtr());
        std::cout << "Icono cargado exitosamente" << std::endl;
    } else {
        std::cout << "No se pudo cargar el icono logo.png" << std::endl;
    }
    
    // Semilla para números aleatorios (para que cambie cada vez que abres el juego)
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    // Cargar fuente para el menú
    if (!font.loadFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
        // Si no encuentra la fuente, usar la fuente por defecto del sistema
        std::cout << "No se pudo cargar la mendiga letra.ttf, usando fuente por defecto" << std::endl;
        exit(1);
    }
    
    // Configurar texto del menú
    menuText.setFont(font);
    menuText.setString("ARKANOID");
    menuText.setCharacterSize(titleFontSize);
    menuText.setFillColor(sf::Color::White);

    sf::FloatRect titleRect = menuText.getLocalBounds();
    menuText.setOrigin(titleRect.left + titleRect.width/2.0f, titleRect.top + titleRect.height/2.0f);
    menuText.setPosition(window.getSize().x / 2.0f, 200);
    
    instructionText.setFont(font);
    instructionText.setString("Presiona ESPACIO para jugar\nPresiona ESC para salir");
    instructionText.setCharacterSize(instructionFontSize);
    instructionText.setFillColor(sf::Color::White);
    instructionText.setPosition(320, 400);
    
    // Configurar textos del juego (HUD)
    livesText.setFont(font);
    livesText.setCharacterSize(hudFontSize);
    livesText.setFillColor(sf::Color::Green);
    
    scoreText.setFont(font);
    scoreText.setCharacterSize(hudFontSize);
    scoreText.setFillColor(sf::Color::Green);
    
    // Textos de Game Over
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(gameOverFontSize);
    gameOverText.setFillColor(sf::Color::Red);
    
    finalScoreText.setFont(font);
    finalScoreText.setCharacterSize(finalScoreFontSize);
    finalScoreText.setFillColor(sf::Color::Yellow);
    
    // Centrar textos de Game Over
    sf::FloatRect gameOverRect = gameOverText.getLocalBounds();
    gameOverText.setOrigin(gameOverRect.left + gameOverRect.width/2.0f, gameOverRect.top + gameOverRect.height/2.0f);
    gameOverText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f - 50);
    
    sf::FloatRect scoreRect = finalScoreText.getLocalBounds();
    finalScoreText.setOrigin(scoreRect.left + scoreRect.width/2.0f, scoreRect.top + scoreRect.height/2.0f);
    finalScoreText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f + 50);

    sf::FloatRect instRect = instructionText.getLocalBounds();
    instructionText.setOrigin(instRect.left + instRect.width/2.0f, instRect.top + instRect.height/2.0f);
    instructionText.setPosition(window.getSize().x / 2.0f, 400);
    
    // Cargar archivos de audio
    if (!bounceBuffer.loadFromFile("assets/music/bounce.mp3")) {
        std::cout << "No se pudo cargar bounce.mp3" << std::endl;
    } else {
        bounceSound.setBuffer(bounceBuffer);
        baseVolumeEffects = 70.0f;
        bounceSound.setVolume(baseVolumeEffects * masterVolume);  
    }
    
    // Cargar 7 músicas de fondo normales
    baseVolumeMusic = 30.0f;
    for (int i = 1; i <= 7; i++) {
        sf::Music* track = new sf::Music();
        std::string filename = "assets/music/background" + std::to_string(i) + ".mp3";
        if (track->openFromFile(filename)) {
            track->setLoop(true);
            track->setVolume(baseVolumeMusic * masterVolume);
            backgroundMusicTracks.push_back(track);
            std::cout << "Música " << i << " cargada correctamente" << std::endl;
        } else {
            std::cout << "No se pudo cargar " << filename << std::endl;
            delete track;
        }
    }
    
    // Cargar 6 músicas del modo Inferno
    for (int i = 1; i <= 6; i++) {
        sf::Music* track = new sf::Music();
        std::string filename = "assets/music/inferno" + std::to_string(i) + ".mp3";
        if (track->openFromFile(filename)) {
            track->setLoop(true);
            track->setVolume(baseVolumeMusic * masterVolume);
            infernoMusicTracks.push_back(track);
            std::cout << "Música Inferno " << i << " cargada correctamente" << std::endl;
        } else {
            std::cout << "No se pudo cargar " << filename << std::endl;
            delete track;
        }
    }
    
    // Seleccionar música aleatoria al inicio
    if (!backgroundMusicTracks.empty()) {
        currentTrackIndex = std::rand() % backgroundMusicTracks.size();
        currentBackgroundMusic = backgroundMusicTracks[currentTrackIndex];
    } else {
        currentBackgroundMusic = nullptr;
    }
    
    // Cargar músicas de vida baja (1 vida restante)
    if (!lowLifeMusic1.openFromFile("assets/music/lowlife1.mp3")) {
        std::cout << "No se pudo cargar lowlife1.mp3" << std::endl;
    } else {
        lowLifeMusic1.setLoop(true);
        lowLifeMusic1.setVolume(baseVolumeMusic * masterVolume);
    }
    
    if (!lowLifeMusic2.openFromFile("assets/music/lowlife2.mp3")) {
        std::cout << "No se pudo cargar lowlife2.mp3" << std::endl;
    } else {
        lowLifeMusic2.setLoop(true);
        lowLifeMusic2.setVolume(baseVolumeMusic * masterVolume);
    }
    
    isPlayingLowLifeMusic = false;
    
    // Cargar música del menú
    if (!menuMusic.openFromFile("assets/music/menu.mp3")) {
       // std::cout << "No se pudo cargar menu.mp3" << std::endl;
    } else {
        menuMusic.setLoop(true);
        baseVolumeMenu = 40.0f;
        menuMusic.setVolume(baseVolumeMenu * masterVolume);  
    }
    
    // Cargar sonido de game over
    if (!gameOverBuffer.loadFromFile("assets/music/gameover.mp3")) {
    } else {
        gameOverSound.setBuffer(gameOverBuffer);
        baseVolumeGameOver = 50.0f;
        gameOverSound.setVolume(baseVolumeGameOver * masterVolume);  
    }
    
    // Cargar sonido especial para tecla M
    if (!specialBuffer.loadFromFile("assets/music/meow.mp3")) {
        std::cout << "No se pudo cargar sonido especial para M" << std::endl;
    } else {
        specialSound.setBuffer(specialBuffer);
        specialSound.setVolume(80.0f * masterVolume);  
    }


    blockTexture.setRepeated(true);


    std::string files[] = {"brick1.jpg", "brick2.jpg", "brick3.jpg", "brick4.jpg", "brick5.jpg"};
    for (int i = 0; i < 5; ++i) {
        if (!brickTextures[i].loadFromFile("assets/images/" + files[i])) {
            // Manejo de error si una imagen no se encuentra
            std::cerr << "Error cargando textura: " << files[i] << std::endl;
            // Crear una textura simple de color sólido en lugar de terminar el programa y se cierre
            sf::Image defaultImage;
            defaultImage.create(32, 32, sf::Color::White);
            brickTextures[i].loadFromImage(defaultImage);
        }
        brickTextures[i].setRepeated(true);
    }
    
    // Cargar textura específica para bloques amarillos (velocidad)
    if (!yellowBlockTexture.loadFromFile("assets/images/error.jpg")) {
        std::cerr << "Error cargando textura amarilla: error.jpg" << std::endl;
        // Crear textura por defecto si no se encuentra
        sf::Image defaultYellow;
        defaultYellow.create(32, 32, sf::Color::Yellow);
        yellowBlockTexture.loadFromImage(defaultYellow);
    }
    yellowBlockTexture.setRepeated(true);
    yellowBlockTexture.setSmooth(true);
    
    // Cargar texturas de PowerUps con tamaño fijo
    if (!powerUpSpeedTexture.loadFromFile("assets/images/powerup_speed.png")) {
        sf::Image defaultImg;
        defaultImg.create(40, 40, sf::Color::Green);
        powerUpSpeedTexture.loadFromImage(defaultImg);
    }
    powerUpSpeedTexture.setSmooth(true);
    
    if (!powerUpExpandTexture.loadFromFile("assets/images/powerup_expand.png")) {
        sf::Image defaultImg;
        defaultImg.create(40, 40, sf::Color::Blue);
        powerUpExpandTexture.loadFromImage(defaultImg);
    }
    powerUpExpandTexture.setSmooth(true);
    
    if (!powerUpMultiBallTexture.loadFromFile("assets/images/powerup_multiball.png")) {
        sf::Image defaultImg;
        defaultImg.create(40, 40, sf::Color::Cyan);
        powerUpMultiBallTexture.loadFromImage(defaultImg);
    }
    powerUpMultiBallTexture.setSmooth(true);
    
    if (!powerUpExtraLifeTexture.loadFromFile("assets/images/powerup_life.png")) {
        sf::Image defaultImg;
        defaultImg.create(40, 40, sf::Color::Red);
        powerUpExtraLifeTexture.loadFromImage(defaultImg);
    }
    powerUpExtraLifeTexture.setSmooth(true);
    
    // Cargar texturas de PowerUps desventajosos
    if (!powerUpShrinkTexture.loadFromFile("assets/images/powerup_shrink.png")) {
        sf::Image defaultImg;
        defaultImg.create(40, 40, sf::Color::Magenta);
        powerUpShrinkTexture.loadFromImage(defaultImg);
    }
    powerUpShrinkTexture.setSmooth(true);
    
    if (!powerUpSlowTexture.loadFromFile("assets/images/powerup_slow.png")) {
        sf::Image defaultImg;
        defaultImg.create(40, 40, sf::Color::Yellow);
        powerUpSlowTexture.loadFromImage(defaultImg);
    }
    powerUpSlowTexture.setSmooth(true);
    
    if (!powerUpSlowMotionTexture.loadFromFile("assets/images/powerup_slowmotion.png")) {
        sf::Image defaultImg;
        defaultImg.create(40, 40, sf::Color(128, 128, 255)); // Azul pastel
        powerUpSlowMotionTexture.loadFromImage(defaultImg);
    }
    powerUpSlowMotionTexture.setSmooth(true);
    
    // Para que no se vea borroso si es pixel art
    blockTexture.setSmooth(true);
    
    // Inicializar volumen maestro
    volumeInput = "50"; // Volumen inicial al 50%
    updateMasterVolume();
    initLevel(); 

    // Inicializar paddle en la parte inferior
    float paddleY = static_cast<float>(window.getSize().y) - 40.f;
    paddle.setPosition(static_cast<float>(window.getSize().x) / 2.f, paddleY);
    originalPaddleWidth = paddle.getSize().x;  // Guardar tamaño original
    originalPaddleSpeed = paddle.speed;        // Guardar velocidad original
    maxLives = 3;                              // Límite máximo de vidas

    // Inicializar bola pegada al paddle
    ball.setPosition(paddle.getPosition().x, paddle.getPosition().y - paddle.getSize().y/2.f - ball.getRadius() - 2.f);
    ball.isStuck = true; // Comienza pegada
    ball.velocity = sf::Vector2f(0.f, 0.f); // Sin velocidad inicial
    
    // Cargar fondos estáticos
    if (!menuBackgroundTexture.loadFromFile("assets/images/menu_background.png")) {
        std::cout << "No se pudo cargar menu_background.png" << std::endl;
    }
    /*
    if (!gameBackgroundTexture.loadFromFile("assets/images/game_background.png")) {
        std::cout << "No se pudo cargar game_background.png" << std::endl;
    }
    
    if (!gameOverBackgroundTexture.loadFromFile("assets/images/gameover_background.png")) {
        std::cout << "No se pudo cargar gameover_background.png" << std::endl;
    }
    */
    // Configurar sprite de fondo
    backgroundSprite.setPosition(0, 0);
    
    // Inicializar efecto Matrix
    showMatrixEffect = false;
    initMatrixEffect();
    
    // Inicializar nivel
    currentLevel = 1;
    
    // Inicializar sistema de menú terminal
    currentInput = "";
    targetPlay = "play";
    targetExit = "exit";
    targetControls = "controls";
    targetInferno = "inferno";
    playProgress = {false, false, false, false}; // p, l, a, y
    exitProgress = {false, false, false, false}; // e, x, i, t
    controlsProgress = {false, false, false, false, false, false, false, false}; // c, o, n, t, r, o, l, s
    infernoProgress = {false, false, false, false, false, false, false}; // i, n, f, e, r, n, o
    
    // Inicializar modo de juego
    isInfernoMode = false;
    
    // Inicializar efectos caóticos del modo Inferno
    infernoShakeTimer = 0.0f;
    nextInfernoShakeTime = 5.0f + static_cast<float>(rand() % 26); // 5-30 segundos
    infernoHitCount = 0;
    infernoBrightnessGlitch = 1.0f;
    glitchTimer = 0.0f;
    isGlitching = false;
    screenCracks.clear();
    
    // Inicializar sistema de menú de controles
    currentControlsInput = "";
    returnProgress = {false, false, false, false, false, false}; // r, e, t, u, r, n
    
    // Configurar textos del menú terminal
    terminalPrompt.setFont(font);
    terminalPrompt.setString("C:\\ARKANOID> ");
    terminalPrompt.setCharacterSize(terminalFontSize);
    terminalPrompt.setFillColor(sf::Color::Green);
    terminalPrompt.setPosition(terminalPromptX, terminalPromptY);
    
    titleCommand.setFont(font);
    titleCommand.setString("> ARKANOID TERMINAL v1.0");
    titleCommand.setCharacterSize(20);
    titleCommand.setFillColor(sf::Color::Cyan);
    titleCommand.setPosition(terminalPromptX, terminalTitleY);
    
    playCommand.setFont(font);
    playCommand.setString("PLAY - START GAME");
    playCommand.setCharacterSize(commandFontSize);
    playCommand.setFillColor(sf::Color::White);
    playCommand.setPosition(50, commandListY);
    
    exitCommand.setFont(font);
    exitCommand.setString("EXIT - QUIT GAME");
    exitCommand.setCharacterSize(commandFontSize);
    exitCommand.setFillColor(sf::Color::White);
    exitCommand.setPosition(50, commandListY + 30);
    
    controlsCommand.setFont(font);
    controlsCommand.setString("CONTROLS - VIEW GAME CONTROLS");
    controlsCommand.setCharacterSize(commandFontSize);
    controlsCommand.setFillColor(sf::Color::White);
    controlsCommand.setPosition(50, commandListY + 60);
    
    infernoCommand.setFont(font);
    infernoCommand.setString("INFERNO - START HELL MODE");
    infernoCommand.setCharacterSize(commandFontSize);
    infernoCommand.setFillColor(sf::Color(255, 69, 0)); // Rojo-naranja intenso
    infernoCommand.setPosition(50, commandListY + 90);
    
    // Inicializar sistema de menú terminal de game over
    currentGameOverInput = "";
    targetReboot = "reboot";
    rebootProgress = {false, false, false, false, false, false}; // r, e, b, o, o, t
    gameOverExitProgress = {false, false, false, false}; // e, x, i, t
    
    gameOverPrompt.setFont(font);
    gameOverPrompt.setString("SYSTEM> ");
    gameOverPrompt.setCharacterSize(terminalFontSize);
    gameOverPrompt.setFillColor(sf::Color::Red);
    gameOverPrompt.setPosition(50, gameOverPromptY);
    
    rebootCommand.setFont(font);
    rebootCommand.setString("REBOOT - Reiniciar sistema");
    rebootCommand.setCharacterSize(commandFontSize);
    rebootCommand.setFillColor(sf::Color::White);
    rebootCommand.setPosition(50, gameOverCommandsY);
    
    gameOverExitCommand.setFont(font);
    gameOverExitCommand.setString("EXIT - Cerrar sistema");
    gameOverExitCommand.setCharacterSize(commandFontSize);
    gameOverExitCommand.setFillColor(sf::Color::White);
    gameOverExitCommand.setPosition(50, gameOverCommandsY + 30);
    
    // Inicializar música del menú automáticamente
    menuMusic.play();
}

Game::~Game() {
    // Liberar memoria de las músicas de fondo
    for (auto* track : backgroundMusicTracks) {
        delete track;
    }
    backgroundMusicTracks.clear();
    
    // Liberar memoria de las músicas del modo Inferno
    for (auto* track : infernoMusicTracks) {
        delete track;
    }
    infernoMusicTracks.clear();
}

// Bucle principal (Game Loop)
void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

// Manejo de inputs (Teclado/Mouse)
void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
            
        // Capturar entrada de texto para el menú terminal
        if (event.type == sf::Event::TextEntered && state == GameState::Menu) {
            if (event.text.unicode >= 32 && event.text.unicode < 128) {
                char c = static_cast<char>(event.text.unicode);
                if (volumeInputMode) {
                    processVolumeInput(c);
                } else {
                    processTerminalInput(std::tolower(c));
                }
            }
        }
        
        // Capturar entrada de texto para game over terminal
        if (event.type == sf::Event::TextEntered && state == GameState::GameOver) {
            if (event.text.unicode >= 32 && event.text.unicode < 128) {
                char c = static_cast<char>(event.text.unicode);
                if (volumeInputMode) {
                    processVolumeInput(c);
                } else {
                    processGameOverInput(std::tolower(c));
                }
            }
        }
        
        // Capturar entrada de texto en modo Playing para volumen
        if (event.type == sf::Event::TextEntered && state == GameState::Playing && volumeInputMode) {
            if (event.text.unicode >= 32 && event.text.unicode < 128) {
                char c = static_cast<char>(event.text.unicode);
                processVolumeInput(c);
            }
        }
        
        // Capturar entrada de texto para el menú de controles
        if (event.type == sf::Event::TextEntered && state == GameState::Controls) {
            if (event.text.unicode >= 32 && event.text.unicode < 128) {
                char c = static_cast<char>(event.text.unicode);
                processControlsInput(std::tolower(c));
            }
        }
        
        if (event.type == sf::Event::KeyPressed) {
            if (state == GameState::Menu) {
                // Solo permitir ESC para salir en el menú
                if (event.key.code == sf::Keyboard::Escape) {
                    if (volumeInputMode) {
                        volumeInputMode = false;
                        volumeInput.clear();
                        std::cout << "Modo volumen cancelado" << std::endl;
                    } else {
                        menuMusic.stop();
                        if (currentBackgroundMusic) currentBackgroundMusic->stop();
                        if (currentLowLifeMusic) currentLowLifeMusic->stop();
                        window.close();
                    }
                }
                // Tecla V para activar modo volumen
                else if (event.key.code == sf::Keyboard::V && !volumeInputMode) {
                    volumeInputMode = true;
                    volumeInput.clear();
                    std::cout << "MODO VOLUMEN: Ingresa 00-99 y presiona Enter (ESC para cancelar)" << std::endl;
                }
                // Tecla M para reproducir audio especial
                else if (event.key.code == sf::Keyboard::M) {
                    specialSound.play();
                }
                // Enter para aplicar volumen
                else if (event.key.code == sf::Keyboard::Enter && volumeInputMode) {
                    if (volumeInput.length() >= 1) {
                        int volume = std::stoi(volumeInput);
                        if (volume >= 0 && volume <= 99) {
                            masterVolume = volume / 99.0f; // Convertir 0-99 a 0.0-1.0
                            updateMasterVolume();
                            std::cout << "Volumen establecido a: " << volume << "%" << std::endl;
                        }
                    }
                    volumeInput.clear();
                    volumeInputMode = false;
                }
                // Backspace para borrar
                else if (event.key.code == sf::Keyboard::BackSpace) {
                    if (volumeInputMode && !volumeInput.empty()) {
                        volumeInput.pop_back();
                    } else if (!currentInput.empty()) {
                        currentInput.pop_back();
                        updateTerminalDisplay();
                    }
                }
            }
            else if (state == GameState::GameOver) {
                // ESC para salir o cancelar volumen
                if (event.key.code == sf::Keyboard::Escape) {
                    if (volumeInputMode) {
                        volumeInputMode = false;
                        volumeInput.clear();
                        std::cout << "Modo volumen cancelado" << std::endl;
                    } else {
                        menuMusic.stop();
                        if (currentBackgroundMusic) currentBackgroundMusic->stop();
                        if (currentLowLifeMusic) currentLowLifeMusic->stop();
                        window.close();
                    }
                }
                // Enter para aplicar volumen
                else if (event.key.code == sf::Keyboard::Enter && volumeInputMode) {
                    if (volumeInput.length() >= 1) {
                        int volume = std::stoi(volumeInput);
                        if (volume >= 0 && volume <= 99) {
                            masterVolume = volume / 99.0f;
                            updateMasterVolume();
                            std::cout << "Volumen establecido a: " << volume << "%" << std::endl;
                        }
                    }
                    volumeInput.clear();
                    volumeInputMode = false;
                }
                // Tecla V para activar modo volumen
                else if (event.key.code == sf::Keyboard::V && !volumeInputMode) {
                    volumeInputMode = true;
                    volumeInput.clear();
                    std::cout << "MODO VOLUMEN: Ingresa 00-99 y presiona Enter (ESC para cancelar)" << std::endl;
                }
                // Tecla M para reproducir audio especial
                else if (event.key.code == sf::Keyboard::M) {
                    specialSound.play();
                }
                // Backspace para borrar
                else if (event.key.code == sf::Keyboard::BackSpace) {
                    if (volumeInputMode && !volumeInput.empty()) {
                        volumeInput.pop_back();
                    } else if (!currentGameOverInput.empty()) {
                        currentGameOverInput.pop_back();
                    }
                }
            }
            else if (state == GameState::Controls) {
                // ESC para volver al menú
                if (event.key.code == sf::Keyboard::Escape) {
                    state = GameState::Menu;
                    currentControlsInput = "";
                    returnProgress = {false, false, false, false, false, false};
                }
                // Backspace para borrar
                else if (event.key.code == sf::Keyboard::BackSpace && !currentControlsInput.empty()) {
                    currentControlsInput.pop_back();
                }
            }
            else if (state == GameState::Playing) {
                if (event.key.code == sf::Keyboard::Space && ball.isStuck) {
                    ball.velocity = sf::Vector2f(0.f, -350.f); // Lanzar derecha hacia arriba
                    ball.isStuck = false;
                }
                // ESC para menú o cancelar volumen
                else if (event.key.code == sf::Keyboard::Escape) {
                    if (volumeInputMode) {
                        volumeInputMode = false;
                        volumeInput.clear();
                        std::cout << "Modo volumen cancelado" << std::endl;
                    } else {
                        state = GameState::Menu;
                        if (currentBackgroundMusic) currentBackgroundMusic->stop();
                        if (currentLowLifeMusic) currentLowLifeMusic->stop();
                        isPlayingLowLifeMusic = false;
                        menuMusic.play();
                    }
                }
                // Enter para aplicar volumen
                else if (event.key.code == sf::Keyboard::Enter && volumeInputMode) {
                    if (volumeInput.length() >= 1) {
                        int volume = std::stoi(volumeInput);
                        if (volume >= 0 && volume <= 99) {
                            masterVolume = volume / 99.0f;
                            updateMasterVolume();
                            std::cout << "Volumen establecido a: " << volume << "%" << std::endl;
                        }
                    }
                    volumeInput.clear();
                    volumeInputMode = false;
                }
                // Tecla V para activar modo volumen
                else if (event.key.code == sf::Keyboard::V && !volumeInputMode) {
                    volumeInputMode = true;
                    volumeInput.clear();
                    std::cout << "MODO VOLUMEN: Ingresa 00-99 y presiona Enter (ESC para cancelar)" << std::endl;
                }
                // Tecla M para reproducir audio especial
                else if (event.key.code == sf::Keyboard::M) {
                    specialSound.play();
                }
                // Tecla S para activar/desactivar velocidad del paddle (solo si tiene el PowerUp y no es nivel 0)
                else if (event.key.code == sf::Keyboard::S && currentLevel > 0) {
                    // Verificar si hay algún SpeedPaddle activo
                    bool hasSpeedPaddle = false;
                    for (const auto& pup : activePowerUps) {
                        if (pup.type == PowerUpType::SpeedPaddle) {
                            hasSpeedPaddle = true;
                            break;
                        }
                    }
                    
                    if (hasSpeedPaddle) {
                        // Alternar el estado del paddle: aplicar o remover el boost
                        // Buscar si está actualmente boosteado
                        bool isBoosted = (paddle.speed > originalPaddleSpeed + 10.f);
                        
                        if (isBoosted) {
                            paddle.speed = originalPaddleSpeed;
                            paddle.setFillColor(sf::Color::White);  // Blanco normal
                        } else {
                            paddle.speed = originalPaddleSpeed * 1.5f;
                            paddle.setFillColor(sf::Color(173, 216, 230));  // Azul cielo pastel
                        }
                    }
                }
                // Backspace para borrar
                else if (event.key.code == sf::Keyboard::BackSpace && volumeInputMode && !volumeInput.empty()) {
                    volumeInput.pop_back();
                }
            }
        }
    }
}

// Lógica (Movimiento, colisiones)
void Game::update() {
    float dt = clock.restart().asSeconds();
    
    // Aplicar SlowMotion si está activo
    bool hasSlowMotion = false;
    for (const auto& pup : activePowerUps) {
        if (pup.type == PowerUpType::SlowMotion) {
            hasSlowMotion = true;
            break;
        }
    }
    if (hasSlowMotion) {
        dt *= slowMotionMultiplier;  // Multiplicar por 0.5f = 50% de velocidad
        
        // Ralentizar la música
        if (currentBackgroundMusic) {
            currentBackgroundMusic->setPitch(slowMotionMultiplier);  // 0.5f = 50% de velocidad
        }
        if (currentLowLifeMusic) {
            currentLowLifeMusic->setPitch(slowMotionMultiplier);
        }
        
        // Ralentizar todos los efectos de sonido
        bounceSound.setPitch(slowMotionMultiplier);
        gameOverSound.setPitch(slowMotionMultiplier);
        specialSound.setPitch(slowMotionMultiplier);
    } else {
        // Restaurar velocidad normal de la música y efectos
        if (currentBackgroundMusic) {
            currentBackgroundMusic->setPitch(1.0f);
        }
        if (currentLowLifeMusic) {
            currentLowLifeMusic->setPitch(1.0f);
        }
        
        // Restaurar velocidad normal de los efectos de sonido
        bounceSound.setPitch(1.0f);
        gameOverSound.setPitch(1.0f);
        specialSound.setPitch(1.0f);
    }
    
    // Actualizar efecto Matrix si está activo
    if (showMatrixEffect && state == GameState::GameOver) {
        updateMatrixEffect();
    }
    
    if (state != GameState::Playing) return;

    // --- Paddle: mover según teclado ---
    sf::Vector2f paddlePos = paddle.getPosition();
    float halfWidth = paddle.getSize().x / 2.f;
    
    // Detectar movimiento y ajustar rotación objetivo
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        paddle.move(-paddle.speed * dt, 0.f);
        targetPaddleRotation = -2.f;  // Inclinar a la izquierda
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        paddle.move(paddle.speed * dt, 0.f);
        targetPaddleRotation = 2.f;   // Inclinar a la derecha
    } else {
        targetPaddleRotation = 0.f;   // Volver a horizontal
    }
    
    // Suavizar rotación (interpolación)
    paddleRotation += (targetPaddleRotation - paddleRotation) * 12.f * dt;
    paddle.setRotation(paddleRotation);
    
    // Clamp paddle dentro de la ventana
    if (paddle.getPosition().x - halfWidth < 0.f) paddle.setPosition(halfWidth, paddle.getPosition().y);
    if (paddle.getPosition().x + halfWidth > window.getSize().x) paddle.setPosition(window.getSize().x - halfWidth, paddle.getPosition().y);

    // --- Bola: manejar si está pegada o libre ---
    if (ball.isStuck) {
        // Mantener la bola pegada encima del paddle
        ball.setPosition(paddle.getPosition().x, paddle.getPosition().y - paddle.getSize().y/2.f - ball.getRadius() - 2.f);
    } else {
        // Actualizar posición normalmente
        ball.update(dt);
        
        // Limitar velocidad máxima para evitar atravesar objetos
        float currentSpeed = std::sqrt(ball.velocity.x * ball.velocity.x + ball.velocity.y * ball.velocity.y);
        float maxSpeed = isInfernoMode ? 650.0f : 500.0f;  // Inferno más rápido pero controlado
        if (currentSpeed > maxSpeed) {
            float scale = maxSpeed / currentSpeed;
            ball.velocity.x *= scale;
            ball.velocity.y *= scale;
        }
    }

    // Colisión con paredes (izquierda/derecha)
    float r = ball.getRadius();
    sf::Vector2f pos = ball.getPosition();
    if (pos.x - r < 0.f) {
        ball.setPosition(r, pos.y);
        ball.velocity.x = std::abs(ball.velocity.x);
        bounceSound.play();  // Sonido de rebote
        if (isInfernoMode) {
            screenShakeDuration = 0.12f;
            screenShakeIntensity = 4.5f;  // Mucho más intenso
        } else {
            screenShakeDuration = 0.08f;
            screenShakeIntensity = 2.5f;
        }
    }
    if (pos.x + r > window.getSize().x) {
        ball.setPosition(static_cast<float>(window.getSize().x) - r, pos.y);
        ball.velocity.x = -std::abs(ball.velocity.x);
        bounceSound.play();  // Sonido de rebote
        if (isInfernoMode) {
            screenShakeDuration = 0.12f;
            screenShakeIntensity = 4.5f;  // Mucho más intenso
        } else {
            screenShakeDuration = 0.08f;
            screenShakeIntensity = 2.5f;
        }
    }
    // Colisión con techo
    if (pos.y - r < 0.f) {
        ball.setPosition(pos.x, r);
        ball.velocity.y = std::abs(ball.velocity.y);
        bounceSound.play();  // Sonido de rebote
        if (isInfernoMode) {
            screenShakeDuration = 0.12f;
            screenShakeIntensity = 4.5f;  // Mucho más intenso
        } else {
            screenShakeDuration = 0.08f;
            screenShakeIntensity = 2.5f;
        }
    }
    // Si la bola principal cae abajo
    if (pos.y - r > window.getSize().y) {
        // Si hay bolas extra, promover una a bola principal
        if (!extraBalls.empty()) {
            // Tomar la primera bola extra y convertirla en principal
            Ball& newMain = extraBalls.front();
            ball.setPosition(newMain.getPosition());
            ball.velocity = newMain.velocity;
            ball.isStuck = false;
            // Remover esa bola de las extras
            extraBalls.erase(extraBalls.begin());
        } else {
            // No quedan bolas, perder vida
            loseLife();
        }
    }
    
    // Actualizar bolas extra
    for (auto& extraBall : extraBalls) {
        extraBall.update(dt);
        
        // Limitar velocidad máxima de bolas extra también
        float extraSpeed = std::sqrt(extraBall.velocity.x * extraBall.velocity.x + extraBall.velocity.y * extraBall.velocity.y);
        float maxExtraSpeed = isInfernoMode ? 650.0f : 500.0f;
        if (extraSpeed > maxExtraSpeed) {
            float scale = maxExtraSpeed / extraSpeed;
            extraBall.velocity.x *= scale;
            extraBall.velocity.y *= scale;
        }
        
        // Colisiones de bolas extra con paredes
        sf::Vector2f extraPos = extraBall.getPosition();
        float extraR = extraBall.getRadius();
        
        if (extraPos.x - extraR < 0.f) {
            extraBall.setPosition(extraR, extraPos.y);
            extraBall.velocity.x = std::abs(extraBall.velocity.x);
            bounceSound.play();
            screenShakeDuration = 0.08f;
            screenShakeIntensity = 2.5f;
        }
        if (extraPos.x + extraR > window.getSize().x) {
            extraBall.setPosition(static_cast<float>(window.getSize().x) - extraR, extraPos.y);
            extraBall.velocity.x = -std::abs(extraBall.velocity.x);
            bounceSound.play();
            screenShakeDuration = 0.08f;
            screenShakeIntensity = 2.5f;
        }
        if (extraPos.y - extraR < 0.f) {
            extraBall.setPosition(extraPos.x, extraR);
            extraBall.velocity.y = std::abs(extraBall.velocity.y);
            bounceSound.play();
        }
        
        // Colisión bola extra con paddle
        if (extraBall.getGlobalBounds().intersects(paddle.getGlobalBounds()) && extraBall.velocity.y > 0) {
            extraBall.velocity.y = -std::abs(extraBall.velocity.y);
            float paddleCenter = paddle.getPosition().x;
            float extraBallCenter = extraBall.getPosition().x;
            float offset = (extraBallCenter - paddleCenter) / (paddle.getSize().x / 2.f);
            extraBall.velocity.x = offset * 300.f;
            bounceSound.play();
        }
        
        // Colisión bola extra con bloques indestructibles (solo rebote)
        for (auto& brick : bricks) {
            if (brick.isDestroyed) continue;
            if (!brick.isIndestructible) continue;  // Solo procesar indestructibles
            
            if (extraBall.getGlobalBounds().intersects(brick.getGlobalBounds())) {
                // Determinar de qué lado viene la pelota
                float ballCenterX = extraBall.getPosition().x;
                float ballCenterY = extraBall.getPosition().y;
                float brickLeft = brick.getPosition().x;
                float brickRight = brick.getPosition().x + brick.getSize().x;
                float brickTop = brick.getPosition().y;
                float brickBottom = brick.getPosition().y + brick.getSize().y;
                
                // Calcular la distancia a cada lado del brick
                float distLeft = ballCenterX - brickLeft;
                float distRight = brickRight - ballCenterX;
                float distTop = ballCenterY - brickTop;
                float distBottom = brickBottom - ballCenterY;
                
                // Encontrar el lado más cercano
                float minDist = std::min({distLeft, distRight, distTop, distBottom});
                
                if (minDist == distLeft || minDist == distRight) {
                    // Colisión desde los lados - invertir velocidad X
                    extraBall.velocity.x = -extraBall.velocity.x;
                    
                    // Reposicionar la bola fuera del bloque
                    if (minDist == distLeft) {
                        extraBall.setPosition(brickLeft - extraR, ballCenterY);  // Empujar a la izquierda
                    } else {
                        extraBall.setPosition(brickRight + extraR, ballCenterY);  // Empujar a la derecha
                    }
                } else {
                    // Colisión desde arriba o abajo - invertir velocidad Y
                    extraBall.velocity.y = -extraBall.velocity.y;
                    
                    // Reposicionar la bola fuera del bloque
                    if (minDist == distTop) {
                        extraBall.setPosition(ballCenterX, brickTop - extraR);  // Empujar hacia arriba
                    } else {
                        extraBall.setPosition(ballCenterX, brickBottom + extraR);  // Empujar hacia abajo
                    }
                }
                
                bounceSound.play();
                break; // Solo procesar una colisión por frame
            }
        }
        
        // Colisión bola extra con bricks
        for (auto& brick : bricks) {
            if (brick.isDestroyed) continue;
            if (brick.isIndestructible) continue;  // Saltar bloques indestructibles
            if (extraBall.getGlobalBounds().intersects(brick.getGlobalBounds())) {
                brick.hitsRemaining--;
                
                // Incrementar contador de golpes para grietas en Inferno
                if (isInfernoMode) {
                    infernoHitCount++;
                    addScreenCrack();
                }
                
                if (brick.hitsRemaining <= 0) {
                    brick.isDestroyed = true;
                    score += brick.pointValue;
                    createBrickParticles(brick.getPosition(), brick.getFillColor());
                    
                    if (brick.isSmallBlock) {
                        if (isInfernoMode) {
                            extraBall.velocity *= 1.12f; // 12% más rápido en modo Inferno
                        } else {
                            extraBall.velocity *= 1.05f; // 5% en modo normal
                        }
                    }
                } else {
                    // Partículas exageradas para ladrillos resistentes (aún tienen golpes)
                    int heavyParticles = isInfernoMode ? 24 : 12;  // Doble en Inferno
                    for (int i = 0; i < heavyParticles; i++) {
                        Particle p;
                        
                        // MODO INFERNO: Partículas mucho más grandes y rojas
                        if (isInfernoMode) {
                            p.shape.setSize(sf::Vector2f(6.f, 6.f));  // Más grandes
                            sf::Color brickColor = brick.getFillColor();
                            sf::Uint8 r = std::min(255, static_cast<int>(brickColor.r) + 100);
                            sf::Uint8 g = static_cast<sf::Uint8>(brickColor.g * 0.3f);
                            sf::Uint8 b = static_cast<sf::Uint8>(brickColor.b * 0.2f);
                            p.shape.setFillColor(sf::Color(r, g, b));
                        } else {
                            p.shape.setSize(sf::Vector2f(4.f, 4.f));
                            p.shape.setFillColor(brick.getFillColor());
                        }
                        
                        p.shape.setPosition(brick.getPosition());
                        
                        float angle = (std::rand() % 360) * 3.14159f / 180.f;
                        float speed = isInfernoMode ? (150.f + (std::rand() % 200)) : (80.f + (std::rand() % 120));
                        p.velocity.x = std::cos(angle) * speed;
                        p.velocity.y = std::sin(angle) * speed;
                        
                        p.lifetime = isInfernoMode ? (0.7f + (std::rand() % 500) / 1000.f) : (0.5f + (std::rand() % 400) / 1000.f);
                        p.maxLifetime = p.lifetime;
                        
                        particles.push_back(p);
                    }
                }
                
                extraBall.velocity.y *= -1;
                bounceSound.play();
                break;
            }
        }
    }
    
    // Eliminar bolas extra que caen
    extraBalls.erase(
        std::remove_if(extraBalls.begin(), extraBalls.end(),
            [this](const Ball& b) { return b.getPosition().y > window.getSize().y; }),
        extraBalls.end()
    );

    // Colisión con paddle
    if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds()) && ball.velocity.y > 0.f) {
        // Calcular donde golpeó la pala para ajustar el ángulo
        float relative = (ball.getPosition().x - paddle.getPosition().x) / (paddle.getSize().x / 2.f); // -1 .. 1
        float bounceAngle = relative * 0.75f; // ajustar para limitar el ángulo
        float speed = std::sqrt(ball.velocity.x*ball.velocity.x + ball.velocity.y*ball.velocity.y);
        ball.velocity.x = speed * bounceAngle;
        ball.velocity.y = -std::abs(ball.velocity.y);
        // Alejar la bola para evitar múltiples colisiones
        ball.setPosition(ball.getPosition().x, paddle.getPosition().y - paddle.getSize().y/2.f - r - 0.5f);
        bounceSound.play();  // Sonido de rebote
    }

    // Colisión con bloques indestructibles (solo rebote, sin destrucción)
    for (auto& brick : bricks) {
        if (brick.isDestroyed) continue;
        if (!brick.isIndestructible) continue;  // Solo procesar indestructibles
        
        if (ball.getGlobalBounds().intersects(brick.getGlobalBounds())) {
            // Determinar de qué lado viene la pelota
            float ballCenterX = ball.getPosition().x;
            float ballCenterY = ball.getPosition().y;
            float brickLeft = brick.getPosition().x;
            float brickRight = brick.getPosition().x + brick.getSize().x;
            float brickTop = brick.getPosition().y;
            float brickBottom = brick.getPosition().y + brick.getSize().y;
            
            // Calcular la distancia a cada lado del brick
            float distLeft = ballCenterX - brickLeft;
            float distRight = brickRight - ballCenterX;
            float distTop = ballCenterY - brickTop;
            float distBottom = brickBottom - ballCenterY;
            
            // Encontrar el lado más cercano
            float minDist = std::min({distLeft, distRight, distTop, distBottom});
            
            float r = ball.getRadius();
            
            if (minDist == distLeft || minDist == distRight) {
                // Colisión desde los lados - invertir velocidad X
                ball.velocity.x = -ball.velocity.x;
                
                // Reposicionar la pelota fuera del bloque
                if (minDist == distLeft) {
                    ball.setPosition(brickLeft - r, ballCenterY);  // Empujar a la izquierda
                } else {
                    ball.setPosition(brickRight + r, ballCenterY);  // Empujar a la derecha
                }
            } else {
                // Colisión desde arriba o abajo - invertir velocidad Y
                ball.velocity.y = -ball.velocity.y;
                
                // Reposicionar la pelota fuera del bloque
                if (minDist == distTop) {
                    ball.setPosition(ballCenterX, brickTop - r);  // Empujar hacia arriba
                } else {
                    ball.setPosition(ballCenterX, brickBottom + r);  // Empujar hacia abajo
                }
            }
            
            bounceSound.play();
            break; // Solo procesar una colisión por frame
        }
    }

    // Colisión con ladrillos
    for (auto& brick : bricks) {
        if (brick.isDestroyed) continue;
        if (brick.isIndestructible) continue;  // Saltar bloques indestructibles
        if (ball.getGlobalBounds().intersects(brick.getGlobalBounds())) {
            // Reducir golpes restantes
            brick.hitsRemaining--;
            
            // Incrementar contador de golpes para grietas en Inferno
            if (isInfernoMode) {
                infernoHitCount++;
                addScreenCrack();
            }
            
            // Si ya no quedan golpes, destruir el bloque
            if (brick.hitsRemaining <= 0) {
                brick.isDestroyed = true;
                score += brick.pointValue;
                createBrickParticles(brick.getPosition(), brick.getFillColor());
                
                // 20% probabilidad de generar PowerUp
                if (std::rand() % 100 < 20) {
                    // Tipo aleatorio de PowerUp con probabilidades diferentes
                    int rarity = std::rand() % 100;
                    PowerUpType type;
                    const sf::Texture* texture;
                    
                    if (rarity < 3) {  // 3% - MUY RARO: Vida extra (ventaja)
                        type = PowerUpType::ExtraLife;
                        texture = &powerUpExtraLifeTexture;
                    } else if (rarity < 25) {  // 22% - SpeedPaddle (ventaja)
                        type = PowerUpType::SpeedPaddle;
                        texture = &powerUpSpeedTexture;
                    } else if (rarity < 45) {  // 20% - ExpandPaddle (ventaja)
                        type = PowerUpType::ExpandPaddle;
                        texture = &powerUpExpandTexture;
                    } else if (rarity < 60) {  // 15% - MultiBall (ventaja)
                        type = PowerUpType::MultiBall;
                        texture = &powerUpMultiBallTexture;
                    } else if (rarity < 75) {  // 15% - ShrinkPaddle (DESVENTAJA)
                        type = PowerUpType::ShrinkPaddle;
                        texture = &powerUpShrinkTexture;
                    } else if (rarity < 87) {  // 12% - SlowPaddle (DESVENTAJA)
                        type = PowerUpType::SlowPaddle;
                        texture = &powerUpSlowTexture;
                    } else {  // 13% - SlowMotion (DESVENTAJA)
                        type = PowerUpType::SlowMotion;
                        texture = &powerUpSlowMotionTexture;
                    }
                    
                    PowerUp powerUp(brick.getPosition().x, brick.getPosition().y, type);
                    powerUp.setTexture(texture);
                    powerUps.push_back(powerUp);
                }
                
                // Efecto especial para bloques pequeños: aumentar velocidad
                if (brick.isSmallBlock) {
                    if (isInfernoMode) {
                        ball.velocity *= 1.12f; // 12% más rápido en modo Inferno
                    } else {
                        ball.velocity *= 1.05f; // 5% en modo normal
                    }
                }
            } else {
                // Partículas exageradas para ladrillos resistentes (aún tienen golpes)
                int heavyParticles = isInfernoMode ? 24 : 12;  // Doble en Inferno
                for (int i = 0; i < heavyParticles; i++) {
                    Particle p;
                    
                    // MODO INFERNO: Partículas mucho más grandes y rojas
                    if (isInfernoMode) {
                        p.shape.setSize(sf::Vector2f(6.f, 6.f));  // Más grandes
                        sf::Color brickColor = brick.getFillColor();
                        sf::Uint8 r = std::min(255, static_cast<int>(brickColor.r) + 100);
                        sf::Uint8 g = static_cast<sf::Uint8>(brickColor.g * 0.3f);
                        sf::Uint8 b = static_cast<sf::Uint8>(brickColor.b * 0.2f);
                        p.shape.setFillColor(sf::Color(r, g, b));
                    } else {
                        p.shape.setSize(sf::Vector2f(4.f, 4.f));
                        p.shape.setFillColor(brick.getFillColor());
                    }
                    
                    p.shape.setPosition(brick.getPosition());
                    
                    float angle = (std::rand() % 360) * 3.14159f / 180.f;
                    float speed = isInfernoMode ? (150.f + (std::rand() % 200)) : (80.f + (std::rand() % 120));
                    p.velocity.x = std::cos(angle) * speed;
                    p.velocity.y = std::sin(angle) * speed;
                    
                    p.lifetime = isInfernoMode ? (0.7f + (std::rand() % 500) / 1000.f) : (0.5f + (std::rand() % 400) / 1000.f);
                    p.maxLifetime = p.lifetime;
                    
                    particles.push_back(p);
                }
                
                // Efecto especial para bloques de 3 golpes: aumentar restitución
                if (brick.maxHits == 3) {
                    float speedIncrease = 1.02f;  // 2% más velocidad por golpe
                    ball.velocity *= speedIncrease;
                }
                // Cambiar color según el daño recibido
                if (brick.maxHits == 3) {  // Bloques morados
                    if (brick.hitsRemaining == 2) {
                        brick.setFillColor(sf::Color(120, 0, 150));  // Morado medio
                    } else if (brick.hitsRemaining == 1) {
                        brick.setFillColor(sf::Color(80, 0, 100));   // Morado oscuro
                    }
                }
                else if (brick.maxHits == 2) {  // Bloques rojos
                    brick.setFillColor(sf::Color(150, 0, 0));  // Rojo oscuro
                }
            }
            
            // Invertir componente Y de la velocidad
            ball.velocity.y = -ball.velocity.y;
            bounceSound.play();  // Sonido de rebote
            break; // solo procesar una colisión por frame
        }
    }
    
    // Verificar si todos los bloques DESTRUCTIBLES están destruidos
    bool allDestroyed = true;
    for (const auto& brick : bricks) {
        // Solo contar bloques destructibles (no indestructibles)
        if (!brick.isIndestructible && !brick.isDestroyed) {
            allDestroyed = false;
            break;
        }
    }
    
    // Si todos los bloques destructibles están destruidos, avanzar al siguiente nivel
    if (allDestroyed && !bricks.empty()) {
        currentLevel++;  // Incrementar nivel
        
        // Limpiar PowerUps y bolas extra
        powerUps.clear();
        extraBalls.clear();
        activePowerUps.clear();  // Limpiar poder-ups activos
        
        // Reiniciar paddle a tamaño y velocidad original
        sf::Vector2f originalSize(originalPaddleWidth, paddle.getSize().y);
        paddle.setSize(originalSize);
        paddle.setOrigin(originalSize.x / 2.f, originalSize.y / 2.f);
        paddle.speed = originalPaddleSpeed;
        paddle.setFillColor(sf::Color::White);  // Color normal
        
        // Inicializar el nuevo nivel
        initLevel();
        
        // Reiniciar bola pegada al paddle
        ball.isStuck = true;
        ball.velocity = sf::Vector2f(0.f, 0.f);
    }
    
    // Actualizar PowerUps
    for (auto& powerUp : powerUps) {
        if (powerUp.isActive()) {
            powerUp.update(dt);
            
            // Verificar colisión con paddle
            if (powerUp.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
                powerUp.deactivate();
                
                // Aplicar efecto según tipo
                switch(powerUp.getType()) {
                    case PowerUpType::SpeedPaddle:
                        {
                            ActivePowerUp newPowerUp;
                            newPowerUp.type = PowerUpType::SpeedPaddle;
                            newPowerUp.elapsedTime = 0.f;
                            newPowerUp.duration = speedPaddleDuration;
                            newPowerUp.paddleSpeed = originalPaddleSpeed;
                            activePowerUps.push_back(newPowerUp);
                            
                            paddle.speed = originalPaddleSpeed * 1.5f;
                            paddle.setFillColor(sf::Color(173, 216, 230));  // Azul cielo pastel
                        }
                        break;
                    case PowerUpType::ExpandPaddle:
                        {
                            ActivePowerUp newPowerUp;
                            newPowerUp.type = PowerUpType::ExpandPaddle;
                            newPowerUp.elapsedTime = 0.f;
                            newPowerUp.duration = expandPaddleDuration;
                            newPowerUp.paddleSize = paddle.getSize();
                            activePowerUps.push_back(newPowerUp);
                            
                            sf::Vector2f newSize(paddle.getSize().x * 1.5f, paddle.getSize().y);
                            paddle.setSize(newSize);
                            paddle.setOrigin(newSize.x / 2.f, newSize.y / 2.f);
                        }
                        break;
                    case PowerUpType::MultiBall:
                        // Crear 2 bolas adicionales
                        // Solo crear si la bola principal NO está pegada al paddle
                        if (!ball.isStuck) {
                            for (int i = 0; i < 2; i++) {
                                Ball newBall(ball.getRadius(), true);  // true = extra ball (colored)
                                newBall.setPosition(ball.getPosition());
                                newBall.speed = ball.speed;
                                float angle = (std::rand() % 60 - 30) * 3.14159f / 180.f;
                                newBall.velocity = sf::Vector2f(
                                    ball.velocity.x * std::cos(angle) - ball.velocity.y * std::sin(angle),
                                    ball.velocity.x * std::sin(angle) + ball.velocity.y * std::cos(angle)
                                );
                                newBall.isStuck = false;
                                extraBalls.push_back(newBall);
                            }
                        }
                        // Si la bola está pegada, el power-up simplemente no hace efecto
                        break;
                    case PowerUpType::ExtraLife:
                        if (lives < maxLives) {
                            lives++;
                            
                            // Si estaba en música de vida baja y ahora tiene más de 1 vida, restaurar música normal
                            if (isPlayingLowLifeMusic && lives > 1) {
                                if (currentLowLifeMusic) currentLowLifeMusic->stop();
                                isPlayingLowLifeMusic = false;
                                
                                if (currentBackgroundMusic) {
                                    currentBackgroundMusic->play();
                                }
                            }
                        }
                        break;
                    case PowerUpType::ShrinkPaddle:
                        {
                            ActivePowerUp newPowerUp;
                            newPowerUp.type = PowerUpType::ShrinkPaddle;
                            newPowerUp.elapsedTime = 0.f;
                            newPowerUp.duration = shrinkPaddleDuration;
                            newPowerUp.paddleSize = paddle.getSize();
                            activePowerUps.push_back(newPowerUp);
                            
                            // Reducir el tamaño de la paleta a 0.7x (desventaja)
                            sf::Vector2f newSize(paddle.getSize().x * 0.7f, paddle.getSize().y);
                            paddle.setSize(newSize);
                            paddle.setOrigin(newSize.x / 2.f, newSize.y / 2.f);
                            paddle.setFillColor(sf::Color::Magenta);  // Color magenta para indicar desventaja
                        }
                        break;
                    case PowerUpType::SlowPaddle:
                        {
                            ActivePowerUp newPowerUp;
                            newPowerUp.type = PowerUpType::SlowPaddle;
                            newPowerUp.elapsedTime = 0.f;
                            newPowerUp.duration = slowPaddleDuration;
                            newPowerUp.paddleSpeed = originalPaddleSpeed;
                            activePowerUps.push_back(newPowerUp);
                            
                            // Reducir la velocidad de la paleta a 0.6x (desventaja)
                            paddle.speed = originalPaddleSpeed * 0.6f;
                            paddle.setFillColor(sf::Color::Yellow);  // Color amarillo para indicar desventaja
                        }
                        break;
                    case PowerUpType::SlowMotion:
                        {
                            ActivePowerUp newPowerUp;
                            newPowerUp.type = PowerUpType::SlowMotion;
                            newPowerUp.elapsedTime = 0.f;
                            newPowerUp.duration = slowMotionDuration;
                            activePowerUps.push_back(newPowerUp);
                            
                            // El slowMotion se aplicará en el update() afectando el delta time
                            paddle.setFillColor(sf::Color(173, 216, 230));  // Azul claro
                        }
                        break;
                }
            }
            
            // Desactivar si sale de la pantalla
            if (powerUp.getPosition().y > window.getSize().y) {
                powerUp.deactivate();
            }
        }
    }
    
    // Limpiar PowerUps inactivos
    powerUps.erase(
        std::remove_if(powerUps.begin(), powerUps.end(),
            [](const PowerUp& p) { return !p.isActive(); }),
        powerUps.end()
    );
    
    // Actualizar duraciones de efectos activos
    for (int i = activePowerUps.size() - 1; i >= 0; --i) {
        activePowerUps[i].elapsedTime += dt;
        
        // Calcular porcentaje de progreso (0 = inicio, 1 = expiración)
        float progress = activePowerUps[i].elapsedTime / activePowerUps[i].duration;
        
        if (activePowerUps[i].elapsedTime >= activePowerUps[i].duration) {
            // Expiró este efecto
            if (activePowerUps[i].type == PowerUpType::SpeedPaddle) {
                // Al expirar un SpeedPaddle, reducir la velocidad proporcionalmente
                // Contar cuántos SpeedPaddle quedan después de eliminar este
                int remainingSpeedCount = 0;
                for (int j = 0; j < activePowerUps.size(); ++j) {
                    if (j != i && activePowerUps[j].type == PowerUpType::SpeedPaddle) {
                        remainingSpeedCount++;
                    }
                }
                
                // Remover este SpeedPaddle
                activePowerUps.erase(activePowerUps.begin() + i);
                
                // Calcular la nueva velocidad: reduce 1/1.5 por cada SpeedPaddle activo restante
                // Si no hay más, vuelve a la original
                if (remainingSpeedCount == 0) {
                    // No quedan SpeedPaddle, volver a la velocidad original
                    paddle.speed = originalPaddleSpeed;
                    paddle.setFillColor(sf::Color::White);
                } else {
                    // Quedan más SpeedPaddle, reducir la velocidad: dividir entre 1.5
                    paddle.speed = paddle.speed / 1.5f;
                    paddle.setFillColor(sf::Color(173, 216, 230));  // Mantener color boost
                }
            } 
            else if (activePowerUps[i].type == PowerUpType::ExpandPaddle) {
                // Al expirar un ExpandPaddle, reducir el tamaño proporcionalmente
                // Contar cuántos ExpandPaddle quedan después de eliminar este
                int remainingExpandCount = 0;
                for (int j = 0; j < activePowerUps.size(); ++j) {
                    if (j != i && activePowerUps[j].type == PowerUpType::ExpandPaddle) {
                        remainingExpandCount++;
                    }
                }
                
                // Remover este ExpandPaddle
                activePowerUps.erase(activePowerUps.begin() + i);
                
                // Calcular el nuevo tamaño: reduce 1/1.5 por cada ExpandPaddle activo restante
                // Si no hay más, vuelve al original
                if (remainingExpandCount == 0) {
                    // No quedan ExpandPaddle, volver al tamaño original
                    paddle.setSize(sf::Vector2f(originalPaddleWidth, paddle.getSize().y));
                    paddle.setOrigin(originalPaddleWidth / 2.f, paddle.getSize().y / 2.f);
                } else {
                    // Quedan más ExpandPaddle, reducir el tamaño: dividir entre 1.5
                    float newWidth = paddle.getSize().x / 1.5f;
                    paddle.setSize(sf::Vector2f(newWidth, paddle.getSize().y));
                    paddle.setOrigin(newWidth / 2.f, paddle.getSize().y / 2.f);
                }
            }
            else if (activePowerUps[i].type == PowerUpType::ShrinkPaddle) {
                // Al expirar ShrinkPaddle, restaurar el tamaño
                // Contar cuántos ShrinkPaddle quedan después de eliminar este
                int remainingShrinkCount = 0;
                for (int j = 0; j < activePowerUps.size(); ++j) {
                    if (j != i && activePowerUps[j].type == PowerUpType::ShrinkPaddle) {
                        remainingShrinkCount++;
                    }
                }
                
                // Remover este ShrinkPaddle
                activePowerUps.erase(activePowerUps.begin() + i);
                
                // Si no hay más ShrinkPaddle, volver al tamaño normal
                if (remainingShrinkCount == 0) {
                    paddle.setSize(sf::Vector2f(originalPaddleWidth, paddle.getSize().y));
                    paddle.setOrigin(originalPaddleWidth / 2.f, paddle.getSize().y / 2.f);
                    paddle.setFillColor(sf::Color::White);
                } else {
                    // Quedan más ShrinkPaddle, aumentar ligeramente el tamaño: multiplicar por 1/0.7
                    float newWidth = paddle.getSize().x / 0.7f;
                    paddle.setSize(sf::Vector2f(newWidth, paddle.getSize().y));
                    paddle.setOrigin(newWidth / 2.f, paddle.getSize().y / 2.f);
                    paddle.setFillColor(sf::Color::Magenta);
                }
            }
            else if (activePowerUps[i].type == PowerUpType::SlowPaddle) {
                // Al expirar SlowPaddle, restaurar la velocidad
                // Contar cuántos SlowPaddle quedan después de eliminar este
                int remainingSlowCount = 0;
                for (int j = 0; j < activePowerUps.size(); ++j) {
                    if (j != i && activePowerUps[j].type == PowerUpType::SlowPaddle) {
                        remainingSlowCount++;
                    }
                }
                
                // Remover este SlowPaddle
                activePowerUps.erase(activePowerUps.begin() + i);
                
                // Si no hay más SlowPaddle, volver a la velocidad normal
                if (remainingSlowCount == 0) {
                    paddle.speed = originalPaddleSpeed;
                    paddle.setFillColor(sf::Color::White);
                } else {
                    // Quedan más SlowPaddle, aumentar ligeramente la velocidad: multiplicar por 1/0.6
                    paddle.speed = paddle.speed / 0.6f;
                    paddle.setFillColor(sf::Color::Yellow);
                }
            }
            else if (activePowerUps[i].type == PowerUpType::SlowMotion) {
                // Al expirar SlowMotion, se restablece automáticamente el dt
                activePowerUps.erase(activePowerUps.begin() + i);
                paddle.setFillColor(sf::Color::White);
            }
        }
    }
    
    // Actualizar partículas
    updateParticles(dt);
    
    // Actualizar destello de pelota
    updateBallGlow();
    
    // Actualizar estelas de la pelota
    updateBallTrails(dt);
    
    // Actualizar efectos caóticos del modo Inferno
    updateInfernoEffects(dt);
    
    // Actualizar sacudida de pantalla
    if (screenShakeDuration > 0.f) {
        screenShakeDuration -= dt;
        float intensity = screenShakeIntensity * (screenShakeDuration / 0.08f);
        screenShakeOffset.x = ((std::rand() % 200 - 100) / 100.f) * intensity;
        screenShakeOffset.y = ((std::rand() % 200 - 100) / 100.f) * intensity;
    } else {
        screenShakeOffset = sf::Vector2f(0.f, 0.f);
    }
}

// Dibujado
void Game::render() {
    // ===============================================
    // CONSTANTES DE POSICIÓN Y TAMAÑO - RENDER
    // ===============================================
    
    // Posiciones del HUD en juego
    const float hudY = 65.f;
    const float hudSeparation = 40.f;
    
    window.clear(sf::Color::Black);
    
    // Aplicar sacudida de pantalla en Playing
    if (state == GameState::Playing) {
        sf::View view = window.getDefaultView();
        view.setCenter(window.getSize().x / 2.f + screenShakeOffset.x, 
                       window.getSize().y / 2.f + screenShakeOffset.y);
        window.setView(view);
    } else {
        window.setView(window.getDefaultView());
    }
    
    // Configurar y dibujar fondo según el estado
    if (state == GameState::Menu) {
        backgroundSprite.setTexture(menuBackgroundTexture);
        // Escalar dinámicamente
        if (menuBackgroundTexture.getSize().x > 0) {
            float scaleX = static_cast<float>(window.getSize().x) / menuBackgroundTexture.getSize().x;
            float scaleY = static_cast<float>(window.getSize().y) / menuBackgroundTexture.getSize().y;
            backgroundSprite.setScale(scaleX, scaleY);
        }
        window.draw(backgroundSprite);
    } else if (state == GameState::Playing) {
        backgroundSprite.setTexture(menuBackgroundTexture);  // Usar la misma textura del menú
        // Escalar dinámicamente
        if (menuBackgroundTexture.getSize().x > 0) {
            float scaleX = static_cast<float>(window.getSize().x) / menuBackgroundTexture.getSize().x;
            float scaleY = static_cast<float>(window.getSize().y) / menuBackgroundTexture.getSize().y;
            backgroundSprite.setScale(scaleX, scaleY);
        }
        
        // Aplicar tinte rojo progresivo según el nivel
        float redIntensity = std::min(1.0f, (currentLevel - 1) * 0.1f);  // 10% más rojo por nivel
        
        // MODO INFERNO: Filtro rojo sangriento mucho más intenso
        if (isInfernoMode) {
            redIntensity = std::min(1.0f, redIntensity + 0.6f);  // Agregar 60% de intensidad roja base
        }
        
        sf::Uint8 red = static_cast<sf::Uint8>(255);
        sf::Uint8 green = static_cast<sf::Uint8>(255 * (1.0f - redIntensity * 0.5f));  // Reducir verde
        sf::Uint8 blue = static_cast<sf::Uint8>(255 * (1.0f - redIntensity * 0.8f));   // Reducir azul más
        backgroundSprite.setColor(sf::Color(red, green, blue));
        
        window.draw(backgroundSprite);
    } else if (state == GameState::GameOver) {
        backgroundSprite.setTexture(menuBackgroundTexture);  // Usar la misma textura del menú
        backgroundSprite.setColor(sf::Color::White);  // Sin tinte
        // Escalar dinámicamente
        if (menuBackgroundTexture.getSize().x > 0) {
            float scaleX = static_cast<float>(window.getSize().x) / menuBackgroundTexture.getSize().x;
            float scaleY = static_cast<float>(window.getSize().y) / menuBackgroundTexture.getSize().y;
            backgroundSprite.setScale(scaleX, scaleY);
        }
        window.draw(backgroundSprite);
        
        // Mostrar efecto Matrix si está activado
        if (showMatrixEffect) {
            renderMatrixEffect();
        }
    }

    if (state == GameState::Menu) {
        renderMenu();
    } else if (state == GameState::Playing) {
        // Dibujar todos los ladrillos de la lista
        for (const auto& brick : bricks) {
            if (!brick.isDestroyed)
                window.draw(brick);
        }

        // Dibujar paddle
        window.draw(paddle);
        
        // Dibujar estelas de la pelota (atrás de la bola)
        for (const auto& trail : ballTrails) {
            window.draw(trail.shape);
        }
        
        // Dibujar destello de pelota (si hay velocidad)
        if (ballGlowIntensity > 0.f) {
            window.draw(ballGlow);
        }
        
        // Dibujar bola
        window.draw(ball);
        
        // Dibujar bolas extra
        for (const auto& extraBall : extraBalls) {
            window.draw(extraBall);
        }
        
        // Dibujar PowerUps
        for (const auto& powerUp : powerUps) {
            if (powerUp.isActive()) {
                window.draw(powerUp);
            }
        }
        
        // Dibujar partículas
        for (const auto& particle : particles) {
            window.draw(particle.shape);
        }
        
        // Actualizar y dibujar HUD (distribuido en la pantalla)
        livesText.setString("Vidas: " + std::to_string(lives));
        scoreText.setString("Puntos: " + std::to_string(score));
        
        // Texto de nivel
        sf::Text levelText;
        levelText.setFont(font);
        levelText.setCharacterSize(14);
        levelText.setFillColor(sf::Color::Yellow);
        levelText.setString("Nivel: " + std::to_string(currentLevel));
        
        // Display de volumen
        sf::Text volumeHUD;
        volumeHUD.setFont(font);
        volumeHUD.setCharacterSize(14);
        
        if (volumeInputMode) {
            std::string volDisplay = "VOL:[" + volumeInput + "_]";
            volumeHUD.setString(volDisplay);
            volumeHUD.setFillColor(sf::Color::Blue);
        } else {
            int currentVol = static_cast<int>(masterVolume * 99);
            std::string volDisplay = "VOL:" + std::to_string(currentVol) + "%";
            volumeHUD.setString(volDisplay);
            volumeHUD.setFillColor(sf::Color::Yellow);
        }
        
        // Indicador de modo Inferno
        sf::Text infernoModeText;
        if (isInfernoMode) {
            infernoModeText.setFont(font);
            infernoModeText.setCharacterSize(14);
            infernoModeText.setFillColor(sf::Color(220, 20, 20)); // Rojo intenso
            infernoModeText.setString("[INFERNO]");
            infernoModeText.setPosition(window.getSize().x - 230.f, hudY + 30.f); // A la izquierda del indicador [S]
        }
        
        // Posicionar manualmente (ajusta los valores en píxeles)
        volumeHUD.setPosition(820.f, hudY);      // Volumen
        scoreText.setPosition(350.f, hudY);     // Puntos
        livesText.setPosition(550.f, hudY);     // Vidas
        levelText.setPosition(690.f, hudY);     // Nivel
        
        window.draw(livesText);
        window.draw(scoreText);
        window.draw(levelText);
        window.draw(volumeHUD);
        if (isInfernoMode) {
            window.draw(infernoModeText);
        }
        
        // Dibujar HUD de power-ups activos con contador
        renderActivePowerUpsHUD();
        
        // Renderizar efectos caóticos del modo Inferno (grietas, glitches)
        renderInfernoEffects();
        
    } else if (state == GameState::Controls) {
        renderControls();
    } else if (state == GameState::GameOver) {
        renderGameOver();
    }

    window.display();
}

void Game::renderMenu() {
    // ===============================================
    // CONSTANTES DE POSICIÓN - MENÚ TERMINAL
    // ===============================================
    
    // Posiciones del menú terminal
    const float terminalPromptY = 150.f;
    const float progressStartY = 320.f;
    
    // Tamaños de fuente
    const int terminalFontSize = 16;
    const int progressFontSize = 14;
    
    // Dibujar menú tipo terminal
    window.draw(titleCommand);
    window.draw(terminalPrompt);
    
    // Dibujar input actual del usuario
    sf::Text currentInputText;
    currentInputText.setFont(font);
    currentInputText.setString(currentInput);
    currentInputText.setCharacterSize(terminalFontSize);
    currentInputText.setFillColor(sf::Color::White);
    currentInputText.setPosition(terminalPrompt.getPosition().x + terminalPrompt.getLocalBounds().width, terminalPromptY);
    window.draw(currentInputText);
    
    // Cursor parpadeante
    static sf::Clock cursorClock;
    if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
        cursorClock.restart();
    }
    if (cursorClock.getElapsedTime().asSeconds() < 0.25f) {
        sf::Text cursor;
        cursor.setFont(font);
        cursor.setString("_");
        cursor.setCharacterSize(16);
        cursor.setFillColor(sf::Color::Green);
        cursor.setPosition(currentInputText.getPosition().x + currentInputText.getLocalBounds().width, 150);
        window.draw(cursor);
    }
    
    // Dibujar comandos disponibles con progreso
    window.draw(playCommand);
    window.draw(exitCommand);
    window.draw(controlsCommand);
    window.draw(infernoCommand);
    
    // Mostrar progreso visual de "PLAY"
    std::string playDisplay = "PLAY: ";
    for (size_t i = 0; i < targetPlay.length(); ++i) {
        if (playProgress[i]) {
            playDisplay += targetPlay[i];
        } else {
            playDisplay += "_";
        }
    }
    sf::Text playProgressText;
    playProgressText.setFont(font);
    playProgressText.setString(playDisplay);
    playProgressText.setCharacterSize(progressFontSize);
    playProgressText.setFillColor(sf::Color::Yellow);
    playProgressText.setPosition(50, progressStartY + 60);
    window.draw(playProgressText);
    
    // Mostrar progreso visual de "EXIT"
    std::string exitDisplay = "EXIT: ";
    for (size_t i = 0; i < targetExit.length(); ++i) {
        if (exitProgress[i]) {
            exitDisplay += targetExit[i];
        } else {
            exitDisplay += "_";
        }
    }
    sf::Text exitProgressText;
    exitProgressText.setFont(font);
    exitProgressText.setString(exitDisplay);
    exitProgressText.setCharacterSize(progressFontSize);
    exitProgressText.setFillColor(sf::Color::Red);
    exitProgressText.setPosition(50, 405);
    window.draw(exitProgressText);
    
    // Mostrar progreso visual de "CONTROLS"
    std::string controlsDisplay = "CONTROLS: ";
    for (size_t i = 0; i < targetControls.length(); ++i) {
        if (controlsProgress[i]) {
            controlsDisplay += targetControls[i];
        } else {
            controlsDisplay += "_";
        }
    }
    sf::Text controlsProgressText;
    controlsProgressText.setFont(font);
    controlsProgressText.setString(controlsDisplay);
    controlsProgressText.setCharacterSize(progressFontSize);
    controlsProgressText.setFillColor(sf::Color::Magenta);
    controlsProgressText.setPosition(50, 430);
    window.draw(controlsProgressText);
    
    // Mostrar progreso visual de "INFERNO"
    std::string infernoDisplay = "INFERNO: ";
    for (size_t i = 0; i < targetInferno.length(); ++i) {
        if (infernoProgress[i]) {
            infernoDisplay += targetInferno[i];
        } else {
            infernoDisplay += "_";
        }
    }
    sf::Text infernoProgressText;
    infernoProgressText.setFont(font);
    infernoProgressText.setString(infernoDisplay);
    infernoProgressText.setCharacterSize(progressFontSize);
    infernoProgressText.setFillColor(sf::Color(255, 69, 0)); // Rojo-naranja intenso
    infernoProgressText.setPosition(50, 455);
    window.draw(infernoProgressText);
    
    // Mostrar control de volumen
    sf::Text volumeInfo;
    volumeInfo.setFont(font);
    volumeInfo.setCharacterSize(12);
    volumeInfo.setFillColor(sf::Color::Cyan);
    volumeInfo.setPosition(50, 530);
    
    if (volumeInputMode) {
        std::string volDisplay = "VOLUMEN: [" + volumeInput + "_] (00-99, Enter para aplicar)";
        volumeInfo.setString(volDisplay);
        volumeInfo.setFillColor(sf::Color::Yellow);
    } else {
        int currentVol = static_cast<int>(masterVolume * 99);
        std::string volDisplay = "VOL: " + std::to_string(currentVol) + "% (Presiona V para cambiar)";
        volumeInfo.setString(volDisplay);
    }
    window.draw(volumeInfo);
}

void Game::renderControls() {
    // ===============================================
    // CONSTANTES DE POSICIÓN - CONTROLES
    // ===============================================
    
    const float titleY = 80.f;
    const float controlsListStartY = 180.f;
    const float controlsLineSpacing = 40.f;
    const float promptY = 450.f;
    
    const int titleFontSize = 24;
    const int controlsFontSize = 18;
    const int promptFontSize = 16;
    
    // Título
    sf::Text title;
    title.setFont(font);
    title.setString("C:\\ARKANOID\\CONTROLS>");
    title.setCharacterSize(titleFontSize);
    title.setFillColor(sf::Color::Cyan);
    title.setPosition(50, titleY);
    window.draw(title);
    
    // Lista de controles
    std::vector<std::string> controls = {
        "PADDLE: Arrows (Left/Right)",
        "THROW: Space",
        "SPEED: S (with powerup)",
        "RETURN: ESC (in game)",
        "EXIT: ESC (in menu)"
    };
    
    for (size_t i = 0; i < controls.size(); ++i) {
        sf::Text controlText;
        controlText.setFont(font);
        controlText.setString(controls[i]);
        controlText.setCharacterSize(controlsFontSize);
        controlText.setFillColor(sf::Color::White);
        controlText.setPosition(70, controlsListStartY + i * controlsLineSpacing);
        window.draw(controlText);
    }
    
    // Prompt de retorno
    sf::Text returnPrompt;
    returnPrompt.setFont(font);
    returnPrompt.setString("Type 'return' to go back to menu");
    returnPrompt.setCharacterSize(promptFontSize);
    returnPrompt.setFillColor(sf::Color::Yellow);
    returnPrompt.setPosition(50, promptY);
    window.draw(returnPrompt);
    
    // Input del usuario
    sf::Text inputPrompt;
    inputPrompt.setFont(font);
    inputPrompt.setString("> ");
    inputPrompt.setCharacterSize(promptFontSize);
    inputPrompt.setFillColor(sf::Color::Green);
    inputPrompt.setPosition(50, promptY + 40);
    window.draw(inputPrompt);
    
    sf::Text userInput;
    userInput.setFont(font);
    userInput.setString(currentControlsInput);
    userInput.setCharacterSize(promptFontSize);
    userInput.setFillColor(sf::Color::White);
    userInput.setPosition(70, promptY + 40);
    window.draw(userInput);
    
    // Cursor parpadeante
    static sf::Clock cursorClock;
    if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
        cursorClock.restart();
    }
    if (cursorClock.getElapsedTime().asSeconds() < 0.25f) {
        sf::Text cursor;
        cursor.setFont(font);
        cursor.setString("_");
        cursor.setCharacterSize(promptFontSize);
        cursor.setFillColor(sf::Color::Green);
        cursor.setPosition(userInput.getPosition().x + userInput.getLocalBounds().width, promptY + 40);
        window.draw(cursor);
    }
}

void Game::resetGame() {
    // Reiniciar nivel
    currentLevel = 1;
    // Recrear nivel
    initLevel();
    
    // Limpiar PowerUps y bolas extra
    powerUps.clear();
    extraBalls.clear();
    activePowerUps.clear();  // Limpiar poder-ups activos
    
    // Resetear efectos caóticos del modo Inferno
    infernoShakeTimer = 0.0f;
    nextInfernoShakeTime = 5.0f + static_cast<float>(rand() % 26);
    infernoHitCount = 0;
    infernoBrightnessGlitch = 1.0f;
    glitchTimer = 0.0f;
    isGlitching = false;
    screenCracks.clear();
    
    // Seleccionar nueva música aleatoria para el nuevo juego
    std::cout << "Seleccionando música aleatoria..." << std::endl;
    
    // Elegir música según el modo de juego
    if (isInfernoMode && !infernoMusicTracks.empty()) {
        currentTrackIndex = std::rand() % infernoMusicTracks.size();
        currentBackgroundMusic = infernoMusicTracks[currentTrackIndex];
        std::cout << "Música Inferno seleccionada: índice " << currentTrackIndex << std::endl;
    } else if (!backgroundMusicTracks.empty()) {
        currentTrackIndex = std::rand() % backgroundMusicTracks.size();
        currentBackgroundMusic = backgroundMusicTracks[currentTrackIndex];
        std::cout << "Música normal seleccionada: índice " << currentTrackIndex << std::endl;
    } else {
        std::cout << "ERROR: No hay músicas cargadas" << std::endl;
        currentBackgroundMusic = nullptr;
    }
    
    // Resetear sistema de música de vida baja
    if (currentLowLifeMusic && currentLowLifeMusic->getStatus() == sf::Music::Playing) {
        currentLowLifeMusic->stop();
    }
    currentLowLifeMusic = nullptr;
    isPlayingLowLifeMusic = false;
    
    // Reiniciar paddle a tamaño y velocidad original
    sf::Vector2f originalSize(originalPaddleWidth, paddle.getSize().y);
    paddle.setSize(originalSize);
    paddle.setOrigin(originalSize.x / 2.f, originalSize.y / 2.f);
    paddle.speed = originalPaddleSpeed;
    paddle.setFillColor(sf::Color::White);  // Color normal
    float paddleY = static_cast<float>(window.getSize().y) - 40.f;
    paddle.setPosition(static_cast<float>(window.getSize().x) / 2.f, paddleY);
    
    // Resetear PowerUp de velocidad
    activePowerUps.clear();
    
    // Reiniciar vidas al límite
    lives = maxLives;
    
    // Reiniciar bola pegada al paddle
    ball.setPosition(paddle.getPosition().x, paddle.getPosition().y - paddle.getSize().y/2.f - ball.getRadius() - 2.f);
    ball.isStuck = true;
    ball.velocity = sf::Vector2f(0.f, 0.f);
}

void Game::loseLife() {
    lives--;
    if (lives <= 0) {
        // Game Over
        state = GameState::GameOver;
        
        // Detener toda la música
        if (currentBackgroundMusic) currentBackgroundMusic->stop();
        if (currentLowLifeMusic) currentLowLifeMusic->stop();
        isPlayingLowLifeMusic = false;
        
        gameOverSound.play();      // Reproducir sonido de game over
        showMatrixEffect = true;   // Activar efecto de cascada
        initMatrixEffect();        // Reinicializar columnas
        finalScoreText.setString("Puntuacion Final: " + std::to_string(score) + "\nPresiona ESPACIO para continuar");
        
        // Recentrar el texto de puntuación final
        sf::FloatRect scoreRect = finalScoreText.getLocalBounds();
        finalScoreText.setOrigin(scoreRect.left + scoreRect.width/2.0f, scoreRect.top + scoreRect.height/2.0f);
        finalScoreText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f + 50);
        
        // Limpiar bolas extra
        extraBalls.clear();
    } else {
        // Si queda 1 vida, cambiar a música de tensión
        if (lives == 1 && !isPlayingLowLifeMusic) {
            if (currentBackgroundMusic) currentBackgroundMusic->stop();
            
            // Seleccionar aleatoriamente entre lowlife1 y lowlife2
            currentLowLifeMusic = (std::rand() % 2 == 0) ? &lowLifeMusic1 : &lowLifeMusic2;
            currentLowLifeMusic->play();
            isPlayingLowLifeMusic = true;
        }
        
        // Reiniciar bola pegada al paddle
        ball.isStuck = true;
        ball.velocity = sf::Vector2f(0.f, 0.f);
        // Limpiar bolas extra al perder vida
        extraBalls.clear();
    }
}

void Game::renderGameOver() {
    // ===============================================
    // CONSTANTES DE POSICIÓN - GAME OVER
    // ===============================================
    
    // Posiciones de Game Over
    const float gameOverTitleY = 135.f;
    const float gameOverScoreY = 160.f;
    const float gameOverPromptY = 220.f;
    const float gameOverProgressY = 380.f;
    
    // Tamaños de fuente
    const int systemCrashFontSize = 24;
    const int crashScoreFontSize = 16;
    const int terminalFontSize = 16;
    const int progressFontSize = 14;
    
    // Dibujar título de vaalio madres
    sf::Text systemCrash;
    systemCrash.setFont(font);
    systemCrash.setString(">>> SYSTEM CRASHED <<<");
    systemCrash.setCharacterSize(systemCrashFontSize);
    systemCrash.setFillColor(sf::Color::Red);
    sf::FloatRect crashRect = systemCrash.getLocalBounds();
    systemCrash.setOrigin(crashRect.left + crashRect.width/2.0f, crashRect.top + crashRect.height/2.0f);
    systemCrash.setPosition(window.getSize().x / 2.0f, gameOverTitleY);
    window.draw(systemCrash);
    
    // Dibujar puntuación final
    sf::Text crashScore;
    crashScore.setFont(font);
    crashScore.setString("Puntuacion Final: " + std::to_string(score));
    crashScore.setCharacterSize(crashScoreFontSize);
    crashScore.setFillColor(sf::Color::Yellow);
    sf::FloatRect crashScoreRect = crashScore.getLocalBounds();
    crashScore.setOrigin(crashScoreRect.left + crashScoreRect.width/2.0f, crashScoreRect.top + crashScoreRect.height/2.0f);
    crashScore.setPosition(window.getSize().x / 2.0f, gameOverScoreY);
    window.draw(crashScore);
    
    // Dibujar menú tipo terminal para game over
    window.draw(gameOverPrompt);
    
    // Dibujar input actual del usuario
    sf::Text currentGameOverInputText;
    currentGameOverInputText.setFont(font);
    currentGameOverInputText.setString(currentGameOverInput);
    currentGameOverInputText.setCharacterSize(16);
    currentGameOverInputText.setFillColor(sf::Color::White);
    currentGameOverInputText.setPosition(gameOverPrompt.getPosition().x + gameOverPrompt.getLocalBounds().width, gameOverPromptY);
    window.draw(currentGameOverInputText);
    
    // Cursor parpadeante
    static sf::Clock gameOverCursorClock;
    if (gameOverCursorClock.getElapsedTime().asSeconds() > 0.5f) {
        gameOverCursorClock.restart();
    }
    if (gameOverCursorClock.getElapsedTime().asSeconds() < 0.25f) {
        sf::Text cursor;
        cursor.setFont(font);
        cursor.setString("_");
        cursor.setCharacterSize(16);
        cursor.setFillColor(sf::Color::Red);
        cursor.setPosition(currentGameOverInputText.getPosition().x + currentGameOverInputText.getLocalBounds().width, gameOverPromptY);
        window.draw(cursor);
    }
    
    // Dibujar comandos disponibles
    window.draw(rebootCommand);
    window.draw(gameOverExitCommand);
    
    // Mostrar progreso visual de "REBOOT"
    std::string rebootDisplay = "REBOOT: ";
    for (size_t i = 0; i < targetReboot.length(); ++i) {
        if (rebootProgress[i]) {
            rebootDisplay += targetReboot[i];
        } else {
            rebootDisplay += "_";
        }
    }
    sf::Text rebootProgressText;
    rebootProgressText.setFont(font);
    rebootProgressText.setString(rebootDisplay);
    rebootProgressText.setCharacterSize(progressFontSize);
    rebootProgressText.setFillColor(sf::Color::Green);
    rebootProgressText.setPosition(50, gameOverProgressY);
    window.draw(rebootProgressText);
    
    // Display de volumen en Game Over (abajo de las opciones)
    sf::Text volumeGameOver;
    volumeGameOver.setFont(font);
    volumeGameOver.setCharacterSize(12);
    volumeGameOver.setPosition(50, gameOverProgressY + 60);
    
    if (volumeInputMode) {
        std::string volDisplay = "VOLUMEN: [" + volumeInput + "_] (00-99, Enter para aplicar)";
        volumeGameOver.setString(volDisplay);
        volumeGameOver.setFillColor(sf::Color::Yellow);
    } else {
        int currentVol = static_cast<int>(masterVolume * 99);
        std::string volDisplay = "VOL: " + std::to_string(currentVol) + "% (Presiona V para cambiar)";
        volumeGameOver.setString(volDisplay);
        volumeGameOver.setFillColor(sf::Color::Cyan);
    }
    window.draw(volumeGameOver);
    
    // Mostrar progreso visual de "EXIT"
    std::string gameOverExitDisplay = "EXIT: ";
    for (size_t i = 0; i < targetExit.length(); ++i) {
        if (gameOverExitProgress[i]) {
            gameOverExitDisplay += targetExit[i];
        } else {
            gameOverExitDisplay += "_";
        }
    }
    sf::Text gameOverExitProgressText;
    gameOverExitProgressText.setFont(font);
    gameOverExitProgressText.setString(gameOverExitDisplay);
    gameOverExitProgressText.setCharacterSize(progressFontSize);
    gameOverExitProgressText.setFillColor(sf::Color::Red);
    gameOverExitProgressText.setPosition(50, 400);
    window.draw(gameOverExitProgressText);
}

// Actualizar volumen maestro
void Game::updateMasterVolume() {
    // Aplicar volumen maestro a todos los componentes de audio
    bounceSound.setVolume(baseVolumeEffects * masterVolume);
    
    // Actualizar volumen de todas las músicas de fondo
    for (auto* track : backgroundMusicTracks) {
        track->setVolume(baseVolumeMusic * masterVolume);
    }
    
    // Actualizar volumen de todas las músicas de Inferno
    for (auto* track : infernoMusicTracks) {
        track->setVolume(baseVolumeMusic * masterVolume);
    }
    
    // Actualizar músicas de vida baja
    lowLifeMusic1.setVolume(baseVolumeMusic * masterVolume);
    lowLifeMusic2.setVolume(baseVolumeMusic * masterVolume);
    
    menuMusic.setVolume(baseVolumeMenu * masterVolume);
    gameOverSound.setVolume(baseVolumeGameOver * masterVolume);
}

// Procesar input de volumen
void Game::processVolumeInput(char c) {
    // Solo procesar números 0-9
    if (c >= '0' && c <= '9') {
        if (volumeInput.length() < 2) {
            volumeInput += c;
        }
    }
    // Enter y ESC se manejan en KeyPressed, no aquí
}

// Inicializar efecto Matrix
void Game::initMatrixEffect() {
    // ===============================================
    // CONFIGURACIÓN DEL EFECTO MATRIX 
    // ===============================================
    
    // Configuración de densidad y apariencia
    const int columnSpacing = 35;           // Mayor separación entre columnas (era 20)
    const int minLength = 5;               // Menos caracteres mínimo (era 10)
    const int maxLength = 12;              // Menos caracteres máximo (era 30)
    const float minSpeed = 30.0f;          // Velocidad más lenta
    const float maxSpeed = 80.0f;          // Velocidad máxima más lenta (era 150)
    
    matrixColumns.clear();
    int numColumns = window.getSize().x / columnSpacing; // Menos columnas
    
    for (int i = 0; i < numColumns; ++i) {
        MatrixColumn column;
        column.x = static_cast<float>(i * columnSpacing);
        column.y = static_cast<float>(std::rand() % window.getSize().y);
        column.speed = minSpeed + (std::rand() % static_cast<int>(maxSpeed - minSpeed));
        
        // Crear cadena más corta de caracteres aleatorios
        column.letters = "";
        int length = minLength + (std::rand() % (maxLength - minLength)); // Cadenas más cortas
        for (int j = 0; j < length; ++j) {
            char c;
            int type = std::rand() % 3;
            if (type == 0) c = '0' + (std::rand() % 10);      // Números 0-9
            else if (type == 1) c = 'A' + (std::rand() % 26); // Letras A-Z
            else c = 'a' + (std::rand() % 26);                // Letras a-z
            column.letters += c;
        }
        
        // Color verde Matrix con variación
        int greenIntensity = 100 + (std::rand() % 156); // Verde entre 100-255
        column.color = sf::Color(0, greenIntensity, 0);
        
        matrixColumns.push_back(column);
    }
}

// Actualizar efecto Matrix
void Game::updateMatrixEffect() {
    float dt = matrixClock.restart().asSeconds();
    
    for (auto& column : matrixColumns) {
        column.y += column.speed * dt;
        
        // Si la columna sale de la pantalla, reiniciarla arriba
        if (column.y > window.getSize().y + 100) {
            column.y = -static_cast<float>(column.letters.length() * 15);
            column.speed = 50.0f + (std::rand() % 100);
            
            // Cambiar color ocasionalmente
            if (std::rand() % 10 == 0) {
                int greenIntensity = 100 + (std::rand() % 156);
                column.color = sf::Color(0, greenIntensity, 0);
            }
        }
    }
}

// Renderizar efecto Matrix
void Game::renderMatrixEffect() {
    // ===============================================
    // CONSTANTES DEL EFECTO MATRIX - MODIFICAR AQUÍ
    // ===============================================
    
    // Configuración de apariencia
    const int matrixFontSize = 12;           // Fuente más pequeña (era 14)
    const float letterSpacing = 20.f;       // Más separación entre letras (era 15)
    const float baseAlpha = 0.4f;          // Transparencia base (40% opaco)
    const float fadeMultiplier = 0.6f;     // Factor de desvanecimiento más suave
    
    for (const auto& column : matrixColumns) {
        sf::Text matrixText;
        matrixText.setFont(font);
        matrixText.setCharacterSize(matrixFontSize);
        matrixText.setFillColor(column.color);
        
        // Dibujar cada letra de la columna (con mayor espaciado)
        for (size_t i = 0; i < column.letters.length(); ++i) {
            matrixText.setString(column.letters[i]);
            float letterY = column.y - (i * letterSpacing); // Espaciado aumentado
            matrixText.setPosition(column.x, letterY);
            
            // Solo dibujar si está dentro de la pantalla
            if (letterY >= -25 && letterY <= window.getSize().y) {
                // Efecto de desvanecimiento más suave y transparente
                float fadeRatio = i / static_cast<float>(column.letters.length());
                sf::Uint8 alpha = static_cast<sf::Uint8>(255 * baseAlpha * (1.0f - (fadeRatio * fadeMultiplier)));
                sf::Color fadeColor = column.color;
                fadeColor.a = alpha;
                matrixText.setFillColor(fadeColor);
                
                window.draw(matrixText);
            }
        }
    }
}

// Procesar entrada del menú terminal
void Game::processTerminalInput(char c) {
    currentInput += c;
    
    // Verificar si la letra pertenece a "play"
    for (size_t i = 0; i < targetPlay.length(); ++i) {
        if (targetPlay[i] == c && !playProgress[i]) {
            playProgress[i] = true;
            break;
        }
    }
    
    // Verificar si la letra pertenece a "exit"
    for (size_t i = 0; i < targetExit.length(); ++i) {
        if (targetExit[i] == c && !exitProgress[i]) {
            exitProgress[i] = true;
            break;
        }
    }
    
    // Verificar si la letra pertenece a "controls"
    for (size_t i = 0; i < targetControls.length(); ++i) {
        if (targetControls[i] == c && !controlsProgress[i]) {
            controlsProgress[i] = true;
            break;
        }
    }
    
    // Verificar si la letra pertenece a "inferno"
    for (size_t i = 0; i < targetInferno.length(); ++i) {
        if (targetInferno[i] == c && !infernoProgress[i]) {
            infernoProgress[i] = true;
            break;
        }
    }
    
    // Verificar si "play" está completo
    bool playComplete = true;
    for (bool progress : playProgress) {
        if (!progress) {
            playComplete = false;
            break;
        }
    }
    
    // Verificar si "exit" está completo
    bool exitComplete = true;
    for (bool progress : exitProgress) {
        if (!progress) {
            exitComplete = false;
            break;
        }
    }
    
    // Verificar si "controls" está completo
    bool controlsComplete = true;
    for (bool progress : controlsProgress) {
        if (!progress) {
            controlsComplete = false;
            break;
        }
    }
    
    // Verificar si "inferno" está completo
    bool infernoComplete = true;
    for (bool progress : infernoProgress) {
        if (!progress) {
            infernoComplete = false;
            break;
        }
    }
    
    // Ejecutar comando si está completo
    if (playComplete) {
        // Reiniciar progreso para próxima vez
        playProgress = {false, false, false, false};
        exitProgress = {false, false, false, false};
        controlsProgress = {false, false, false, false, false, false, false, false};
        infernoProgress = {false, false, false, false, false, false, false};
        currentInput = "";
        
        isInfernoMode = false; // Modo normal
        // Iniciar juego
        state = GameState::Playing;
        menuMusic.stop();
        resetGame();
        
        // Reproducir música aleatoria
        std::cout << "Intentando reproducir música..." << std::endl;
        if (currentBackgroundMusic) {
            std::cout << "Reproduciendo música de fondo" << std::endl;
            currentBackgroundMusic->play();
        } else {
            std::cout << "ERROR: No hay música cargada!" << std::endl;
        }
    }
    else if (exitComplete) {
        // Salir del juego
        menuMusic.stop();
        if (currentBackgroundMusic) currentBackgroundMusic->stop();
        if (currentLowLifeMusic) currentLowLifeMusic->stop();
        window.close();
    }
    else if (controlsComplete) {
        // Ir a pantalla de controles
        playProgress = {false, false, false, false};
        exitProgress = {false, false, false, false};
        controlsProgress = {false, false, false, false, false, false, false, false};
        infernoProgress = {false, false, false, false, false, false, false};
        currentInput = "";
        state = GameState::Controls;
    }
    else if (infernoComplete) {
        // Iniciar juego en modo INFERNO
        playProgress = {false, false, false, false};
        exitProgress = {false, false, false, false};
        controlsProgress = {false, false, false, false, false, false, false, false};
        infernoProgress = {false, false, false, false, false, false, false};
        currentInput = "";
        
        isInfernoMode = true; // Activar modo Inferno
        state = GameState::Playing;
        menuMusic.stop();
        resetGame();
        
        // Seleccionar música aleatoria del modo Inferno
        std::cout << "MODO INFERNO ACTIVADO!" << std::endl;
        if (!infernoMusicTracks.empty()) {
            currentTrackIndex = std::rand() % infernoMusicTracks.size();
            currentBackgroundMusic = infernoMusicTracks[currentTrackIndex];
            std::cout << "Reproduciendo música Inferno " << currentTrackIndex + 1 << std::endl;
            currentBackgroundMusic->play();
        } else if (currentBackgroundMusic) {
            std::cout << "Reproduciendo música de fondo (no hay músicas Inferno)" << std::endl;
            currentBackgroundMusic->play();
        } else {
            std::cout << "ERROR: No hay música cargada!" << std::endl;
        }
    }
}

// Actualizar display del terminal
void Game::updateTerminalDisplay() {
    // Esta función se puede usar para efectos adicionales si es necesario
    // Por ahora el renderizado se maneja directamente en renderMenu()
}

// Procesar entrada del menú de controles
void Game::processControlsInput(char c) {
    currentControlsInput += c;
    
    // Verificar si la letra pertenece a "return"
    std::string targetReturn = "return";
    for (size_t i = 0; i < targetReturn.length(); ++i) {
        if (targetReturn[i] == c && !returnProgress[i]) {
            returnProgress[i] = true;
            break;
        }
    }
    
    // Verificar si "return" está completo
    bool returnComplete = true;
    for (bool progress : returnProgress) {
        if (!progress) {
            returnComplete = false;
            break;
        }
    }
    
    // Ejecutar comando si está completo
    if (returnComplete) {
        // Reiniciar progreso para próxima vez
        returnProgress = {false, false, false, false, false, false};
        currentControlsInput = "";
        
        // Volver al menú principal
        state = GameState::Menu;
    }
}

// Procesar entrada del menú de game over
void Game::processGameOverInput(char c) {
    currentGameOverInput += c;
    
    // Verificar si la letra pertenece a "reboot"
    for (size_t i = 0; i < targetReboot.length(); ++i) {
        if (targetReboot[i] == c && !rebootProgress[i]) {
            rebootProgress[i] = true;
            break;
        }
    }
    
    // Verificar si la letra pertenece a "exit"
    for (size_t i = 0; i < targetExit.length(); ++i) {
        if (targetExit[i] == c && !gameOverExitProgress[i]) {
            gameOverExitProgress[i] = true;
            break;
        }
    }
    
    // Verificar si "reboot" está completo
    bool rebootComplete = true;
    for (bool progress : rebootProgress) {
        if (!progress) {
            rebootComplete = false;
            break;
        }
    }
    
    // Verificar si "exit" está completo
    bool gameOverExitComplete = true;
    for (bool progress : gameOverExitProgress) {
        if (!progress) {
            gameOverExitComplete = false;
            break;
        }
    }
    
    // Ejecutar comando si está completo
    if (rebootComplete) {
        // Reiniciar progreso para próxima vez
        rebootProgress = {false, false, false, false, false, false};
        gameOverExitProgress = {false, false, false, false};
        currentGameOverInput = "";
        
        // Reiniciar juego (volver a jugar)
        state = GameState::Playing;
        lives = 3;
        score = 0;
        showMatrixEffect = false;
        resetGame();
        
        // Reproducir música aleatoria
        std::cout << "Intentando reproducir música..." << std::endl;
        if (currentBackgroundMusic) {
            std::cout << "Reproduciendo música de fondo" << std::endl;
            currentBackgroundMusic->play();
        } else {
            std::cout << "ERROR: No hay música cargada!" << std::endl;
        }
    }
    else if (gameOverExitComplete) {
        // Salir del juego
        menuMusic.stop();
        if (currentBackgroundMusic) currentBackgroundMusic->stop();
        if (currentLowLifeMusic) currentLowLifeMusic->stop();
        window.close();
    }
}

// Crear partículas cuando se rompe un ladrillo
void Game::createBrickParticles(sf::Vector2f position, sf::Color color) {
    // MODO INFERNO: Más partículas y más agresivas
    int numParticles = isInfernoMode ? 18 : 8;  // Más del doble en Inferno
    
    for (int i = 0; i < numParticles; i++) {
        Particle p;
        
        // MODO INFERNO: Partículas más grandes y con tinte rojo
        if (isInfernoMode) {
            p.shape.setSize(sf::Vector2f(5.f, 5.f));  // Más grandes
            // Agregar tinte rojo sangriento a las partículas
            sf::Uint8 r = std::min(255, static_cast<int>(color.r) + 80);
            sf::Uint8 g = static_cast<sf::Uint8>(color.g * 0.4f);  // Reducir verde
            sf::Uint8 b = static_cast<sf::Uint8>(color.b * 0.3f);  // Reducir azul
            p.shape.setFillColor(sf::Color(r, g, b));
        } else {
            p.shape.setSize(sf::Vector2f(3.f, 3.f));  // Normales
            p.shape.setFillColor(color);
        }
        
        p.shape.setPosition(position);
        
        // Velocidad aleatoria en todas direcciones
        float angle = (std::rand() % 360) * 3.14159f / 180.f;
        
        // MODO INFERNO: Partículas mucho más rápidas y explosivas
        float speed = isInfernoMode ? (120.f + (std::rand() % 180)) : (50.f + (std::rand() % 100));
        p.velocity.x = std::cos(angle) * speed;
        p.velocity.y = std::sin(angle) * speed;
        
        // MODO INFERNO: Partículas duran más tiempo para efecto dramático
        p.lifetime = isInfernoMode ? (0.6f + (std::rand() % 400) / 1000.f) : (0.4f + (std::rand() % 300) / 1000.f);
        p.maxLifetime = p.lifetime;
        
        particles.push_back(p);
    }
}

// Actualizar partículas
void Game::updateParticles(float dt) {
    for (auto& particle : particles) {
        particle.lifetime -= dt;
        particle.shape.move(particle.velocity * dt);
        
        // Fade out progresivo
        float alpha = (particle.lifetime / particle.maxLifetime) * 255.f;
        sf::Color color = particle.shape.getFillColor();
        color.a = static_cast<sf::Uint8>(alpha);
        particle.shape.setFillColor(color);
        
        // Gravedad sutil
        particle.velocity.y += 200.f * dt;
    }
    
    // Eliminar partículas muertas
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const Particle& p) { return p.lifetime <= 0.f; }),
        particles.end()
    );
}

// Actualizar destello de la pelota según velocidad
void Game::updateBallGlow() {
    // Calcular velocidad de la pelota
    float speed = std::sqrt(ball.velocity.x * ball.velocity.x + ball.velocity.y * ball.velocity.y);
    float baseSpeed = 400.f;  // Velocidad base de referencia
    
    // Intensidad del destello según velocidad (0-1)
    if (speed > baseSpeed) {
        ballGlowIntensity = std::min((speed - baseSpeed) / 300.f, 1.f);  // Escala hasta 700 de velocidad
    } else {
        ballGlowIntensity = 0.f;
    }
    
    // Actualizar el destello visual
    ballGlow.setPosition(ball.getPosition());
    
    // Color blanco con alpha según intensidad
    sf::Uint8 alpha = static_cast<sf::Uint8>(ballGlowIntensity * 180.f);
    ballGlow.setFillColor(sf::Color(255, 255, 255, alpha));
}

// Actualizar estelas tipo Tron de la pelota
void Game::updateBallTrails(float dt) {
    // Calcular velocidad de la pelota
    float speed = std::sqrt(ball.velocity.x * ball.velocity.x + ball.velocity.y * ball.velocity.y);
    float speedThreshold = 500.f;  // Velocidad mínima para que aparezcan estelas (después de 3+ golpes)
    
    // Solo crear estelas si la pelota va rápido
    if (speed > speedThreshold && !ball.isStuck) {
        trailSpawnTimer += dt;
        
        // Crear una nueva estela cada 0.03 segundos (sutil)
        if (trailSpawnTimer >= 0.03f) {
            trailSpawnTimer = 0.f;
            
            BallTrail trail;
            trail.shape.setRadius(ball.getRadius() * 0.8f);  // Ligeramente más pequeña
            trail.shape.setOrigin(trail.shape.getRadius(), trail.shape.getRadius());
            trail.shape.setPosition(ball.getPosition());
            trail.lifetime = 0.15f;  // Dura muy poco (efecto sutil)
            
            // Color celeste/cyan con transparencia
            trail.shape.setFillColor(sf::Color(100, 200, 255, 120));
            
            ballTrails.push_back(trail);
        }
    }
    
    // Actualizar estelas existentes
    for (auto& trail : ballTrails) {
        trail.lifetime -= dt;
        
        // Fade out progresivo
        float alpha = (trail.lifetime / 0.15f) * 120.f;
        sf::Color color = trail.shape.getFillColor();
        color.a = static_cast<sf::Uint8>(alpha);
        trail.shape.setFillColor(color);
    }
    
    // Eliminar estelas muertas
    ballTrails.erase(
        std::remove_if(ballTrails.begin(), ballTrails.end(),
            [](const BallTrail& t) { return t.lifetime <= 0.f; }),
        ballTrails.end()
    );
}

// Renderizar HUD de power-ups activos con contador
void Game::renderActivePowerUpsHUD() {
    if (activePowerUps.empty()) return;
    
    const float hudY = 65.f;
    const float startX = window.getSize().x - 250.f;  // Posición inicial (derecha)
    const float iconSize = 40.f;
    const float spacing = 50.f;
    
    for (size_t i = 0; i < activePowerUps.size(); ++i) {
        const ActivePowerUp& pup = activePowerUps[i];
        float xPos = startX - (i * spacing);  // Posicionar de derecha a izquierda
        
        // Calcular tiempo restante
        float timeRemaining = pup.duration - pup.elapsedTime;
        if (timeRemaining < 0.f) timeRemaining = 0.f;
        
        // Calcular porcentaje (0 = expirado, 1 = inicio)
        float progress = timeRemaining / pup.duration;
        
        // Obtener textura e icono según tipo
        sf::Sprite powerUpIcon;
        std::string timeText;
        
        switch (pup.type) {
            case PowerUpType::SpeedPaddle:
                powerUpIcon.setTexture(powerUpSpeedTexture);
                break;
            case PowerUpType::ExpandPaddle:
                powerUpIcon.setTexture(powerUpExpandTexture);
                break;
            case PowerUpType::ShrinkPaddle:
                powerUpIcon.setTexture(powerUpShrinkTexture);
                break;
            case PowerUpType::SlowPaddle:
                powerUpIcon.setTexture(powerUpSlowTexture);
                break;
            case PowerUpType::SlowMotion:
                powerUpIcon.setTexture(powerUpSlowMotionTexture);
                break;
            default:
                continue;  // No renderizar otros tipos
        }
        
        // Configurar icono
        float scale = iconSize / std::max(powerUpIcon.getTexture()->getSize().x, 
                                         powerUpIcon.getTexture()->getSize().y);
        powerUpIcon.setScale(scale, scale);
        powerUpIcon.setPosition(xPos, hudY);
        
        // Efecto de fade-out: la transparencia disminuye conforme se acaba el tiempo
        sf::Uint8 alpha = static_cast<sf::Uint8>(progress * 255.f);
        sf::Color iconColor(255, 255, 255, alpha);
        powerUpIcon.setColor(iconColor);
        
        window.draw(powerUpIcon);
        
        // Dibujar contador de tiempo encima del icono
        sf::Text timerText;
        timerText.setFont(font);
        timerText.setCharacterSize(14);
        timerText.setString(std::to_string(static_cast<int>(timeRemaining)));
        timerText.setPosition(xPos + 12.f, hudY - 5.f);
        
        // Color del texto según tiempo restante
        if (timeRemaining < 2.f) {
            timerText.setFillColor(sf::Color::Red);  // Rojo cuando está por expirar
        } else {
            timerText.setFillColor(sf::Color::White);
        }
        
        window.draw(timerText);
        
        // Dibujar barra de progreso debajo
        sf::RectangleShape progressBar(sf::Vector2f(40.f, 3.f));
        progressBar.setPosition(xPos, hudY + 45.f);
        progressBar.setFillColor(sf::Color(50, 50, 50, 150));  // Fondo gris
        window.draw(progressBar);
        
        // Barra de progreso coloreada
        sf::RectangleShape progressBarFill(sf::Vector2f(40.f * progress, 3.f));
        progressBarFill.setPosition(xPos, hudY + 45.f);
        
        // Color según tipo de poder
        if (pup.type == PowerUpType::SpeedPaddle) {
            progressBarFill.setFillColor(sf::Color(173, 216, 230, 200));  // Azul cielo
        } else if (pup.type == PowerUpType::ExpandPaddle) {
            progressBarFill.setFillColor(sf::Color(144, 238, 144, 200));  // Verde claro
        }
        
        window.draw(progressBarFill);
    }
}

void Game::updateInfernoEffects(float dt) {
    if (!isInfernoMode) return;
    
    // ====== SACUDIDAS ALEATORIAS VIOLENTAS ======
    infernoShakeTimer += dt;
    if (infernoShakeTimer >= nextInfernoShakeTime) {
        // Activar sacudida violenta (reducida para no marear)
        screenShakeIntensity = 6.0f;   // Intensidad moderada
        screenShakeDuration = 0.25f;   // Duración más corta
        
        // Resetear timer y generar nuevo tiempo aleatorio (5-30 segundos)
        infernoShakeTimer = 0.0f;
        nextInfernoShakeTime = 5.0f + static_cast<float>(rand() % 26);
    }
    
    // ====== EFECTO DE GLITCH DE BRILLO ======
    if (isGlitching) {
        glitchTimer -= dt;
        if (glitchTimer <= 0.0f) {
            isGlitching = false;
            infernoBrightnessGlitch = 1.0f;  // Restaurar brillo
        }
    } else {
        // Probabilidad aleatoria de iniciar un glitch (cada frame tiene pequeña chance)
        if (rand() % 500 == 0) {  // ~0.2% chance por frame
            isGlitching = true;
            glitchTimer = 0.05f + static_cast<float>(rand() % 20) / 100.0f;  // 0.05-0.25 segundos
            infernoBrightnessGlitch = 0.2f + static_cast<float>(rand() % 50) / 100.0f;  // 20%-70% brillo
        }
    }
}

void Game::renderInfernoEffects() {
    if (!isInfernoMode) return;
    
    // ====== EFECTO DE BRILLO FALLANDO (OSCURECIMIENTO) ======
    if (isGlitching && infernoBrightnessGlitch < 1.0f) {
        // Crear overlay oscuro para simular fallo de monitor
        sf::RectangleShape darknessOverlay(sf::Vector2f(
            static_cast<float>(window.getSize().x),
            static_cast<float>(window.getSize().y)
        ));
        darknessOverlay.setPosition(0, 0);
        
        // El alpha depende de cuánto se reduce el brillo
        int alpha = static_cast<int>((1.0f - infernoBrightnessGlitch) * 200);
        darknessOverlay.setFillColor(sf::Color(0, 0, 0, alpha));
        window.draw(darknessOverlay);
        
        // Añadir líneas de escaneo horizontal para efecto CRT
        if (rand() % 3 == 0) {  // Parpadeo aleatorio
            for (int i = 0; i < 5; i++) {
                sf::RectangleShape scanLine(sf::Vector2f(
                    static_cast<float>(window.getSize().x),
                    2.0f
                ));
                scanLine.setPosition(0, static_cast<float>(rand() % window.getSize().y));
                scanLine.setFillColor(sf::Color(255, 255, 255, 30 + rand() % 40));
                window.draw(scanLine);
            }
        }
    }
    
    // ====== GRIETAS EN PANTALLA (ESTILO TELARAÑA) ======
    for (const auto& crack : screenCracks) {
        window.draw(crack);
    }
}

void Game::addScreenCrack() {
    if (!isInfernoMode) return;
    
    // Añadir grieta cada cierta cantidad de golpes
    if (infernoHitCount % 8 == 0 && infernoHitCount > 0) {  // Cada 8 golpes
        // Crear una grieta en posición aleatoria
        float startX = static_cast<float>(rand() % window.getSize().x);
        float startY = static_cast<float>(rand() % window.getSize().y);
        
        // Crear varias líneas finas como telaraña
        int numSegments = 5 + rand() % 6;  // 5-10 segmentos
        float currentX = startX;
        float currentY = startY;
        
        for (int i = 0; i < numSegments; i++) {
            // Longitud y ángulo aleatorio para cada segmento
            float length = 20.0f + static_cast<float>(rand() % 50);
            float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;
            
            // Línea muy fina y transparente (telaraña)
            sf::RectangleShape crackLine(sf::Vector2f(length, 1.0f));
            crackLine.setPosition(currentX, currentY);
            crackLine.setRotation(angle * 180.0f / 3.14159f);
            
            // Color blanco muy transparente
            int alpha = 40 + rand() % 40;  // 40-80 alpha (muy transparente)
            crackLine.setFillColor(sf::Color(255, 255, 255, alpha));
            
            screenCracks.push_back(crackLine);
            
            // Añadir sub-ramas (50% probabilidad para más efecto telaraña)
            if (rand() % 100 < 50) {
                float branchLength = 10.0f + static_cast<float>(rand() % 25);
                float branchAngle = angle + (rand() % 120 - 60) * 3.14159f / 180.0f;
                
                sf::RectangleShape branchLine(sf::Vector2f(branchLength, 1.0f));
                branchLine.setPosition(currentX + length * 0.5f * cos(angle), 
                                       currentY + length * 0.5f * sin(angle));
                branchLine.setRotation(branchAngle * 180.0f / 3.14159f);
                branchLine.setFillColor(sf::Color(255, 255, 255, 30 + rand() % 30));
                
                screenCracks.push_back(branchLine);
            }
            
            // Siguiente segmento empieza donde termina este
            currentX += length * cos(angle);
            currentY += length * sin(angle);
        }
        
        // Limitar cantidad máxima de grietas (para rendimiento)
        if (screenCracks.size() > 200) {
            screenCracks.erase(screenCracks.begin(), screenCracks.begin() + 40);
        }
    }
}
