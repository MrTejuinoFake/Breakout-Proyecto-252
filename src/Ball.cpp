#include "Ball.hpp"
#include <cmath>

Ball::Ball(float radius) : sf::CircleShape(radius), velocity(200.f, -200.f), speed(400.f) {
    setFillColor(sf::Color::White);
    setOrigin(radius, radius);
}

void Ball::update(float dt) {
    move(velocity * dt);
}
