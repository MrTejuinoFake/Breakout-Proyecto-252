#include "Ball.hpp"

// Constructor
// float radius: radio de la bola
// bool isExtra: indica si la bola fue generada por el power-up MultiBall
Ball::Ball(float radius, bool isExtra) : sf::CircleShape(radius), isExtraBall(isExtra)
{
    // Velocidad base (X, Y)
    velocity = {0.f, 0.f};
    speed = 400.f;

    isStuck = true; // Variable para saber si está pegada a la pala

    // Color según tipo. Extra o no
    if (isExtra) // Color aleatorio para bolas extra
    {

        int colorChoice = std::rand() % 2; // Numero aleatorio entre 0 y 1
        if (colorChoice == 0)              // Si es 0 Color cyan
        {
            setFillColor(sf::Color::Cyan);
        }
        else // Si no Color magenta
        {
            setFillColor(sf::Color::Magenta);
        }
    }
    else // Color blanco para bola normal
    {
        setFillColor(sf::Color::White);
    }

    setOrigin(radius, radius); // Mover centro de la bola
}

// Actualizar en cada frame
void Ball::update(float dt)
{
    // Solo se mueve sola si NO está pegada
    if (!isStuck)
    {
        move(velocity * dt);
    }
}