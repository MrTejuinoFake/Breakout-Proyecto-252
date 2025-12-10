#include "Paddle.hpp"

// Constructor
Paddle::Paddle(float width, float height)
{
    setSize(sf::Vector2f(width, height)); // Define las dimensiones del rectangulo
    setFillColor(sf::Color::White);       // Pone color
    setOrigin(width / 2.f, height / 2.f); // Mueve el punto de anclaje al centro de la figura
}

//[[[[[[[[MODIFICAR]]]]]]]]]]]
void Paddle::update(float dt)
{
    // Movimiento manejado desde Game (input directo), pero dejamos la función
}
