# Documentación Técnica - Arkanoid Proyecto 252

## Resumen Ejecutivo
Implementación moderna de Breakout/Arkanoid usando arquitectura orientada a objetos en C++17 con SFML 2.x. El proyecto destaca por su sistema de UI tipo terminal, gestión avanzada de audio con control dinámico de volumen, y mecánicas de bloques diferenciadas con efectos especiales.

**Stack técnico**: C++17 | SFML 2.x | Make  
**Plataforma**: Windows (portable a Linux/macOS)  
**Resolución**: 1000x800 @ 120 FPS

---

## Arquitectura del Sistema

### Diseño Modular
El proyecto sigue un patrón de separación de responsabilidades con cuatro componentes principales:

**Game** - Controlador central del loop de juego
- Gestiona estados (Menu, Playing, GameOver)
- Coordina entidades (Ball, Paddle, Bricks)
- Maneja sistemas de audio, texturas y efectos visuales
- Implementa terminal interactivo con parsing de comandos

**Ball** - Entidad física con colisiones
- Física basada en vectores de velocidad
- Estado de adherencia al paddle
- Aceleración progresiva por efectos de bloques

**Paddle** - Control del jugador
- Input mapping de teclado
- Límites de movimiento parametrizados
- Influencia en trayectoria de rebote

**Brick** - Objetos destructibles con propiedades
- Sistema de golpes múltiples con degradación visual
- Asignación de puntajes diferenciados
- Variantes especiales con efectos en gameplay

### Flujo de Ejecución
```
main() → Game::Game() → Game::run()
         │                    │
         │                    ├─→ processEvents() - Input handling
         │                    ├─→ update()        - Game logic
         │                    └─→ render()        - Display output
         │
         └─→ Inicialización:
             - Carga de assets (texturas, fuentes, audio)
             - Configuración de ventana y framerate
             - Setup de sistemas (volumen, terminal, Matrix)
```

---

## Sistemas Principales

### 1. Sistema de Estados
El juego opera bajo una máquina de estados finitos:

**Menu**: Terminal interactivo que parsea comandos ("play", "exit") con verificación letra por letra. Visualización de progreso en tiempo real.

**Playing**: Loop principal con detección de colisiones, actualización de física, y renderizado de HUD. Progresión visual mediante tintado RGB del fondo basado en `currentLevel`.

**GameOver**: Pantalla terminal con efecto Matrix (columnas de caracteres animados con desvanecimiento alpha). Comandos disponibles: "reboot" y "exit".

### 2. Sistema de Audio
Arquitectura de tres capas:

**Capa 1 - Volúmenes Base**: Constantes diferenciadas por tipo de audio (música: 30-40, efectos: 70, gameover: 50).

**Capa 2 - Volumen Maestro**: Multiplicador global (0.0-1.0) controlado por usuario vía input numérico.

**Capa 3 - Aplicación**: `updateMasterVolume()` sincroniza todas las fuentes de audio con el volumen resultante.

### 3. Sistema de Colisiones
Detección por intersección de `FloatRect` con respuesta basada en análisis de overlap mínimo:

- **Ball-Paddle**: Ajuste de velocidad X proporcional a posición de impacto (`offset * 300`)
- **Ball-Brick**: Inversión de componente de velocidad según lado de colisión (horizontal/vertical)
- **Ball-Walls**: Rebote simple con inversión de eje correspondiente

### 4. Generación de Niveles
`initLevel()` crea grilla procedural de bloques con distribución probabilística:

**Parámetros configurables**:
- Dimensiones: 16 columnas × 8 filas (configurable)
- Probabilidades: 15% amarillos, 10% morados, 25% rojos, 50% normales
- Geometría: cellWidth 60px, cellHeight 30px, márgenes ajustables
- Recompensas: 10/20/100/50 puntos según tipo

