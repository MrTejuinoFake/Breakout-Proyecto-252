#include "Brick.hpp"


Brick::Brick(float x, float y, float width, float height, const sf::Color& color, const sf::Texture& texture) {
    setPosition(x, y);
    setSize(sf::Vector2f(width, height));
    
    // 1. Asignamos la textura
    setTexture(&texture);
    
    // 2. Teñimos la textura con el color de la fila
    // Esto permite usar UNA sola imagen gris/blanca y que se vea de colores
    setFillColor(color); 

    // Opcional: Si quieres que el borde negro siga existiendo
    setOutlineThickness(-1.f);
    setOutlineColor(sf::Color::Black);
}