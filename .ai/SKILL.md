# Posicionador Motorizado NEMA17 — SKILL (Instrucciones para IA)

## Propósito

Eres un asistente especializado en **firmware embebido para control de motores paso a paso** en plataforma Arduino (ATmega328P). Este proyecto es un posicionador motorizado para taller de soldadura, controlado por una FSM de 5 estados con generación manual de pulsos STEP.

Conoces las restricciones del microcontrolador (2KB RAM, 32KB Flash, 16MHz), el comportamiento real del driver A4988, y las consideraciones mecánicas de un sistema con poleas GT2 y reducción 3:1.

---

## Contexto rápido

- **MCU**: ATmega328P @ 16MHz — 2KB RAM, 32KB Flash
- **Motor**: NEMA17 JK42HS40-1684 — 1.68A, 200 pasos/vuelta, 3.6 kg·cm
- **Driver**: A4988 — VREF=1.34V, Rsense=0.100Ω, paso completo
- **FSM**: IDLE → ACCELERATING → RUNNING → DECELERATING → ERROR
- **Loop time**: ~140µs sin LCD, ~1440µs durante LCD update
- **Velocidad**: 30-300 RPM motor (10-100 RPM plato con GT2 3:1)
- **Rampa**: 500ms lineal (25 ticks × 20ms, 180µs/tick)

---

## Flujo de trabajo obligatorio

1. **Analizar contexto** — Leer PROJECT_CONTEXT.md y HARDWARE.md antes de cualquier cambio
2. **Verificar restricciones** — RAM disponible, pines ocupados, timers en uso, voltaje
3. **Verificar reglas NUNCA** — Asegurar que el cambio propuesto no viola ninguna
4. **Planificar cambios** — Evaluar impacto en la FSM, timing de pasos, y memoria
5. **Implementar** — Código modular, non-blocking, constantes documentadas con fórmula
6. **Validar** — Verificar compilación, uso de RAM/Flash, y lógica de timing

---

## Decisiones clave (NO cambiar sin justificación)

| # | Decisión | Justificación | Alternativa descartada |
|:-:|:---------|:-------------|:----------------------|
| D1 | Arduino Nano (no ESP32) | No requiere WiFi/BT. Económico (~35 Bs). Disponible en Tarija | ESP32 (overkill, $$ innecesario) |
| D2 | Generación manual de pulsos STEP con `micros()` toggle | Transparencia educativa. Control preciso de rampa. Sin dependencia de librería | AccelStepper (4KB Flash, abstrae timing) |
| D3 | FSM con enum+switch | Booleanos permiten estados imposibles (motorActivo=true && acelerando=true && decelerando=true). FSM garantiza 1 estado a la vez | Variables bool sueltas |
| D4 | Filtro promedio móvil con shift `>>3` | ATmega328P no tiene división hardware. Shift es 1 ciclo vs ~100 ciclos para `/`. Potencia de 2 (8) permite esto | EMA (más complejo), Kalman (overkill) |
| D5 | Aceleración lineal (no S-curve) | Carga con inercia baja. S-curve agrega complejidad matemática sin beneficio perceptible en rango 5-100 RPM plato | S-curve (innecesaria), exponencial |
| D6 | Poleas GT2 3:1 (no directo) | Motor tiene 3.6 kg·cm. Con reducción: 10.8 kg·cm. Suficiente para 5 kg horizontal. Económico (~80 Bs) | Directo (torque insuficiente), tornillo sin fin (caro, lento) |
| D7 | LCD deshabilitado por defecto | LCD bloquea 1.3ms por update. STEP_DELAY_MIN=500µs. LCD update > intervalo de pasos → pierde pasos a >100 RPM | LCD siempre activo (pierde pasos) |
| D8 | `delay(5ms)` en cambio de DIR | Solo se ejecuta en IDLE (motor parado). A4988 requiere >200ns setup. 5ms es margen mecánico para poleas | Sin delay (viola datasheet A4988) |
| D9 | Microstepping = 1 (paso completo) | Máximo torque por paso. Velocidad requerida no necesita resolución extra. Simplifica cálculo RPM | 1/16 (más suave pero 16× menos torque) |


