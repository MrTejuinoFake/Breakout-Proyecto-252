#include "Game.hpp"
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>


void Game::initLevel() {
    // CONFIGURACIÓN de la posicion de bloques
    const int columns = 16;        
    const int rows = 8;            
    const float cellWidth = 62.f;  
    const float cellHeight = 30.f; 
    const float startY = 50.f;     

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
            
            if (blockTypeRandom < 15) {  // 15% bloques pequeños
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



            int textureIndex = std::rand() % 5;
            const sf::Texture& selectedTexture = brickTextures[textureIndex];
            
            // --- CREAR EL LADRILLO ---
            float posX = x * cellWidth;
            float posY = startY + y * cellHeight;
            float realWidth = isSmallBlock ? (cellWidth * 0.6f) : (widthInCells * cellWidth);
            float realHeight = isSmallBlock ? (cellHeight * 0.6f) : cellHeight;
            
            // Centrar bloques pequeños
            if (isSmallBlock) {
                posX += (cellWidth - realWidth) / 2.0f;
                posY += (cellHeight - realHeight) / 2.0f;
            } 

            // Decidir aleatoriamente el tipo de bloque
            int hits = 1;
            sf::Color brickColor = rowColor;
            int pointValue = 10;  // Valor por defecto
            
            if (isSmallBlock) {
                brickColor = sf::Color(255, 255, 0);  // Amarillo para bloques pequeños
                pointValue = 50;  // Bloques pequeños dan más puntos
            } else {
                int random = std::rand() % 100;
                
                if (random < 10) {  // 10% de probabilidad - BLOQUES MORADOS (3 golpes)
                    hits = 3;
                    brickColor = sf::Color(150, 0, 200);  // Morado intenso
                    pointValue = 100;  // 100 puntos al destruirlo completamente
                }
                else if (random < 25) {  // 15% de probabilidad - BLOQUES ROJOS (2 golpes)
                    hits = 2;
                    brickColor = sf::Color(200, 0, 0);  // Rojo intenso
                    pointValue = 20;
                }
                // El resto son bloques normales de 1 golpe (10 puntos)
            }

            bricks.emplace_back(posX, posY, realWidth, realHeight, brickColor, selectedTexture, hits);
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
Game::Game() : state(GameState::Menu), lives(3), score(0) {
    std::cout << "--- INICIANDO JUEGO ---" << std::endl;
    window.create(sf::VideoMode(1000, 800), "Arkanoid - sonido fisicas(blk ylw & prl)", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(120);
    
    // Semilla para números aleatorios (para que cambie cada vez que abres el juego)
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    // Cargar fuente para el menú
    if (!font.loadFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
        // Si no encuentra la fuente, usar la fuente por defecto del sistema
        std::cout << "No se pudo cargar arial.ttf, usando fuente por defecto" << std::endl;
        exit(1);
    }
    
    // Configurar texto del menú
    menuText.setFont(font);
    menuText.setString("ARKANOID");
    menuText.setCharacterSize(72);
    menuText.setFillColor(sf::Color::White);
   // menuText.setPosition(200, 250);

    sf::FloatRect titleRect = menuText.getLocalBounds();
    menuText.setOrigin(titleRect.left + titleRect.width/2.0f, titleRect.top + titleRect.height/2.0f);
    menuText.setPosition(window.getSize().x / 2.0f, 200);
    
    instructionText.setFont(font);
    instructionText.setString("Presiona ESPACIO para jugar\nPresiona ESC para salir");
    instructionText.setCharacterSize(24);
    instructionText.setFillColor(sf::Color::Yellow);
    instructionText.setPosition(320, 400);
    
    // Configurar textos del juego
    livesText.setFont(font);
    livesText.setCharacterSize(20);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(10, 10);
    
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 40);
    
    // Textos de Game Over
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(72);
    gameOverText.setFillColor(sf::Color::Red);
    
    finalScoreText.setFont(font);
    finalScoreText.setCharacterSize(32);
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
        bounceSound.setVolume(70);  
    }
    
    if (!backgroundMusic.openFromFile("assets/music/background.mp3")) {
        std::cout << "No se pudo cargar background.mp3" << std::endl;
    } else {
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(30); 
    }
    
    // Cargar música del menú
    if (!menuMusic.openFromFile("assets/music/menu.mp3")) {
       // std::cout << "No se pudo cargar menu.mp3" << std::endl;
    } else {
        menuMusic.setLoop(true);
        menuMusic.setVolume(40);  
    }
    
    // Cargar sonido de game over
    if (!gameOverBuffer.loadFromFile("assets/music/gameover.mp3")) {
    } else {
        gameOverSound.setBuffer(gameOverBuffer);
        gameOverSound.setVolume(50);  
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
    
 
    // Para que no se vea borroso si es pixel art
    blockTexture.setSmooth(true);
    initLevel(); 

    // Inicializar paddle en la parte inferior
    float paddleY = static_cast<float>(window.getSize().y) - 40.f;
    paddle.setPosition(static_cast<float>(window.getSize().x) / 2.f, paddleY);

    // Inicializar bola pegada al paddle
    ball.setPosition(paddle.getPosition().x, paddle.getPosition().y - paddle.getSize().y/2.f - ball.getRadius() - 2.f);
    ball.isStuck = true; // Comienza pegada
    ball.velocity = sf::Vector2f(0.f, 0.f); // Sin velocidad inicial
    
    // Iniciar música del menú automáticamente
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
            
        if (event.type == sf::Event::KeyPressed) {
            if (state == GameState::Menu) {
                if (event.key.code == sf::Keyboard::Space) {
                    state = GameState::Playing;
                    resetGame();
                    menuMusic.stop();           // Parar música del menú
                    backgroundMusic.play();     // Iniciar música de fondo
                }
                if (event.key.code == sf::Keyboard::Escape) {
                    menuMusic.stop();
                    backgroundMusic.stop();
                    window.close();
                }
            }
            else if (state == GameState::GameOver) {
                if (event.key.code == sf::Keyboard::Space) {
                    state = GameState::Menu;
                    lives = 3;
                    score = 0;
                    backgroundMusic.stop();  // Parar música de fondo
                    menuMusic.play();        // Volver a música del menú
                }
                if (event.key.code == sf::Keyboard::Escape) {
                    menuMusic.stop();
                    backgroundMusic.stop();
                    window.close();
                }
            }
            else if (state == GameState::Playing) {
                if (event.key.code == sf::Keyboard::Space && ball.isStuck) {
                    ball.isStuck = false;
                    ball.velocity = sf::Vector2f(0.f, -350.f); // Lanzar derecha hacia arriba
                }
            }
        }
    }
}

// Lógica (Movimiento, colisiones)
void Game::update() {
    if (state != GameState::Playing) return;
    
    float dt = clock.restart().asSeconds();

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
    
    // Si todos los bloques están destruidos, reiniciar nivel
    if (allDestroyed && !bricks.empty()) {
        initLevel();  // Recrear todos los bloques
        // Reiniciar bola pegada al paddle
        ball.isStuck = true;
        ball.velocity = sf::Vector2f(0.f, 0.f);
    }
}

// Dibujado
void Game::render() {
    window.clear(sf::Color::Black);

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
        
        // Actualizar y dibujar HUD
        livesText.setString("Vidas: " + std::to_string(lives));
        scoreText.setString("Puntos: " + std::to_string(score));
        window.draw(livesText);
        window.draw(scoreText);
    } else if (state == GameState::GameOver) {
        renderGameOver();
    }

    window.display();
}

