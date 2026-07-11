# Posicionador Motorizado NEMA17 — Decisiones Técnicas

## Decisiones Tomadas

### D1: Arduino Nano en vez de ESP32
- **Decisión**: Usar ATmega328P (Arduino Nano)
- **Alternativas**: ESP32 DevKit, STM32 Blue Pill, Arduino Mega
- **Razón**: El proyecto NO requiere WiFi/BT. El Nano es económico (~6 USD), disponible localmente en Tarija, tiene suficiente I/O para el control básico, y el ecosistema Arduino es familiar para estudiantes de nivel técnico.
- **Trade-off**: Solo 2KB RAM (limitante para LCD + Serial simultáneos). Solo 2 interrupciones externas hardware.
- **Revisable si**: Se agrega control remoto, telemetría, o interfaz web.

### D2: Generación manual de pulsos STEP (no librería AccelStepper)
- **Decisión**: Generar pulsos con `micros()` + toggle manual en pin STEP
- **Alternativas**: AccelStepper library, Timer interrupt, analogWrite (PWM)
- **Razón**: AccelStepper agrega ~4KB de Flash y abstrae el timing de forma que dificulta el control preciso de la rampa. La generación manual permite entender exactamente qué pasa (objetivo educativo) y controlar la aceleración con fórmula propia. PWM (analogWrite) no sirve porque no permite cambiar frecuencia dinámicamente en ATmega328P sin manipular registros de timer.
- **Trade-off**: Más código propio, pero 100% transparente y educativo.
- **Revisable si**: Se necesitan perfiles de movimiento complejos (S-curve, multi-eje).

### D3: FSM con enum + switch en vez de booleanos
- **Decisión**: Máquina de estados explícita (IDLE, ACCEL, RUN, DECEL, ERROR)
- **Alternativas**: Variables bool (motorActivo, acelerando, etc.)
- **Razón**: Con booleanos, el código tiene combinaciones imposibles que no se detectan (ej: motorActivo=true && acelerando=true && decelerando=true). La FSM garantiza un solo estado válido a la vez y documenta TODAS las transiciones.
- **Referencia**: guia-crecimiento-de-codigo.md — "Máquina de estados si hay flujo complejo (FSM con enum + switch)"

### D4: Filtro de promedio móvil para ADC (no EMA)
- **Decisión**: Promedio móvil de 8 muestras para el potenciómetro
- **Alternativas**: Exponential Moving Average (EMA), Kalman, mediana
- **Razón**: El promedio móvil es simple de implementar, fácil de entender (proyecto educativo), y suficiente para un potenciómetro de panel que cambia lentamente. Con 8 muestras a ~100µs/lectura, el delay total es <1ms (despreciable vs los 500µs-5ms entre pasos).
- **Trade-off**: Respuesta ligeramente más lenta que EMA. Para un control manual con la mano, es imperceptible.

### D5: Aceleración lineal (no S-curve ni exponencial)
- **Decisión**: Rampa lineal: incrementar velocidad en cantidad fija cada 20ms
- **Alternativas**: S-curve (suave), exponencial (rápida al inicio)
- **Razón**: La carga del posicionador tiene inercia relativamente baja. Una rampa lineal de 500ms (0 a velocidad target) es suficiente para evitar pérdida de pasos. S-curve agrega complejidad matemática sin beneficio perceptible en este rango de velocidades (5-100 RPM en plato).
- **Parámetro clave**: `ACCEL_INTERVAL_MS = 20ms`, `ACCEL_STEP_US = (target-min)/25 ticks`

### D6: Reducción mecánica por poleas GT2 (3:1)
- **Decisión**: Poleas dentadas GT2, 20T motor / 60T plato
- **Alternativas**: Acoplamiento directo, engranajes rectos, tornillo sin fin
- **Razón**: GT2 es económica (~60-100 Bs total), no requiere mecanizado preciso, permite ajustar tensión fácilmente, y la relación 3:1 triplica el torque a ~10.8 kg·cm (suficiente para 5 kg centrado en horizontal).
- **Trade-off**: Velocidad máxima en plato se reduce a ~50 RPM (más que suficiente para posicionamiento).

### D7: VREF calculado no medido experimentalmente
- **Decisión**: Usar fórmula VREF = Imax × 8 × Rsense = 1.34V (A4988)
- **Alternativas**: Ajustar "hasta que no caliente", usar % del máximo
- **Razón**: La fórmula es la correcta según el datasheet del A4988. Ajustar "al ojo" lleva a sobrecorriente (destruye motor/driver) o subcorriente (pierde pasos).
- **Cálculo**: 1.68A × 8 × 0.100Ω = 1.344V → Ajustar potenciómetro del driver a 1.34V con multímetro.

## Constantes Críticas con Justificación

| Constante | Valor | Fórmula/Razón | Si ↑ | Si ↓ |
|:----------|:------|:--------------|:-----|:-----|
| STEP_DELAY_MIN_US | 500 | 60M/(RPM_max×2×pasos×µstep) = 60M/(300×2×200×1) | Motor más lento (max) | Motor más rápido → puede perder pasos |
| STEP_DELAY_MAX_US | 5000 | 60M/(RPM_min×2×pasos×µstep) = 60M/(30×2×200×1) | Motor más lento (min) | Motor muy rápido al mínimo del pot |
| ACCEL_INTERVAL_MS | 20 | 500ms/25 ticks = 20ms por tick | Aceleración más suave pero lenta | Aceleración más brusca |
| DEBOUNCE_MS | 50 | > rebote típico de pulsador (10-30ms) | Botón responde lento | Puede detectar falsos pulsos |
| ADC_FILTER_SIZE | 8 | Potencia de 2 para shift en vez de div | Más filtrado, más delay | Menos filtrado, más ruido |
| POT_DEAD_ZONE | 10 | ~1% del rango ADC (10/1023) | Más difícil arrancar | Motor arranca con leve toque |
| LCD_UPDATE_MS | 500 | 2 Hz es suficiente para leer (ojo humano) | Actualización más lenta | Consume más CPU, LCD parpadea |
| DIR_CHANGE_DELAY_MS | 5 | A4988 requiere >200ns entre DIR y STEP | Más seguro pero detiene más | Podría violar timing del driver |
