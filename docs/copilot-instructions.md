# Copilot Instructions — Posicionador Motorizado NEMA17

> Este archivo proporciona contexto inmediato para cualquier asistente IA
> que trabaje con el código de este proyecto. Contiene TODOS los datos
> necesarios para generar código correcto sin preguntar.

---

## Identidad del proyecto

- **Qué es**: Firmware para posicionador motorizado de soldadura
- **MCU**: Arduino Nano (ATmega328P, 16MHz, 2KB RAM, 32KB Flash)
- **Motor**: NEMA17 JK42HS40-1684 (1.68A, 200 pasos/vuelta, 3.6 kg·cm)
- **Driver**: A4988 (Allegro), Rsense=0.100Ω, VREF=1.34V
- **Framework**: Arduino (avr-gcc)
- **Versión**: 2.0.0 (Nivel 2 — Robusto)
- **Nivel educativo**: Proyecto de grado de colegio técnico (Bolivia)

---

## Datos clave del proyecto

| Constante | Valor | Fórmula |
|:----------|:------|:--------|
| STEP_DELAY_MIN_US | `500` µs | 60M/(300×2×200) — 300 RPM motor |
| STEP_DELAY_MAX_US | `5000` µs | 60M/(30×2×200) — 30 RPM motor |
| ACCEL_INTERVAL_MS | `20` ms | 500ms/25 ticks — intervalo entre ticks rampa |
| ACCEL_TICKS_TOTAL | `25` ticks | Número de pasos para rampa completa |
| ACCEL_STEP_US | `180` µs/tick | (5000-500)/25 — reducción por tick |
| ADC_FILTER_SIZE | `8` muestras | Promedio móvil, shift >>3 |
| ADC_READ_INTERVAL_MS | `10` ms | 100 Hz muestreo pot |
| POT_DEAD_ZONE_LOW | `10` | ~1% ADC — por debajo = motor OFF |
| POT_DEAD_ZONE_HIGH | `1013` | ~99% ADC — por encima = máximo |
| DEBOUNCE_MS | `50` ms | > rebote típico 10-30ms |
| DIR_CHANGE_DELAY_MS | `5` ms | Margen sobre req. A4988 (>200ns) |
| LCD_UPDATE_INTERVAL_MS | `500` ms | 2 Hz visual |
| SERIAL_REPORT_INTERVAL_MS | `2000` ms | 0.5 Hz debug |
| MOTOR_TIMEOUT_MS | `5000` ms | Timeout sin pasos → ERROR |
| ERROR_BLINK_MS | `250` ms | LED parpadeo en error |
| VREF A4988 | `1.34V` | 1.68A × 8 × 0.1Ω |
| Loop time sin LCD | `~140 µs` | analogRead 104µs + FSM 20µs + misc |
| Loop time con LCD update | `~1440 µs` | ¡MAYOR que STEP_DELAY_MIN! |
| RPM motor min/max | `30-300` | Con STEP_DELAY 5000-500 µs |
| RPM plato min/max | `10-100` | Con reducción GT2 3:1 |
| Reducción mecánica | `3:1` | Poleas GT2 20T/60T |
| Torque en plato | `~10.8 kg·cm` | 3.6 × 3 |


---

## Arquitectura del firmware

```
main.cpp ─── setup() + loop() (solo coordinación, CERO lógica)
    │
    ├── InputManager (input_manager.h/.cpp)
    │     ├── Filtrado ADC: buffer circular [8], promedio con >>3
    │     ├── Debounce botón: 50ms, flanco bajada, flag one-shot
    │     ├── Selector dirección: detección de cambio, flag one-shot
    │     └── Output: struct InputState {potRaw, targetDelay, btnStartPressed, dirSwitchState, dirChanged, potInDeadZone}
    │
    ├── MotorController (motor_controller.h/.cpp)
    │     ├── FSM: IDLE → ACCELERATING → RUNNING → DECELERATING → ERROR
    │     ├── Generación pasos: micros() toggle, non-blocking
    │     ├── Aceleración: lineal, 180µs/tick cada 20ms
    │     ├── Dirección: solo cambia en IDLE
    │     └── Output: struct MotorStatus {state, direction, currentDelay, targetDelay, currentRPM, enabled}
    │
    └── DisplayManager (display_manager.h/.cpp)
          ├── LCD (opcional): rate-limited 500ms, línea1=RPM+DIR, línea2=estado+barra
          └── Serial (debug): rate-limited 2000ms, formato "[STATUS] State=X | RPM=X | Dir=X | Pot=X | Delay=Xus"
```

