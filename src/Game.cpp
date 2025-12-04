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
    
    // Dimensiones de la grilla
    const int columns = 16;                  // Número de columnas de bloques
    const int rows = 8;                      // Número de filas de bloques
    
    // Tamaño de cada celda
    const float cellWidth = 60.f;           // Ancho de cada celda en píxeles
    const float cellHeight = 30.f;          // Alto de cada celda en píxeles
    
    // Posicionamiento
    const float startX = 8.0f;              // Distancia desde el borde izquierdo
    const float startY = 200.f;             // Distancia desde la parte superior
    
    // Probabilidades de bloques especiales
    const int smallBlockChance = 15;        // % probabilidad de bloques pequeños (0-100)
    const int purpleBlockChance = 10;       // % probabilidad de bloques morados 3 golpes (0-100)  
    const int redBlockChance = 25;          // % probabilidad de bloques rojos 2 golpes (0-100)
    
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
            
            if (isSmallBlock) {
                brickColor = sf::Color(255, 255, 0);  // Amarillo para bloques pequeños
                pointValue = smallBlockPoints;  // Puntos de bloques pequeños
            } else {
                int random = std::rand() % 100;
                
                if (random < purpleBlockChance) {  // % de probabilidad - BLOQUES MORADOS (3 golpes)
                    hits = 3;
                    brickColor = sf::Color(150, 0, 200);  // Morado intenso
                    pointValue = purpleBlockPoints;  // Puntos por bloque morado
                }
                else if (random < redBlockChance) {  // % de probabilidad - BLOQUES ROJOS (2 golpes)
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
    
    if (!backgroundMusic.openFromFile("assets/music/background.mp3")) {
        std::cout << "No se pudo cargar background.mp3" << std::endl;
    } else {
        backgroundMusic.setLoop(true);
        baseVolumeMusic = 30.0f;
        backgroundMusic.setVolume(baseVolumeMusic * masterVolume); 
    }
    
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
    
 
    // Para que no se vea borroso si es pixel art
    blockTexture.setSmooth(true);
    
    // Inicializar volumen maestro
    volumeInput = "50"; // Volumen inicial al 50%
    updateMasterVolume();
    initLevel(); 

    // Inicializar paddle en la parte inferior
    float paddleY = static_cast<float>(window.getSize().y) - 40.f;
    paddle.setPosition(static_cast<float>(window.getSize().x) / 2.f, paddleY);

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
    playProgress = {false, false, false, false}; // p, l, a, y
    exitProgress = {false, false, false, false}; // e, x, i, t
    
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
    playCommand.setString("PLAY - Iniciar juego");
    playCommand.setCharacterSize(commandFontSize);
    playCommand.setFillColor(sf::Color::White);
    playCommand.setPosition(50, commandListY);
    
    exitCommand.setFont(font);
    exitCommand.setString("EXIT - Salir del programa");
    exitCommand.setCharacterSize(commandFontSize);
    exitCommand.setFillColor(sf::Color::White);
    exitCommand.setPosition(50, commandListY + 30);
    
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
                        backgroundMusic.stop();
                        window.close();
                    }
                }
                // Tecla V para activar modo volumen
                else if (event.key.code == sf::Keyboard::V && !volumeInputMode) {
                    volumeInputMode = true;
                    volumeInput.clear();
                    std::cout << "MODO VOLUMEN: Ingresa 00-99 y presiona Enter (ESC para cancelar)" << std::endl;
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
                        backgroundMusic.stop();
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
                // Backspace para borrar
                else if (event.key.code == sf::Keyboard::BackSpace) {
                    if (volumeInputMode && !volumeInput.empty()) {
                        volumeInput.pop_back();
                    } else if (!currentGameOverInput.empty()) {
                        currentGameOverInput.pop_back();
                    }
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
                        backgroundMusic.stop();
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
    
    // Actualizar efecto Matrix si está activo
    if (showMatrixEffect && state == GameState::GameOver) {
        updateMatrixEffect();
    }
    
    if (state != GameState::Playing) return;

    // --- Paddle: mover según teclado ---
    sf::Vector2f paddlePos = paddle.getPosition();
    float halfWidth = paddle.getSize().x / 2.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        paddle.move(-paddle.speed * dt, 0.f);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        paddle.move(paddle.speed * dt, 0.f);
    }
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
    }

    // Colisión con paredes (izquierda/derecha)
    float r = ball.getRadius();
    sf::Vector2f pos = ball.getPosition();
    if (pos.x - r < 0.f) {
        ball.setPosition(r, pos.y);
        ball.velocity.x = std::abs(ball.velocity.x);
        bounceSound.play();  // Sonido de rebote
    }
    if (pos.x + r > window.getSize().x) {
        ball.setPosition(static_cast<float>(window.getSize().x) - r, pos.y);
        ball.velocity.x = -std::abs(ball.velocity.x);
        bounceSound.play();  // Sonido de rebote
    }
    // Colisión con techo
    if (pos.y - r < 0.f) {
        ball.setPosition(pos.x, r);
        ball.velocity.y = std::abs(ball.velocity.y);
        bounceSound.play();  // Sonido de rebote
    }
    // Si la bola cae abajo, perder vida
    if (pos.y - r > window.getSize().y) {
        loseLife();
    }

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

    // Colisión con ladrillos
    for (auto& brick : bricks) {
        if (brick.isDestroyed) continue;
        if (ball.getGlobalBounds().intersects(brick.getGlobalBounds())) {
            // Reducir golpes restantes
            brick.hitsRemaining--;
            
            // Si ya no quedan golpes, destruir el bloque
            if (brick.hitsRemaining <= 0) {
                brick.isDestroyed = true;
                score += brick.pointValue;
                
                // Efecto especial para bloques pequeños: aumentar velocidad 5%
                if (brick.isSmallBlock) {
                    ball.velocity *= 1.05f;
                }
            } else {
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
    
    // Verificar si todos los bloques están destruidos
    bool allDestroyed = true;
    for (const auto& brick : bricks) {
        if (!brick.isDestroyed) {
            allDestroyed = false;
            break;
        }
    }
    
    // Si todos los bloques están destruidos, avanzar al siguiente nivel
    if (allDestroyed && !bricks.empty()) {
        currentLevel++;  // Incrementar nivel
        initLevel();     // Recrear todos los bloques
        // Reiniciar bola pegada al paddle
        ball.isStuck = true;
        ball.velocity = sf::Vector2f(0.f, 0.f);
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

        // Dibujar paddle y bola
        window.draw(paddle);
        window.draw(ball);
        
        // Actualizar y dibujar HUD (parte superior centro, horizontal)
        livesText.setString("Vidas: " + std::to_string(lives));
        scoreText.setString("Puntos: " + std::to_string(score));
        
        // Posicionar horizontalmente en la parte superior centro
        sf::FloatRect livesRect = livesText.getLocalBounds();
        sf::FloatRect scoreRect = scoreText.getLocalBounds();
        
        // Calcular ancho total de ambos textos con separación
        float totalWidth = livesRect.width + scoreRect.width + hudSeparation;
        float startX = (window.getSize().x - totalWidth) / 2.0f; // Centrar horizontalmente
        
        livesText.setPosition(startX, hudY); // Vidas a la izquierda
        scoreText.setPosition(startX + livesRect.width + hudSeparation, hudY); // Puntos a la derecha
        
        window.draw(livesText);
        window.draw(scoreText);
        
        // Display de volumen a la derecha del score
        sf::Text volumeHUD;
        volumeHUD.setFont(font);
        volumeHUD.setCharacterSize(14); // Mismo tamaño que el HUD
        
        if (volumeInputMode) {
            std::string volDisplay = "VOL:[" + volumeInput + "_]";
            volumeHUD.setString(volDisplay);
            volumeHUD.setFillColor(sf::Color::Blue); // Azul cuando es modificable
        } else {
            int currentVol = static_cast<int>(masterVolume * 99);
            std::string volDisplay = "VOL:" + std::to_string(currentVol) + "%";
            volumeHUD.setString(volDisplay);
            volumeHUD.setFillColor(sf::Color::Yellow); // Amarillo fijo
        }
        
        sf::FloatRect scoreRect2 = scoreText.getLocalBounds();
        volumeHUD.setPosition(startX + livesRect.width + hudSeparation + scoreRect2.width + 20, hudY);
        window.draw(volumeHUD);
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
    playProgressText.setPosition(50, progressStartY);
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
    exitProgressText.setPosition(50, 350);
    window.draw(exitProgressText);
    
    // Mostrar control de volumen
    sf::Text volumeInfo;
    volumeInfo.setFont(font);
    volumeInfo.setCharacterSize(12);
    volumeInfo.setFillColor(sf::Color::Cyan);
    volumeInfo.setPosition(50, 450);
    
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

void Game::resetGame() {
    // Reiniciar nivel
    currentLevel = 1;
    // Recrear nivel
    initLevel();
    
    // Reiniciar paddle
    float paddleY = static_cast<float>(window.getSize().y) - 40.f;
    paddle.setPosition(static_cast<float>(window.getSize().x) / 2.f, paddleY);
    
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
        backgroundMusic.stop();    // Parar música de fondo
        gameOverSound.play();      // Reproducir sonido de game over
        showMatrixEffect = true;   // Activar efecto de cascada
        initMatrixEffect();        // Reinicializar columnas
        finalScoreText.setString("Puntuacion Final: " + std::to_string(score) + "\nPresiona ESPACIO para continuar");
        
        // Recentrar el texto de puntuación final
        sf::FloatRect scoreRect = finalScoreText.getLocalBounds();
        finalScoreText.setOrigin(scoreRect.left + scoreRect.width/2.0f, scoreRect.top + scoreRect.height/2.0f);
        finalScoreText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f + 50);
    } else {
        // Reiniciar bola pegada al paddle
        ball.isStuck = true;
        ball.velocity = sf::Vector2f(0.f, 0.f);
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
    backgroundMusic.setVolume(baseVolumeMusic * masterVolume);
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
    
    // Ejecutar comando si está completo
    if (playComplete) {
        // Reiniciar progreso para próxima vez
        playProgress = {false, false, false, false};
        exitProgress = {false, false, false, false};
        currentInput = "";
        
        // Iniciar juego
        state = GameState::Playing;
        resetGame();
        menuMusic.stop();
        backgroundMusic.play();
    }
    else if (exitComplete) {
        // Salir del juego
        menuMusic.stop();
        backgroundMusic.stop();
        window.close();
    }
}

// Actualizar display del terminal
void Game::updateTerminalDisplay() {
    // Esta función se puede usar para efectos adicionales si es necesario
    // Por ahora el renderizado se maneja directamente en renderMenu()
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
        backgroundMusic.play();
    }
    else if (gameOverExitComplete) {
        // Salir del juego
        menuMusic.stop();
        backgroundMusic.stop();
        window.close();
    }
}

