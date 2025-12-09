#pragma once
#include <SFML/Graphics.hpp>

enum class PowerUpType {
    SpeedPaddle,
    ExpandPaddle,
    MultiBall,
    ExtraLife
};

class PowerUp : public sf::RectangleShape {
public:
    PowerUp(float x, float y, PowerUpType type);
    
    void update(float dt);
    PowerUpType getType() const { return type; }
    bool isActive() const { return active; }
    void deactivate() { active = false; }
    void setTexture(const sf::Texture* texture);
    
private:
    PowerUpType type;
    float speed;
    bool active;
};
