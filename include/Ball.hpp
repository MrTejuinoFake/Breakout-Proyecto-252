#pragma once
#include <SFML/Graphics.hpp>

class Ball : public sf::CircleShape
{
public:
    // Constructor pelota
    // float radius: radio de la bola
    // bool isExtra: indica si la bola fue generada por el power-up MultiBall
    Ball(float radius = 8.f, bool isExtra = false);
    void update(float dt); // Actualiza la posición de la bola

    sf::Vector2f velocity; // Velocidad de la bola
    float speed;           // Magnitud de la velocidad

    // Variables de estado
    bool isStuck;     // Indica si la bola está pegada a la pala
    bool isExtraBall; // Para identificar bolas generadas por MultiBall
};