#pragma once
#include <SFML/Graphics.hpp>

class Brick : public sf::RectangleShape
{
public:
    // Constructor ladrillo (Brick)
    // Float x, float y: posición
    // Float width, float height: tamaño
    // const sf::Color& color: color del bloque
    // const sf::Texture& texture: textura del bloque
    Brick(float x, float y, float width, float height, const sf::Color &color, const sf::Texture &texture, int hits = 1);

    // Variables de estado
    bool isDestroyed = false;  // Indica si el bloque ha sido destruido
    int hitsRemaining = 1;     // Golpes restantes para destruir el bloque
    int maxHits = 1;           // Golpes máximos (para cambiar color)
    int pointValue = 10;       // Puntos que da al destruirse
    bool isSmallBlock = false; // Si es un bloque pequeño
    bool isIndestructible = false; // Si es un bloque indestructible
};