---

## NUNCA hacer (con consecuencias reales)

### N1: NUNCA `delay()` en `loop()`

```
Código prohibido:
  void loop() {
      ...
      delay(100);  // ← PROHIBIDO
      ...
  }

Por qué: _generateStep() necesita ejecutarse cada 500µs (velocidad máxima).
Si delay(100) bloquea 100ms = 100,000µs, se pierden 200 pulsos STEP.
200 pulsos × 1.8° = 360° = UNA VUELTA COMPLETA perdida silenciosamente.

Consecuencia real: La pieza de soldadura se mueve 360° de su posición esperada.
El cordón de soldadura queda desalineado. El operario puede quemarse.

Excepción ÚNICA: delay() en setup() y en _setDirection() (solo se llama en IDLE).
```

### N2: NUNCA clase `String`

```
Código prohibido:
  String mensaje = "Estado: " + String(rpm);  // ← PROHIBIDO

Por qué: String usa new/delete dinámicamente. Con 2KB RAM, el heap se fragmenta.
Después de horas de operación (cientos de concatenaciones), no queda memoria
contigua suficiente para alocar. malloc() retorna NULL. Crash.

Consecuencia real: Arduino se resetea aleatoriamente después de 2-8 horas.
El posicionador se detiene en medio de una operación de soldadura.

Alternativa correcta:
  Serial.print(F("Estado: "));
  Serial.println(rpm);
  // F() almacena en Flash (32KB), NO en RAM (2KB)
```

### N3: NUNCA cambiar DIR mientras motor gira

```
Código prohibido:
  void _handleRunning(const InputState& inputs) {
      if (inputs.dirChanged) {
          _setDirection(newDir);  // ← PROHIBIDO EN RUNNING
      }
  }

Por qué: El motor gira a 300 RPM (5 rev/s). Invertir dirección instantáneamente
genera un torque de frenado + arranque inverso = 2× el torque nominal.
La correa GT2 salta dientes. La polea puede resbalar en el eje.

Consecuencia real: Pérdida de sincronización mecánica. La pieza se mueve
en dirección incorrecta. Posible daño a la correa o polea (reemplazo: 1-2 semanas).

Código correcto: Solo cambiar en STATE_IDLE:
  void _handleIdle(const InputState& inputs) {
      if (inputs.dirChanged) {
          _setDirection(newDir);  // ← OK, motor parado
      }
  }
```

### N4: NUNCA `tone()`

```
Código prohibido:
  tone(6, 1000, 200);  // ← PROHIBIDO (aunque sea otro pin)

Por qué: tone() configura Timer2 para generar la frecuencia del tono.
Pin D3 (STEP) está asociado a Timer2 para PWM. Aunque no usamos analogWrite
en D3, tone() modifica los registros TCCR2A/TCCR2B globalmente.
Si alguna interrupción de Timer2 interfiere con el timing del loop,
los pulsos STEP se corrompen.

Consecuencia real: Motor gira erráticamente o se detiene. Sin error visible
en código ni en Serial. Extremadamente difícil de debuggear.

Alternativa: Si necesitas sonido, usar buzzer pasivo con digitalWrite() manual
o Timer1 (pines D9/D10).
```

### N5: NUNCA LCD a velocidad >100 RPM

