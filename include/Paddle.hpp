#pragma once
#include <SFML/Graphics.hpp>

class Paddle : public sf::RectangleShape
{
public:
    // Constructor
    // float width: ancho de la pala
    // float height: alto de la pala
    Paddle(float width = 120.f, float height = 18.f);
    void update(float dt); // Actualiza la posición de la pala

    float speed = 600.f; // Velocidad de movimiento de la pala
};