---

## Pines (NO cambiar sin actualizar config.h Y hardware)

| Pin | Función | Módulo | Notas CRÍTICAS |
|:---:|:--------|:-------|:---------------|
| D3 | STEP | MotorController | Timer2 capable. **NUNCA tone()** |
| D4 | DIR | MotorController | Solo cambia en IDLE |
| D5 | ENABLE | MotorController | LOW=activo, HIGH=sleep |
| D7 | Switch dirección | InputManager | INPUT_PULLUP. HIGH=CW |
| D8 | Botón marcha | InputManager | INPUT_PULLUP. Toggle en flanco ↓ |
| D9 | MS1 | MotorController | LOW para paso completo |
| D10 | MS2 | MotorController | LOW para paso completo |
| D11 | MS3 | MotorController | LOW para paso completo |
| D13 | LED estado | MotorController | OFF=IDLE, ON=RUN, BLINK=ERROR |
| A0 | Potenciómetro | InputManager | Filtro 8 muestras, lectura cada 10ms |
| A1-A5, D2 | LCD (si habilitado) | DisplayManager | **Ocupados SOLO si LCD_ENABLED=1** |

---

## Reglas de implementación (INVIOLABLES)

### NUNCA hacer:

1. **NUNCA `delay()` en `loop()`** — Bloquea _generateStep(). El motor pierde pulsos STEP y la pieza se mueve erráticamente. Solo permitido en setup() y en _setDirection() (que solo se llama en IDLE).

2. **NUNCA `String` class** — 2KB RAM. Heap fragmentation inevitable. Crash aleatorio después de horas. Usar `char[]` con tamaño fijo y `F()` macro para literales Serial/LCD.

3. **NUNCA cambiar DIR mientras motor gira** — El cambio abrupto genera torque inverso instantáneo. Daña poleas GT2, salta dientes de correa. Solo cambiar en STATE_IDLE.

4. **NUNCA `tone()`** — Timer2 compartido con pin D3 (STEP). tone() reconfigura Timer2 y corrompe los pulsos del motor silenciosamente.

5. **NUNCA LCD a velocidad >100 RPM** — lcd.print() bloquea 1.3ms (32 chars × 40µs). STEP_DELAY_MIN=500µs. El bloqueo es MAYOR que el intervalo entre pasos → pierde pasos.

6. **NUNCA `analogRead()` sin filtrar** — ADC del ATmega328P tiene ruido ±2-4 LSB. Sin filtro, la velocidad oscila constantemente → motor vibra, pierde pasos, desgaste mecánico.

7. **NUNCA exceder 300 RPM motor (500 RPM absoluto)** — El torque NEMA17 cae drásticamente. A 500 RPM: <1 kg·cm disponible. Motor zumba pero no gira la carga.

8. **NUNCA energizar driver sin capacitor 100µF en VMOT** — Picos inductivos de 40-60V destruyen el chip A4988 instantáneamente.

9. **NUNCA conectar/desconectar motor con driver energizado** — Back-EMF destruye el A4988.


### SIEMPRE hacer:

1. **SIEMPRE non-blocking** — Usar `micros()` para timing de STEP, `millis()` para todo lo demás. Comparar con `>=` (no `==`).

2. **SIEMPRE filtrar ADC** — Promedio móvil con buffer circular. Usar shift (`>>3`) en vez de división para ATmega.

