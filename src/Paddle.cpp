#include "Paddle.hpp"

Paddle::Paddle(float width, float height) {
    setSize(sf::Vector2f(width, height));
    setFillColor(sf::Color::White);
    setOrigin(width / 2.f, height / 2.f);
}

void Paddle::update(float dt) {
    // Movimiento manejado desde Game (input directo), pero dejamos la función
}
