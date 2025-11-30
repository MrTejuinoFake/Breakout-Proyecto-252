#pragma once
#include <SFML/Graphics.hpp>

class Brick : public sf::RectangleShape {
public:
    // Agregamos 'const sf::Texture& texture' al constructor
    Brick(float x, float y, float width, float height, const sf::Color& color, const sf::Texture& texture, int hits = 1);

    bool isDestroyed = false;
    int hitsRemaining = 1;  // Golpes restantes para destruir el bloque
    int maxHits = 1;        // Golpes máximos (para cambiar color)
};