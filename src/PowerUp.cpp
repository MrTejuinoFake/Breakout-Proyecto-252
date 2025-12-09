#include "PowerUp.hpp"

PowerUp::PowerUp(float x, float y, PowerUpType type) 
    : type(type), speed(150.f), active(true) {
    
    setSize(sf::Vector2f(40.f, 40.f));
    setPosition(x, y);
}

void PowerUp::update(float dt) {
    if (active) {
        move(0.f, speed * dt);
    }
}

void PowerUp::setTexture(const sf::Texture* texture) {
    sf::RectangleShape::setTexture(texture);
}
