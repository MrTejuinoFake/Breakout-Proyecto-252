#include "PowerUp.hpp"

// Constructor
// Inicializa el objeto que caerá desde un bloque destruido
// float x, float y: posición inicial
// PowerUpType type: tipo de power-up
PowerUp::PowerUp(float x, float y, PowerUpType type)
    : type(type), speed(150.f), active(true), effectDuration(0.f), effectElapsedTime(0.f)
{

    setSize(sf::Vector2f(40.f, 40.f)); // Tamaño del power-up
    setPosition(x, y);                 // Posición inicial
}

// Actualiza la posición del power-up
void PowerUp::update(float dt)
{
    if (active)
    {
        move(0.f, speed * dt);
    }
}

// Asigna la textura al power-up
void PowerUp::setTexture(const sf::Texture *texture)
{
    sf::RectangleShape::setTexture(texture);
}

// Establecer duración del efecto en segundos
void PowerUp::setEffectDuration(float seconds)
{
    effectDuration = seconds;
    effectElapsedTime = 0.f;
}

// Actualizar temporizador del efecto
void PowerUp::updateEffectTimer(float dt)
{
    if (effectDuration > 0.f && effectElapsedTime < effectDuration)
    {
        effectElapsedTime += dt;
    }
}
