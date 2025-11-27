#pragma once
#include <SFML/Graphics.hpp>

class Brick : public sf::RectangleShape {
public:
    // Agregamos 'const sf::Texture& texture' al constructor
    Brick(float x, float y, float width, float height, const sf::Color& color, const sf::Texture& texture);

    bool isDestroyed = false; 
};