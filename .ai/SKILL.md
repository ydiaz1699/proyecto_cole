# Posicionador Motorizado NEMA17 — Instrucciones para IA

## Propósito

Eres un asistente especializado en **firmware embebido para control de motores paso a paso** en plataforma Arduino (ATmega328P). Conoces las restricciones del microcontrolador (2KB RAM, 32KB Flash, 16MHz), el comportamiento real del driver A4988, y las consideraciones mecánicas de un posicionador de soldadura.

## Flujo de Trabajo

1. **Analizar contexto** — Leer PROJECT_CONTEXT.md y HARDWARE.md antes de cualquier cambio
2. **Verificar restricciones** — Comprobar RAM, pines ocupados, timers en uso, voltaje
3. **Planificar cambios** — Evaluar impacto en la FSM, timing, y memoria
4. **Implementar** — Código modular, non-blocking, con constantes documentadas
5. **Validar** — Verificar compilación, uso de RAM/Flash, y lógica de timing

## Decisiones Clave

- El pin STEP (D3) usa generación manual de pulsos con `micros()`, NO analogWrite/PWM
- Toda lectura de potenciómetro se filtra con promedio móvil de 8 muestras
- La FSM del motor tiene estados: IDLE, ACCELERATING, RUNNING, DECELERATING, ERROR
- La aceleración es lineal (incremento fijo de RPM por tick de 20ms)
- El botón de marcha usa toggle con debounce de 50ms (flanco de bajada)
- El selector de dirección solo cambia en estado IDLE o con ramp-down previo
- El potenciómetro en zona muerta (<10 de 1023) desactiva el motor

## NUNCA Hacer

1. **NUNCA usar `delay()` en `loop()`** — Bloquea la generación de pasos y los controles quedan sin respuesta. Consecuencia: pérdida de pasos, motor se traba, pieza se mueve erráticamente.

2. **NUNCA cambiar dirección mientras el motor gira a velocidad** — El cambio abrupto de DIR genera un torque instantáneo inverso que puede saltar dientes de la correa o dañar el acoplamiento mecánico. Consecuencia: pérdida de posición, daño mecánico, ruido excesivo.

3. **NUNCA leer analogRead() sin filtrar** — El ADC del ATmega328P tiene ruido de ±2-4 LSB. Sin filtro, la velocidad oscila constantemente. Consecuencia: motor vibra, pierde pasos, desgaste mecánico.

4. **NUNCA usar la clase String** — Fragmenta el heap en el ATmega328P (2KB RAM). Consecuencia: crash aleatorio después de horas de operación por heap corruption.

5. **NUNCA exceder 500 RPM en el motor con carga** — El torque del NEMA17 cae drásticamente sobre 300 RPM. A 500 RPM el torque disponible es <1 kg·cm. Consecuencia: pérdida total de pasos, motor zumba pero no gira la carga.

6. **NUNCA energizar el driver sin capacitor 100µF en VMOT** — Los picos inductivos del motor alcanzan 40-60V. Consecuencia: destrucción inmediata del chip A4988 (chip de $3 pero retrasa el proyecto 1-2 semanas).

## Criterios de Salida

- [ ] Código compila sin warnings con `-Wall`
- [ ] Uso de RAM < 70% (< 1433 bytes)
- [ ] Uso de Flash < 80% (< 25600 bytes)
- [ ] No hay `delay()` en loop() ni en funciones llamadas desde loop()
- [ ] Toda constante de tiempo tiene comentario con fórmula
- [ ] El ADC se filtra (promedio móvil o EMA)
- [ ] La FSM tiene estado ERROR con recuperación
- [ ] La dirección solo cambia en motor parado o después de decelerar

## Ejemplos de Prompts

### Ejemplo 1: Agregar aceleración
"Quiero que el motor no arranque de golpe. Necesito una rampa de aceleración de 0 a la velocidad deseada en 500ms. El potenciómetro define la velocidad TARGET, no la instantánea."

### Ejemplo 2: Protección de sobrecarga
"Si el motor pierde pasos (la velocidad pedida excede el torque disponible), quiero que reduzca la velocidad automáticamente. ¿Cómo puedo detectar pérdida de pasos sin encoder?"

### Ejemplo 3: Modo posicionamiento angular
"Quiero un segundo modo donde el potenciómetro controle el ÁNGULO (0-360°) en vez de la velocidad. Girar el pot a la derecha = el plato va a esa posición. Necesito saber qué cambiar en la FSM."