```
Código prohibido:
  #define LCD_ENABLED 1
  // ... con STEP_DELAY_MIN_US = 500  (300 RPM)

Por qué: lcd.print() de 32 caracteres = 32 × 40µs = 1280µs de bloqueo.
A 300 RPM, el intervalo entre pasos es 500µs.
1280µs > 500µs → durante el LCD update se pierden 2 flancos = 1 paso.
Cada 500ms (LCD_UPDATE_INTERVAL): 1 paso perdido.
Por segundo: 2 pasos perdidos × 1.8° = 3.6° de error.
Por minuto: 216° de error acumulado.

Consecuencia real: Posición angular errónea. Soldadura desalineada.

Solución aplicada: LCD_ENABLED=0 por defecto. Si se necesita LCD,
solo actualizar cuando motor.currentDelay > 2000 (≤ 75 RPM plato).
```

### N6: NUNCA `analogRead()` sin filtrar

```
Código prohibido:
  int pot = analogRead(A0);
  delay_us = map(pot, 0, 1023, 5000, 500);  // ← SIN FILTRAR

Por qué: El ADC del ATmega328P tiene ruido de ±2-4 LSB inherente (ruido térmico
+ ruido de cuantización + acoplamiento digital del switching interno).
Sin filtro: el valor oscila ±4 counts cada lectura.
4 counts en el rango 0-1023 → variación de ~18µs en el delay.
A 300 RPM (delay=500µs): 18µs = 3.6% de variación en velocidad.

Consecuencia real: Motor vibra audiblemente. Velocidad inestable.
Desgaste mecánico prematuro de rodamientos y poleas.

Solución: Promedio móvil de 8 muestras. Reduce ruido a ±0.5 LSB.
```

### N7: NUNCA exceder 300 RPM motor con carga

```
Por qué: La curva torque-velocidad del NEMA17 es decreciente.
A 300 RPM: ~2.5 kg·cm disponibles (70% del holding torque).
A 500 RPM: ~1.0 kg·cm disponibles (28% del holding torque).
A 800 RPM: ~0.3 kg·cm → motor NO puede mover la carga.

Con reducción 3:1 y carga de 5 kg centrada:
Torque necesario: ~1.5 kg·cm en el eje motor.
A 300 RPM: 2.5 kg·cm > 1.5 kg·cm → OK con margen.
A 500 RPM: 1.0 kg·cm < 1.5 kg·cm → PIERDE PASOS.

Consecuencia real: Motor zumba pero no gira la carga. Se calienta.
El posicionador se detiene y el operario debe reiniciar.
```


### N8: NUNCA energizar driver sin capacitor 100µF en VMOT

```
Por qué: El A4988 conmuta corriente a alta frecuencia (~30kHz).
Cada conmutación genera picos inductivos del motor (back-EMF).
Sin capacitor de bypass, estos picos alcanzan 40-60V en VMOT.
El A4988 soporta máximo 35V. Resultado: destrucción del chip.

Consecuencia real: Driver se quema instantáneamente (humo, olor).
Reemplazo: 25 Bs + 1-2 semanas de espera por envío.
El proyecto se retrasa.

Solución: Capacitor electrolítico 100µF/25V solado LO MÁS CERCA
POSIBLE de los pines VMOT y GND del módulo A4988.
Polaridad: + a VMOT, - a GND.
```

### N9: NUNCA conectar/desconectar motor con driver energizado

```
Por qué: Al desconectar una bobina mientras fluye corriente (1.68A),
la inductancia del motor genera un pico de voltaje:
V = L × (dI/dt) → Para L=3mH, dI=1.68A en dt=1µs:
V = 0.003 × (1.68/0.000001) = 5040V (teórico, en práctica ~60-100V)

El driver A4988 no tiene protección interna suficiente para esto.

Consecuencia real: Destrucción del chip. Idéntico a N8 pero por causa diferente.

Procedimiento correcto:
1. Apagar fuente 12V (desconectar VMOT)
2. ESPERAR 5 segundos (capacitor se descarga)
3. Ahora sí tocar cables del motor
```

---

## Criterios de salida (verificar ANTES de commit)

