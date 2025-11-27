#include "Brick.hpp"

Brick::Brick(float x, float y, float width, float height, sf::Color color) {
    setPosition(x, y);
    setSize(sf::Vector2f(width, height));
    setFillColor(color);
    
    // Un borde negro delgado para que se distingan los bloques entre sí
    setOutlineThickness(-1.f); 
    setOutlineColor(sf::Color::White);
}