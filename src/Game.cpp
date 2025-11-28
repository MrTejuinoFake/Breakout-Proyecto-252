#include "Game.hpp"
#include <ctime>
#include <cstdlib>
#include <iostream>


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
    // Aquí actualizaremos la física de la bola y colisiones
}

// Dibujado
void Game::render() {
    window.clear(sf::Color::Black);

    // Dibujar todos los ladrillos de la lista
    for (const auto& brick : bricks) {
        window.draw(brick);
    }

    window.display();
}