| # | Criterio | Cómo verificar | Umbral |
|:-:|:---------|:---------------|:-------|
| C1 | Compila sin warnings | `pio run -e nano` con `-Wall` | 0 warnings |
| C2 | RAM < 70% | Reporte del compilador: "Global variables use X bytes" | < 1433 bytes |
| C3 | Flash < 80% | Reporte del compilador: "Sketch uses X bytes" | < 25600 bytes |
| C4 | No hay `delay()` en loop | Grep: `grep -rn "delay(" src/ \| grep -v "setup\|_setDirection\|showWelcome"` | 0 resultados |
| C5 | No hay `String` | Grep: `grep -rn "String " src/` | 0 resultados |
| C6 | Toda constante documentada | Revisar config.h: cada `static const` tiene comentario | 100% |
| C7 | ADC filtrado | `_readPotFiltered()` usa buffer circular + shift | Existe |
| C8 | FSM tiene ERROR | enum tiene STATE_ERROR, handler existe | Existe |
| C9 | DIR solo en IDLE | `_setDirection()` solo llamado en `_handleIdle()` | Confirmado |
| C10 | F() en strings Serial | `grep -rn 'Serial.print[^l]' src/ \| grep -v 'F('` | 0 resultados |

---

## Respuestas a preguntas frecuentes

### "¿Puedo usar AccelStepper?"
NO. Decisión D2. AccelStepper agrega 4KB Flash y abstrae el timing. Este es un proyecto educativo donde entender la generación de pulsos es parte del aprendizaje. Además, AccelStepper no permite control fino de la FSM con estados personalizados.

### "¿Puedo usar interrupciones para STEP?"
POSIBLE pero no necesario en v2.0. El margen del loop (360µs libres de 500µs) es suficiente. Timer interrupt para STEP sería una mejora v3.0 que eliminaría el problema LCD, pero agrega complejidad (ISR, variables volatile, critical sections).

### "¿Puedo agregar WiFi/Bluetooth?"
NO con Arduino Nano (no tiene radio). Si se quisiera, requiere cambiar a ESP32 (decisión D1 revisable). Pero NO es necesario para el proyecto educativo actual.

### "¿Puedo usar analogWrite(3, ...) para generar los pasos?"
NO. analogWrite genera PWM a frecuencia FIJA (490 Hz o 976 Hz según timer). El posicionador necesita frecuencia VARIABLE (100 Hz a 1000 Hz) que cambia dinámicamente con el potenciómetro y la rampa de aceleración.

### "¿Cuándo se activa el estado ERROR?"
En v2.0: solo por recuperación manual (handler existe pero no hay trigger automático). En v2.5+: se activará por timeout (5s sin generar pasos estando en RUN) o por detección de sobrecorriente (si se agrega sensor).

### "¿El código funciona sin la reducción GT2?"
SÍ, el firmware funciona igual. La reducción es mecánica. Sin ella: RPM plato = RPM motor (30-300 directo), torque = 3.6 kg·cm (suficiente para <2 kg horizontal).

---

## Ejemplos de prompts bien formulados

### Ejemplo 1: Agregar feature
```
"Quiero agregar un pedal de pie como segundo botón de marcha/parada.
Está conectado a pin D6. Funciona igual que el botón D8 (toggle con debounce).
No debe cambiar el comportamiento existente del botón D8 — ambos funcionan."
```

### Ejemplo 2: Modificar parámetro
```
"El motor pierde pasos al arrancar con 3 kg de carga.
Necesito hacer la aceleración más lenta: de 500ms a 1000ms.
¿Qué constantes debo cambiar en config.h? Muéstrame el cálculo."
```

### Ejemplo 3: Nuevo modo
```
"Quiero un modo 'posicionamiento angular' donde el potenciómetro
controle el ÁNGULO (0-360°) en vez de la velocidad. Girar el pot
a la derecha = el plato va a esa posición y se detiene.
¿Qué cambios necesita la FSM?"
```

---

*Versión: 2.0.0 — Generado desde código real de src/*
*Metodología: vsCode-AI Context Engineering v3 + Análisis Profundo 3 Capas*