void Game::renderMenu() {
    // Si no se pudo cargar la fuente, dibujar menú simple con formas
    if (font.getInfo().family.empty()) {
        // Crear un rectángulo para el título
        sf::RectangleShape titleBg(sf::Vector2f(400, 80));
        titleBg.setPosition(300, 250);
        titleBg.setFillColor(sf::Color(100, 50, 150));
        titleBg.setOutlineColor(sf::Color::White);
        titleBg.setOutlineThickness(3);
        window.draw(titleBg);
        
        // Crear un rectángulo para las instrucciones
        sf::RectangleShape instructBg(sf::Vector2f(500, 100));
        instructBg.setPosition(250, 400);
        instructBg.setFillColor(sf::Color(50, 50, 100));
        instructBg.setOutlineColor(sf::Color::Yellow);
        instructBg.setOutlineThickness(2);
        window.draw(instructBg);
        
        // Texto simple (puede no verse bien pero al menos será visible)
        sf::Text simpleTitle("ARKANOID", font, 48);
        simpleTitle.setPosition(350, 270);
        simpleTitle.setFillColor(sf::Color::White);
        window.draw(simpleTitle);
        
        sf::Text simpleInst("ESPACIO = Jugar    ESC = Salir", font, 20);
        simpleInst.setPosition(280, 430);
        simpleInst.setFillColor(sf::Color::Yellow);
        window.draw(simpleInst);
    } else {
        // Si se cargó la fuente, usar el menú normal
        window.draw(menuText);
        window.draw(instructionText);
    }
}

void Game::resetGame() {
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
    window.draw(gameOverText);
    window.draw(finalScoreText);
}

