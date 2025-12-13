# 🎮 Arkanoid - Proyecto 252

> Breakout modernizado con menú estilo terminal, audio completo y efecto Matrix.(pero sera solo un arkanoid clasico?)



[crasheos intencioanles meintras se sigue los dialogos esperados, por lo que se debera de reabrir el juego al menos 3 veces para continuar con la historia]

## 
- **Comandos**: PLAY / EXIT (menú), REBOOT / EXIT (game over)
- **Controles**: ← → mover, ESPACIO lanzar, V volumen (00-99 + Enter), ESC salir/menú
- **Bloques**: normales (10), rojos x2 (20), morados x3 (100), amarillos speed +10% (50)
- **Audio**: música de menú/juego + FX rebote + game over, volumen maestro en vivo
- **Efectos**: fondo con tinte por nivel, Matrix en Game Over

## 🎯 Qué tiene de especial
- Dialogos que generan una pequeña historia(ver mas abajo bajo riesgo de spoiler)
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
- Menú: escribe `play` o `exit` o `controls`
- Juego: ← → mover, **ESPACIO** lanzar, **ESC** menú
- Volumen: **V** → escribe 00-99 → **Enter** aplica (ESC cancela)

## 📂 Estructura mínima
```
assets/ (fonts, images, music)
include/ (Game, Ball, Paddle, Brick)
src/ (implementaciones + main)
docs/ DOCUMENTACION_COMPLETA.md
```
## 🧩 Créditos y Recursos Externos

Este proyecto utiliza recursos multimedia de terceros bajo licencias de uso libre, propósitos educativos o generados por IA.

### 🖼️ Recursos Visuales
* **Referencia de Pantalla Azul (BSOD):** Basado en la documentación oficial de solución de problemas de [Dell Support](https://www.dell.com/support/contents/es-ky/article/product-support/self-support-knowledgebase/fix-common-issues/blue-screen).

### 🎵 Música (Banda Sonora)
Música obtenida de [Pixabay](https://pixabay.com/music/) (Royalty Free).

**Pistas principales:**
* **Killing Battle Theme (キリングバトルテーマ)** por *yoshiyuki_tatsuya*
* **Arms Ready (武器の準備)** por *yoshiyuki_tatsuya*
* **Action Heavy Metal Loop** por *Emmraan*
* **The Return of the 8-Bit Era** por *DJARTMUSIC*
* **I Love My 8-Bit Game Console** por *DJARTMUSIC*
* **Best Game Console** por *DJARTMUSIC*
* *Y otras pistas ambientales cortesía de la comunidad de Pixabay.*

### 🔊 Efectos y Voces
* **SFX (Sonidos de sistema):** Efectos de sonido (ej. USB) obtenidos de [MyInstants](https://www.myinstants.com/).
* **Voces (TTS):** Narración sintetizada mediante inteligencia artificial con [ElevenLabs](https://elevenlabs.io/).


## 📚 Más detalle
Resumen largo en: **docs/DOCUMENTACION_COMPLETA.md**

## 👤 Equipo
Proyecto 252 — [@MrTejuinoFake](https://github.com/MrTejuinoFake)

Líder: Diego Ivan Sandoval Gutierrez (@Diegoo135)
Integrante 2: Omar Ankin Rivas Zarate (@MrTejuinoFake)

## Trama

SPOILER




esta inspirado en la termianl ms dos por lo mismo son tres hermanos, uno,dos y tres, tres la encargada de la seguridad , dos es la que trabaja y tiene control sobre la pc por lo que se comunicara cntigo por medio de logs en la carpeta main.por lo que es necesaria prestar especial atencion a los audios de dos y de tres, esperemos les guste que quisimos hacer algo un poco distinto a un juego block breaker.