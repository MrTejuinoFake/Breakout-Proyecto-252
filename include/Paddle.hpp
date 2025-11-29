#pragma once
#include <SFML/Graphics.hpp>

class Paddle : public sf::RectangleShape {
public:
    Paddle(float width = 120.f, float height = 18.f);
    void update(float dt);

    float speed = 600.f;
};
