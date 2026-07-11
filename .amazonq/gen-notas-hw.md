# Genera notas de hardware (docs/notas.md)

## Objetivo
Crear un archivo `docs/notas.md` con la documentación técnica de hardware del proyecto actual.

## Instrucciones

1. **Analiza el proyecto**: Lee los archivos fuente del workspace para extraer:
   - Placa: Arduino Nano (ATmega328P)
   - Driver: A4988 (Allegro)
   - Motor: NEMA17 JK42HS40-1684 (1.68A, 3.6 kg·cm)
   - Periféricos: Potenciómetro 10kΩ, Switch SPDT, Pulsador N.A.
   - Voltajes: 5V lógica, 12V motor

2. **Genera el markdown** con la estructura:
   - Hardware (lista de componentes)
   - Conexión (tabla pin a pin)
   - Notas (quirks, gotchas, tips de montaje)

3. **Guarda** en `docs/notas.md`.

## Criterios de calidad
- Solo incluir información verificable desde el código o datasheets
- Máximo 30 líneas
- Incluir en Notas: capacitor obligatorio, VREF=1.34V, colores de cable variables
