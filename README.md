# 🎮 Arkanoid - Proyecto 252

> Breakout modernizado con menú estilo terminal, audio completo y efecto Matrix.

## 
- **Comandos**: PLAY / EXIT (menú), REBOOT / EXIT (game over)
- **Controles**: ← → mover, ESPACIO lanzar, V volumen (00-99 + Enter), ESC salir/menú
- **Bloques**: normales (10), rojos x2 (20), morados x3 (100), amarillos speed +10% (50)
- **Audio**: música de menú/juego + FX rebote + game over, volumen maestro en vivo
- **Efectos**: fondo con tinte por nivel, Matrix en Game Over

## 🎯 Qué tiene de especial
- Menú y game over tipo **terminal** con comandos escritos
- **Bloques multi-golpe** y amarillos que aceleran la pelota
- **Volumen maestro** editable en cualquier pantalla (V + 00-99 + Enter)
- Fondo único con **tinte rojo progresivo por nivel** y efecto **Matrix** en game over
- HUD centrado con vidas, puntos y volumen

## 🚀 Instalar y correr
```bash
git clone https://github.com/MrTejuinoFake/Breakout-Proyecto-252.git
cd Breakout-Proyecto-252
make
bin/Arkanoid.exe
```

## 🎮 Controles rápidos
- Menú: escribe `play` o `exit`
- Juego: ← → mover, **ESPACIO** lanzar, **ESC** menú
- Volumen: **V** → escribe 00-99 → **Enter** aplica (ESC cancela)

## 📂 Estructura mínima
```
assets/ (fonts, images, music)
include/ (Game, Ball, Paddle, Brick)
src/ (implementaciones + main)
docs/ DOCUMENTACION_COMPLETA.md
```

## 📚 Más detalle
Resumen largo en: **docs/DOCUMENTACION_COMPLETA.md**

## 👤 Autor
Proyecto 252 — [@MrTejuinoFake](https://github.com/MrTejuinoFake)