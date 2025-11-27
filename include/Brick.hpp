#pragma once
#include <SFML/Graphics.hpp>

class Brick : public sf::RectangleShape {
public:
    // Constructor: Posición X, Y, Ancho, Alto, Color
    Brick(float x, float y, float width, float height, sf::Color color);

    // Aquí agregaremos lógica de vida más tarde (si se rompe de un golpe o dos)
    bool isDestroyed = false; 
};