3. **SIEMPRE usar `F()` macro** — Para TODOS los strings en Serial.print() y lcd.print(). Ejemplo: `Serial.println(F("[FSM] IDLE -> ACCEL"));`

4. **SIEMPRE documentar constantes** — Cada valor numérico debe tener comentario con fórmula. Formato: `// 60M/(300×2×200) = 500µs`

5. **SIEMPRE usar FSM** — Nuevo comportamiento = nuevo estado o nueva transición. NUNCA agregar booleanos sueltos para control de flujo.

6. **SIEMPRE verificar estado antes de cambiar DIR** — `if (_status.state == STATE_IDLE)` antes de cualquier cambio de dirección.

7. **SIEMPRE deshabilitar motor en IDLE y ERROR** — `digitalWrite(PIN_ENABLE, HIGH)` para ahorrar energía y evitar calentamiento.

---

## FSM — Transiciones válidas

```
IDLE ──[btn + pot>deadzone]──→ ACCELERATING
ACCELERATING ──[delay<=target]──→ RUNNING
ACCELERATING ──[btn || pot=0]──→ DECELERATING
RUNNING ──[btn]──→ DECELERATING
RUNNING ──[pot=0]──→ DECELERATING
RUNNING ──[target<current]──→ ACCELERATING (quiere más velocidad)
DECELERATING ──[delay>=MAX]──→ IDLE
ERROR ──[btn]──→ IDLE (recuperación)

Cualquier estado ──[timeout 5s sin pasos]──→ ERROR (no implementado en v2.0)
```

**Transiciones PROHIBIDAS:**
- IDLE → RUNNING (debe pasar por ACCEL)
- RUNNING → IDLE (debe pasar por DECEL)
- Cualquier estado → cambio de DIR (solo en IDLE)

---

## Patrones de código del proyecto

### Lectura filtrada (patrón buffer circular):
```cpp
// En _readPotFiltered():
_adcSum -= _adcBuffer[_adcIndex];        // Restar muestra vieja
_adcBuffer[_adcIndex] = reading;          // Guardar nueva
_adcSum += reading;                       // Sumar nueva
_adcIndex = (_adcIndex + 1) & (ADC_FILTER_SIZE - 1);  // Circular con AND (no módulo)
_state.potRaw = _adcSum >> ADC_FILTER_SHIFT;           // Promedio con shift (no /)
```

### Debounce (patrón flanco con timer):
```cpp
// En _readButton():
if (rawState != _lastBtnRaw) {
    _btnDebounceTime = millis();        // Reset timer si cambió
}
if ((millis() - _btnDebounceTime) > DEBOUNCE_MS) {
    if (rawState != _btnFiltered) {
        _btnFiltered = rawState;
        if (_btnFiltered == LOW) {       // Flanco bajada = presión
            _state.btnStartPressed = true;  // ONE-SHOT flag
        }
    }
}
```

### Generación de pasos (patrón toggle non-blocking):
```cpp
// En _generateStep():
unsigned long now = micros();
if (now - _lastStepTime >= _status.currentDelay) {
    _lastStepTime = now;
    _stepPinState = !_stepPinState;
    digitalWrite(PIN_STEP, _stepPinState ? HIGH : LOW);  // TOGGLE
}
```

### Rate limiting (patrón para display/serial):
```cpp
// En _reportSerial():
unsigned long now = millis();
if (now - _lastSerialReport < SERIAL_REPORT_INTERVAL_MS) return;  // No es tiempo
_lastSerialReport = now;
// ... hacer el trabajo costoso aquí ...
```


---

## Cómo agregar funcionalidad nueva

### Agregar un nuevo sensor (ejemplo: temperatura):
1. Definir pin en `config.h`: `static const uint8_t PIN_TEMP = A1;`
2. Agregar lectura en `InputManager` (con filtrado si es analógico)
3. Agregar campo a `InputState`: `float temperature;`
4. En `MotorController`: usar dato para transición a ERROR si excede límite
5. En `DisplayManager`: mostrar en Serial report

