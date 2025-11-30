# Archivos de Audio

Esta carpeta debe contener los siguientes archivos de audio:

1. **bounce.mp3** - Sonido de rebote de la pelota (corto, ~0.1-0.2 segundos)
   - Se reproduce cuando la pelota rebota contra paredes, paddle o ladrillos
   - Volumen configurado al 70%

2. **background.mp3** - Música de fondo durante el juego (loop continuo)
   - Se reproduce solo durante el juego activo
   - Se detiene automáticamente en game over
   - Volumen configurado al 30%

3. **menu.mp3** - Música del menú principal (loop continuo) 
   - Se reproduce automáticamente al iniciar el juego
   - Se reproduce al regresar desde game over
   - Se detiene al comenzar el juego
   - Volumen configurado al 40%

4. **gameover.mp3** - Sonido de game over (una sola vez, ~1-3 segundos)
   - Se reproduce una vez al perder todas las vidas
   - Efecto dramático de final de juego
   - Volumen configurado al 80%

## Formatos soportados por SFML:
- OGG Vorbis (.ogg) - Recomendado para juegos
- WAV (.wav) - Sin compresión, buena calidad
- FLAC (.flac) - Sin pérdidas
- MP3 (.mp3) - Con compresión

## Recomendaciones:
- Usar archivos cortos para efectos de sonido (< 1 segundo)
- Usar música en loop para el fondo (2-3 minutos)
- Mantener el volumen balanceado entre efectos y música