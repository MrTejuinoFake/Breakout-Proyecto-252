#include "Brick.hpp"

// Constructor ladrillo (Brick)
// Float x, float y: posición
// Float width, float height: tamaño
// const sf::Color& color: color del bloque
// const sf::Texture& texture: textura del bloque
// int hits: Vida del bloque (default 1)
Brick::Brick(float x, float y, float width, float height, const sf::Color &color, const sf::Texture &texture, int hits)
    : hitsRemaining(hits), maxHits(hits)
{
    setPosition(x, y);
    setSize(sf::Vector2f(width, height));

    // Asiga textura al ladrillo
    setTexture(&texture);

    // Asigna color al ladrillo
    setFillColor(color);

    // Estetica para los bordes
    setOutlineThickness(-1.f);
    setOutlineColor(sf::Color::Black);
}