#include "Ball.hpp"

Ball::Ball(float radius, bool isExtra) : sf::CircleShape(radius), isExtraBall(isExtra) {
    // Velocidad base (X, Y)
    velocity = {0.f, 0.f}; 
    speed = 400.f;
    
    // Por defecto empieza pegada
    isStuck = true; 

    // Color según tipo: blanca para principal, cyan/magenta para extras
    if (isExtra) {
        // Color neón aleatorio para bolas extra
        int colorChoice = std::rand() % 2;
        if (colorChoice == 0) {
            setFillColor(sf::Color::Cyan);    // Neón cyan
        } else {
            setFillColor(sf::Color::Magenta); // Neón magenta
        }
    } else {
        setFillColor(sf::Color::White);  // Blanca para la principal
    }
    
    setOrigin(radius, radius);
}

void Ball::update(float dt) {
    // Solo se mueve sola si NO está pegada
    if (!isStuck) {
        move(velocity * dt);
    }
}