#pragma once
#include <SFML/Graphics.hpp>

// Enumeración de tipos de power-up
enum class PowerUpType
{
    SpeedPaddle,  // Aumenta la velocidad de la pala
    ExpandPaddle, // Expande el tamaño de la pala
    MultiBall,    // Genera bolas extra
    ExtraLife     // Otorga una vida extra
};

class PowerUp : public sf::RectangleShape
{
public:
    // Constructor
    // Inicializa el objeto que caerá desde un bloque destruido
    // float x, float y: posición inicial
    // PowerUpType type: tipo de power-up
    PowerUp(float x, float y, PowerUpType type);

    void update(float dt);                       // Actualiza la posición del power-up
    PowerUpType getType() const { return type; } // Obtiene el tipo de power-up
    bool isActive() const { return active; }     // Verifica si el power-up está activo
    void deactivate() { active = false; }        // Desactiva el power-up
    void setTexture(const sf::Texture *texture); // Asigna la textura al power-up

private:
    PowerUpType type; // Tipo de power-up
    float speed;      // Velocidad de caída del power-up
    bool active;      // Indica si el power-up está activo
};
