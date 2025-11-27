#include "Game.hpp"
#include <ctime>
#include <cstdlib>


void Game::initLevel() {
    // CONFIGURACIÓN DE LA MATRIZ
    const int columns = 10;        // 10 columnas
    const int rows = 8;            // 8 filas de ladrillos
    const float cellWidth = 80.f;  // Ancho de cada celda (800px / 10)
    const float cellHeight = 30.f; // Alto de cada celda
    const float startY = 50.f;     // Margen superior

    // Matriz para saber si una celda ya está ocupada
    // false = libre, true = ocupada
    bool grid[rows][columns] = {false};

    bricks.clear(); // Limpiar por si reiniciamos el juego

    // Recorrer filas y columnas
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < columns; ++x) {
            
            // Si la celda ya está ocupada por un bloque anterior, saltar
            if (grid[y][x]) continue;

            // Decidir tamaño aleatorio del bloque (1, 2 o 3 celdas de ancho)
            // rand() % 3 da 0, 1 o 2. Le sumamos 1 para tener ancho 1, 2 o 3.
            int widthInCells = (std::rand() % 3) + 1;

            // Verificar si el bloque cabe (que no se salga del borde derecho)
            if (x + widthInCells > columns) {
                widthInCells = columns - x; // Recortarlo para que ajuste al borde
            }

            // Verificar si las celdas siguientes están libres
            // Si el bloque es de ancho 2, verificamos que la celda de al lado esté libre
            bool fits = true;
            for (int k = 0; k < widthInCells; ++k) {
                if (grid[y][x + k]) {
                    fits = false;
                    break;
                }
            }
            // Si no cabe (choca con otro bloque), lo forzamos a ser de 1 celda
            if (!fits) widthInCells = 1;

            
            
            // Color aleatorio
            sf::Uint8 r = std::rand() % 256;
            sf::Uint8 g = std::rand() % 256;
            sf::Uint8 b = std::rand() % 256;
            sf::Color randomColor(r, g, b);

            // Calcular posición real en píxeles
            float posX = x * cellWidth;
            float posY = startY + y * cellHeight;
            float realWidth = (widthInCells * cellWidth); 

            // Crear objeto y añadirlo al vector
            bricks.emplace_back(posX, posY, realWidth, cellHeight, randomColor);

            // MARCAR LAS CELDAS COMO OCUPADAS
            for (int k = 0; k < widthInCells; ++k) {
                grid[y][x + k] = true;
            }
            

        }
    }
}


// Constructor: Inicializa la ventana
Game::Game() {
    window.create(sf::VideoMode(800, 600), "Arkanoid - Procedural");
    window.setFramerateLimit(60);
    
    // Semilla para números aleatorios (para que cambie cada vez que abres el juego)
    std::srand(static_cast<unsigned>(std::time(nullptr)));

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