# Skill: Posicionador Motorizado NEMA17

## Propósito

Esta skill define cómo generar o corregir código para **control de motor paso a paso NEMA17 con Arduino Nano y driver A4988**. El objetivo es mantener un sistema de posicionamiento de soldadura con velocidad regulable, dirección reversible y arranque/parada gradual.

## Flujo de trabajo

1. **Detectar la intención**
   - El usuario pide agregar funcionalidad al motor, sensores, o display.
   - La skill debe respetar la FSM existente y las restricciones de timing.

2. **Configurar el proyecto**
   - Plataforma: `Arduino Nano (ATmega328P)`.
   - Dependencias: `Arduino Core AVR, LiquidCrystal (opcional)`.
   - Estructura: `src/main.cpp, src/config.h, src/motor_controller.h/.cpp, src/input_manager.h/.cpp, src/display_manager.h/.cpp`.

3. **Construir la lógica principal**
   - Leer entradas filtradas (InputManager: pot + botón + switch).
   - Actualizar FSM del motor (MotorController: IDLE→ACCEL→RUN→DECEL→ERROR).
   - Generar pasos STEP con toggle non-blocking cada `currentDelay` µs.
   - Actualizar display con rate limiting (DisplayManager: cada 2s Serial, 500ms LCD).

4. **Controlar entradas/salidas**
   - Entradas: Potenciómetro A0 (velocidad 0-1023), Switch D7 (dirección CW/CCW), Pulsador D8 (marcha/parada toggle).
   - Salidas: D3 (STEP pulsos), D4 (DIR nivel), D5 (ENABLE LOW=activo), D13 (LED estado).
   - Estados: IDLE (motor deshabilitado), ACCELERATING (rampa up), RUNNING (vel constante), DECELERATING (rampa down), ERROR (LED parpadea, motor off).

5. **Mantener estilo y calidad**
   - Comentarios y nombres de variables en español/inglés mixto (comentarios español, código inglés).
   - Mantener código claro y simple.
   - No introducir lógica innecesaria ni dependencias extras.
   - Asegurarse de que compile con `pio run -e nano`.

## Decisiones clave

- Generación manual de pulsos STEP con `micros()` toggle (NO AccelStepper library) — por transparencia educativa.
- FSM con enum+switch en vez de booleanos — previene combinaciones de estado imposibles.
- Filtro promedio móvil de 8 muestras con shift >>3 — más eficiente que EMA para este caso.
- `setRepeatTransmit()` no aplica aquí (eso es RF) — este proyecto es control local directo.
- Aceleración lineal (no S-curve) — suficiente para la carga y simplifica la matemática.

## Criterios de salida

La respuesta debe incluir:
- Código completo y funcional para el archivo modificado.
- Valores actualizados para constantes en `config.h` si se cambian.
- Breve explicación de los cambios y el comportamiento.
- Comentarios en español.
- Verificación de que no excede 70% RAM ni 80% Flash.
- Nada irrelevante.

## Ejemplos de prompts

- "Agrega un modo de posicionamiento angular donde el pot controle el ángulo (0-360°) en vez de la velocidad."
- "El motor pierde pasos al arrancar con 5kg de carga. Ajusta la rampa de aceleración para que sea más suave."
- "Agrega un buzzer en D6 que suene 3 beeps al entrar en estado ERROR (sin usar tone())."

## Notas adicionales

- Esta skill es específica del repositorio `proyecto_cole` y debe conservar la estructura modular (4 módulos + config.h).
- Si hay un archivo `docs/copilot-instructions.md` o `.amazonq/copilot-instructions.md`, se debe usar como referencia para valores actuales y formato del proyecto.
