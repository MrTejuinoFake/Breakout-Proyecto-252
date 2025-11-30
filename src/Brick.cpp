#include "Brick.hpp"


Brick::Brick(float x, float y, float width, float height, const sf::Color& color, const sf::Texture& texture, int hits) 
    : hitsRemaining(hits), maxHits(hits) {
    setPosition(x, y);
    setSize(sf::Vector2f(width, height));
    
    // 1. Asignamos la textura
    setTexture(&texture);
    
    // 2. Teñimos la textura con el color de la fila
    setFillColor(color); 

    // Opcional: Si quieres que el borde negro siga existiendo
    setOutlineThickness(-1.f);
    setOutlineColor(sf::Color::Black);
}