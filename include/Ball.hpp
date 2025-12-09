#pragma once
#include <SFML/Graphics.hpp>

class Ball : public sf::CircleShape {
public:
    Ball(float radius = 8.f, bool isExtra = false);
    void update(float dt);

    sf::Vector2f velocity;
    float speed;
    
    // NUEVO: Estado para saber si está pegada al paddle
    bool isStuck;
    bool isExtraBall;  // Para identificar bolas extra del MultiBall
};