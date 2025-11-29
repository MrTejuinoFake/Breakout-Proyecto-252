#include "Game.hpp"
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <cmath>


void Game::initLevel() {
    // CONFIGURACIÓN DE LA GRILLA
    const int columns = 15;        
    const int rows = 8;            
    const float cellWidth = 80.f;  
    const float cellHeight = 30.f; 
    const float startY = 50.f;     

    // Matriz de ocupación
    bool grid[rows][columns] = {false};

    bricks.clear(); 

    // BUCLE DE FILAS (Vertical)
    for (int y = 0; y < rows; ++y) {

        // --- CAMBIO AQUÍ ---
        // Generamos el color UNA vez por cada Fila
        sf::Uint8 r = std::rand() % 256;
        sf::Uint8 g = std::rand() % 256;
        sf::Uint8 b = std::rand() % 256;
        sf::Color rowColor(r, g, b);
        
        // Evitar color negro absoluto (opcional, para que se vea)
        if (r < 50 && g < 50 && b < 50) { 
             rowColor = sf::Color::White; 
        }

        // BUCLE DE COLUMNAS (Horizontal)
        for (int x = 0; x < columns; ++x) {
            
            if (grid[y][x]) continue;

            // Decidir tamaño aleatorio (1, 2 o 3 celdas)
            int widthInCells = (std::rand() % 2) + 1;

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
            float realWidth = (widthInCells * cellWidth); 

            // Usamos 'rowColor' que definimos arriba
            bricks.emplace_back(posX, posY, realWidth, cellHeight, rowColor,blockTexture);

            // Marcar celdas
            for (int k = 0; k < widthInCells; ++k) {
                grid[y][x + k] = true;
            }
        }
    }
}


// Constructor: Inicializa la ventana
Game::Game() {
    window.create(sf::VideoMode(1000, 800), "Arkanoid - Procedural(brick and texture)");
    window.setFramerateLimit(60);
    
    // Semilla para números aleatorios (para que cambie cada vez que abres el juego)
    std::srand(static_cast<unsigned>(std::time(nullptr)));

        blockTexture.setRepeated(true);


    std::string files[] = {"brick1.jpg", "brick2.jpg", "brick3.jpg", "brick4.jpg", "brick5.jpg"};
    for (int i = 0; i < 5; ++i) {
        if (!brickTextures[i].loadFromFile("assets/images/" + files[i])) {
            // Manejo de error si una imagen no se encuentra
            std::cerr << "Error cargando textura: " << files[i] << std::endl;
        }
        brickTextures[i].setRepeated(true);
    }
    
    // Cargar una textura principal para usar en los ladrillos
    //if (!blockTexture.loadFromFile("assets/images/brick.jpeg")) {
    //    std::cerr << "Error cargando textura principal: brick.jpeg" << std::endl;
    //}

    // Para que no se vea borroso si es pixel art
    blockTexture.setSmooth(true);
    initLevel(); 

    // Inicializar paddle en la parte inferior
    float paddleY = static_cast<float>(window.getSize().y) - 40.f;
    paddle.setPosition(static_cast<float>(window.getSize().x) / 2.f, paddleY);

    // Inicializar bola encima del paddle
    ball.setPosition(paddle.getPosition().x, paddle.getPosition().y - paddle.getSize().y/2.f - ball.getRadius() - 2.f);
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
        
   
    }
}

// Lógica (Movimiento, colisiones)
void Game::update() {
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

    // --- Bola: actualizar posición ---
    ball.update(dt);

    // Colisión con paredes (izquierda/derecha)
    float r = ball.getRadius();
    sf::Vector2f pos = ball.getPosition();
    if (pos.x - r < 0.f) {
        ball.setPosition(r, pos.y);
        ball.velocity.x = std::abs(ball.velocity.x);
    }
    if (pos.x + r > window.getSize().x) {
        ball.setPosition(static_cast<float>(window.getSize().x) - r, pos.y);
        ball.velocity.x = -std::abs(ball.velocity.x);
    }
    // Colisión con techo
    if (pos.y - r < 0.f) {
        ball.setPosition(pos.x, r);
        ball.velocity.y = std::abs(ball.velocity.y);
    }
    // Si la bola cae abajo, reubicar sobre el paddle (reset simple)
    if (pos.y - r > window.getSize().y) {
        ball.setPosition(paddle.getPosition().x, paddle.getPosition().y - paddle.getSize().y/2.f - r - 2.f);
        ball.velocity = sf::Vector2f(200.f, -200.f);
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
    }

    // Colisión con ladrillos
    for (auto& brick : bricks) {
        if (brick.isDestroyed) continue;
        if (ball.getGlobalBounds().intersects(brick.getGlobalBounds())) {
            brick.isDestroyed = true;
            // Invertir componente Y de la velocidad
            ball.velocity.y = -ball.velocity.y;
            break; // solo procesar una colisión por frame
        }
    }
}

// Dibujado
void Game::render() {
    window.clear(sf::Color::Black);

    // Dibujar todos los ladrillos de la lista
    for (const auto& brick : bricks) {
        if (!brick.isDestroyed)
            window.draw(brick);
    }

    // Dibujar paddle y bola
    window.draw(paddle);
    window.draw(ball);

    window.display();
}