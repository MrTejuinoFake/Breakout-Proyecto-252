#include "Ball.hpp"

Ball::Ball(float radius) : sf::CircleShape(radius) {
    // Velocidad base (X, Y)
    velocity = {0.f, 0.f}; 
    speed = 400.f;
    
    // Por defecto empieza pegada
    isStuck = true; 

    setFillColor(sf::Color::White);
    setOrigin(radius, radius);
}

void Ball::update(float dt) {
    // Solo se mueve sola si NO está pegada
    if (!isStuck) {
        move(velocity * dt);
    }
}