**Tipos de bloques y comportamiento**:
- Normal (1 hit): Destrucción inmediata, textura aleatoria
- Rojo (2 hits): Cambio de color rojo→naranja→destrucción
- Morado (3 hits): Progresión morado→rosa→naranja→destrucción
- Amarillo (1 hit): 50% tamaño, acelera velocidad bola (+10%)

### 5. Sistema de Efectos Visuales
**Progresión cromática**: Interpolación RGB del fondo basada en nivel actual. Fórmula:
```
redIntensity = min(1.0, (nivel-1) * 0.1)
green = 255 * (1 - redIntensity * 0.5)
blue = 255 * (1 - redIntensity * 0.8)
```

**Efecto Matrix**: Generación de columnas animadas con:
- Spacing parametrizado (35px por defecto)
- Longitud variable (5-12 caracteres)
- Velocidad diferenciada (30-80 px/s)
- Desvanecimiento alpha progresivo

---

## Configuración y Personalización

### Constantes Críticas (Game.cpp)

**initLevel()**:
```cpp
columns = 16, rows = 8
cellWidth = 60.f, cellHeight = 30.f
smallBlockChance = 15, purpleBlockChance = 10, redBlockChance = 25
normalBlockPoints = 10, redBlockPoints = 20, 
purpleBlockPoints = 100, smallBlockPoints = 50
```

**Física (Ball.cpp, Paddle.cpp)**:
```cpp
Ball::speed = 350.0f, radius = 8.f
Paddle::speed = 600.f, width = 120.f, height = 18.f
```

**Audio**:
```cpp
baseVolumeMusic = 30.0f, baseVolumeEffects = 70.0f
baseVolumeMenu = 40.0f, baseVolumeGameOver = 50.0f
```

### Assets y Recursos

**Estructura esperada**:
```
assets/
├─ fonts/PressStart2P-Regular.ttf
├─ images/
│  ├─ brick[1-5].jpg (texturas bloques normales)
│  ├─ x.jpg (textura bloques amarillos)
│  ├─ menu_background.png (fondo universal)
│  └─ logo.png (icono ventana)
└─ music/
   ├─ background.mp3 (gameplay loop)
   ├─ menu.mp3 (menú loop)
   ├─ bounce.mp3 (efecto rebote)
   └─ gameover.mp3 (efecto final)
```

**Agregar texturas**: Modificar array `brickTextures[]` en Game.hpp, actualizar carga en constructor, y ajustar índice aleatorio en `initLevel()`.

**Cambiar audio**: Agregar `SoundBuffer` + `Sound` en Game.hpp, cargar en constructor con `loadFromFile()`, asignar volumen inicial, y reproducir con `.play()`.

---

## Compilación y Despliegue

### Requisitos
- Compilador compatible C++17 (g++/MinGW 7.x+)
- SFML 2.5.x o superior (graphics, window, system, audio)
- Make 4.x

### Build
```bash
make                    # Compilar proyecto completo
make clean             # Limpiar objetos y ejecutables
bin/Arkanoid.exe       # Ejecutar
```

### Makefile Overview
```makefile
CXX = g++
CXXFLAGS = -Iinclude
LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-main
SOURCES = src/{Ball,Brick,Paddle,Game,main}.cpp
TARGET = bin/Arkanoid.exe
```

---

## Interfaz de Usuario

### Esquema de Controles

**Contexto Menú**:
- Texto: Comandos "play"/"exit" con parsing no-secuencial
- V: Activar modo volumen
- ESC: Salir aplicación

**Contexto Juego**:
- Flechas: Control direccional paleta
- Espacio: Release de bola desde estado stuck
- V: Modo volumen
- ESC: Retorno a menú

**Contexto GameOver**:
- Texto: Comandos "reboot"/"exit"
- V: Modo volumen
- ESC: Salir

**Sistema de Volumen** (universal):
1. Trigger: Tecla V
2. Input: Dígitos 0-9 (máx 2 caracteres)
3. Confirm: Enter → aplicar volumen [0-99]
4. Cancel: ESC → descartar cambios