### Agregar un nuevo estado FSM:
1. Agregar valor a enum `MotorState` en `config.h`
2. Agregar case en `MotorController::update()` switch
3. Crear handler `_handleNuevoEstado(const InputState& inputs)`
4. Definir transiciones de entrada y salida
5. Actualizar `_reportSerial()` con nombre del nuevo estado

### Agregar un nuevo modo de operación (ejemplo: angular):
1. Agregar enum `OperationMode` en `config.h`: `MODE_SPEED, MODE_ANGLE`
2. Agregar selector de modo (segundo switch o combinación de botones)
3. En `InputManager`: leer selector de modo
4. En `MotorController`: branch en FSM según modo
5. Modo angular: pot controla posición (0-360°), no velocidad

---

## Memoria disponible

| Recurso | Usado (sin LCD) | Disponible | Límite seguro (70%) |
|:--------|:---------------:|:----------:|:-------------------:|
| Flash | ~8 KB | 32 KB | 25.6 KB |
| RAM | ~400 bytes | 2048 bytes | 1433 bytes |

**Presupuesto por feature nueva:**
- Variable global `uint16_t`: 2 bytes RAM
- Variable global `float`: 4 bytes RAM
- Buffer `uint16_t[8]`: 16 bytes RAM
- String con `F()`: 0 bytes RAM (va a Flash)
- String SIN `F()`: N bytes RAM ← EVITAR
- Función nueva: ~50-200 bytes Flash

---

## Compilación y upload

```bash
# Arduino IDE:
# Board: Arduino Nano
# Processor: ATmega328P (o ATmega328P Old Bootloader)
# Port: COM correspondiente
# Upload speed: 115200

# PlatformIO:
pio run -e nano                    # Compilar
pio run -e nano --target upload    # Subir
pio device monitor --baud 9600     # Monitor serial
```

### Build flags:
```
-DLCD_ENABLED=0      → Sin LCD (por defecto, RECOMENDADO)
-DLCD_ENABLED=1      → Con LCD (solo si vel<100 RPM)
-DDEBUG_SERIAL=1     → Con reportes Serial (por defecto)
-DDEBUG_SERIAL=0     → Sin Serial (ahorra ~2KB Flash, ~50 bytes RAM)
```

---

## Contexto del proyecto educativo

- Es un **proyecto de grado** de colegio técnico (6to Secundaria, Bolivia)
- Los estudiantes tienen nivel técnico intermedio (soldadura, electricidad, mecánica)
- El código debe ser **entendible** — preferir claridad sobre optimización extrema
- Los comentarios en español son bienvenidos y esperados
- El proyecto integra 4 áreas: soldadura + electricidad + mecánica + programación
- Presupuesto limitado (~596 Bs ≈ 86 USD) — no recomendar componentes caros
- Componentes deben estar disponibles localmente en Tarija, Bolivia

---

## Archivos de referencia

| Necesitas... | Lee este archivo |
|:-------------|:-----------------|
| Entender el proyecto completo | `.ai/PROJECT_CONTEXT.md` |
| Ver restricciones hardware/pines | `.ai/HARDWARE.md` |
| Entender por qué se tomó una decisión | `.ai/DECISIONS.md` |
| Ver qué falta por hacer | `.ai/TASKS.md` |
| Entender la FSM y módulos | `.ai/ARCHITECTURE.md` |
| Ver el plan futuro | `.ai/ROADMAP.md` |
| Datos exhaustivos del sistema | `archivo-mapa.yml` |
| Procedimientos de calibración | `docs/notas.md` |
| Cálculos mecánicos/eléctricos | `calculos/calculos_tecnicos.md` |
| Análisis profundo del timing | `analisis_posicionador.md` |

---

*Generado desde código real de src/ — NO inventado.*
*Actualizar si cambian constantes, pines o arquitectura.*