### HUD y Feedback Visual

**Durante Juego**:
- Superior izquierda: Vidas restantes
- Superior derecha: Score acumulado + volumen actual
- Bloques: Degradación de color según hits restantes

**Terminal**:
- Prompt estilo DOS: "C:\ARKANOID>"
- Progreso de comandos con resaltado de letras ingresadas
- Cursor parpadeante post-input

---

## Mecánicas de Juego

### Sistema de Vidas y Progresión
- Vidas iniciales: 3
- Pérdida: Ball.y > windowHeight
- GameOver: lives == 0 → transición a estado GameOver
- Avance de nivel: Todos los bloques destruidos → `currentLevel++` + `initLevel()`

### Física y Dinámica
**Rebote en Paddle**: Cálculo de offset desde centro produce ángulo de salida variable. Valores extremos generan trayectorias horizontales pronunciadas.

**Aceleración por Bloques Amarillos**: Multiplicador 1.1x aplicado a `ball.speed` en cada colisión. Acumulativo entre niveles hasta reset manual.

**Colisiones**: Análisis de overlap en 4 direcciones determina normal de rebote. Previene atravesamiento mediante inversión inmediata de velocidad.

---

## Extensibilidad

### Agregar Nuevo Tipo de Bloque
1. Definir probabilidad en `initLevel()`: `const int newBlockChance = X`
2. Agregar lógica condicional en generación: `if (random < threshold)`
3. Asignar propiedades: `hits`, `color`, `pointValue`
4. Implementar comportamiento especial en `update()` si aplica

### Implementar Power-Ups
1. Crear clase `PowerUp` derivada de entidad base
2. Spawn en destrucción de bloques según probabilidad
3. Detección de colisión Paddle-PowerUp en `Game::update()`
4. Aplicar efecto: modificar propiedades de Game/Ball/Paddle

### Sistema de Guardado
Serializar estado:
```cpp
struct GameState {
    int lives, score, currentLevel;
    float ballSpeed;
    // Persistir en JSON/binario
};
```

---

## Debugging y Troubleshooting

### Errores Comunes

**Assets no encontrados**:
- Verificar rutas relativas desde ejecutable
- Confirmar existencia de archivos en `assets/`
- Revisar output console para mensajes de error de carga

**Audio sin reproducir**:
- Validar `masterVolume > 0`
- Confirmar llamadas a `updateMasterVolume()`
- Verificar formato de archivo compatible (MP3/OGG)

**Ventana no responde**:
- Asegurar `processEvents()` en cada frame
- Verificar manejo de `sf::Event::Closed`

### Puntos de Instrumentación
- Constructor de Game: Confirmación de carga de assets
- `initLevel()`: Conteo de bloques generados
- `loseLife()`: Estado de vidas y transiciones
- `updateMasterVolume()`: Valores de volumen aplicados

---

## Roadmap y Mejoras Futuras

**Implementado**:
✅ Sistema de estados completo  
✅ Terminal interactivo con parsing robusto  
✅ Control de volumen dinámico  
✅ Bloques multi-golpe y especiales  
✅ Progresión visual por nivel  
✅ Efecto Matrix en GameOver  

**Pendiente**:
- [ ] Sistema de power-ups (vida extra, multi-bola, paddle extendido)
- [ ] Persistencia de highscores con serialización
- [ ] Múltiples bolas simultáneas con física independiente
- [ ] Modos de juego alternativos (endless, time attack)
- [ ] Soporte de gamepad vía SFML Joystick API
- [ ] Sistema de partículas para destrucción de bloques

---

## Créditos y Licencia

**Desarrollo**: Proyecto 252  
**Framework**: SFML (Simple and Fast Multimedia Library)  
**Documentación**: Diciembre 2025  
**Versión**: 1.0

Para contribuciones, seguir estilo de código existente (CamelCase clases, comentarios explicativos, constantes configurables). Pull requests bienvenidos con tests de regresión.
