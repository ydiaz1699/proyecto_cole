# Análisis de Código: Posicionador Motorizado NEMA17

> Fecha: 2025-07-11
> Capa alcanzada: 2 (Dinámico — source de librerías + cálculos de timing)
> Estado: EN PROGRESO — Requiere calibración con hardware real (Capa 3)
> Archivos analizados: src/main.cpp, src/config.h, src/motor_controller.cpp, src/input_manager.cpp, src/display_manager.cpp

---

## Resumen Ejecutivo

Se diseñó un sistema de control de motor paso a paso NEMA17 para posicionador de soldadura usando Arduino Nano. El análisis profundo revela que el sistema es viable con las constantes calculadas, pero existen **3 puntos críticos** que requieren calibración con hardware real: (1) el timing del `analogRead()` compite con la generación de pasos a velocidades altas, (2) la aceleración lineal puede ser insuficiente con carga pesada + reducción, (3) el desbordamiento de `micros()` cada ~70 minutos requiere manejo.

---

## 1. Mapa del Sistema

### Componentes

| Componente | Tipo | Archivo(s) | Responsabilidad |
|:-----------|:-----|:-----------|:----------------|
| InputManager | Módulo SW | input_manager.h/.cpp | Filtrado ADC, debounce, selector |
| MotorController | Módulo SW (FSM) | motor_controller.h/.cpp | Estados, aceleración, generación STEP |
| DisplayManager | Módulo SW | display_manager.h/.cpp | LCD + Serial con rate limiting |
| Config | Header | config.h | Constantes con fórmulas documentadas |
| A4988 | HW (driver) | — | Amplificación de señal STEP/DIR a corriente del motor |
| NEMA17 | HW (motor) | — | Conversión eléctrica → mecánica (rotación) |
| Potenciómetro | HW (sensor) | — | Divisor de voltaje para lectura analógica |
| Botón/Switch | HW (entrada) | — | Señales digitales de control del usuario |

### Flujo de datos

```
[Pot 10kΩ]──analogRead()──►[ADC 10bit]──filtro 8 muestras──►[potRaw 0-1023]
       │
       └──map()──►[targetDelay µs]──►[MotorController FSM]
                                              │
[Botón]──debounce 50ms──►[btnStartPressed]────┘
                                              │
[Switch]──dirChanged──►[dirección CW/CCW]─────┘
                                              │
                                              ▼
                                    [currentDelay µs]
                                              │
                                              ▼
                              [micros() toggle]──►[PIN_STEP]──►[A4988]──►[NEMA17]
                                              │
                                              ▼
                                     [currentRPM]──►[DisplayManager]──►[LCD/Serial]
```

### Suposiciones implícitas detectadas

| # | Suposición del código | ¿Es verdad? | Consecuencia si es falsa |
|:-:|:---------------------|:-----------:|:--------------------------|
| 1 | `analogRead()` tarda ~110µs | ✅ Sí (datasheet ATmega328P: 13 ciclos ADC × 8µs = 104µs) | Si tardara más, podría perder timing de STEP a velocidades altas |
| 2 | `micros()` no overflow en operación normal | ⚠️ Overflow cada 71.58 minutos | Si el posicionador funciona >70 min seguidos, `_lastStepTime` puede calcular mal |
| 3 | El toggle de STEP genera un pulso limpio | ✅ Sí (el A4988 detecta flanco, no nivel) | — |
| 4 | El potenciómetro genera 0V-5V lineal | ⚠️ Depende de calidad del pot | Si es logarítmico (tipo A), el mapeo de velocidad será no-lineal |
| 5 | `delay(5)` para DIR_CHANGE es suficiente y no peligroso | ⚠️ Es un `delay()` pero solo se ejecuta en estado IDLE | Si se llamara en RUN, bloquearía 5ms de pasos |
| 6 | Los colores de cable del motor corresponden a los documentados | ❌ Los colores varían por fabricante | DEBE verificarse con multímetro antes de conectar |

---

## 2. Análisis Diferencial (Nivel 1 vs Nivel 2)

| Aspecto | Nivel 1 (original) | Nivel 2 (nuevo) |
|:--------|:-------------------|:-----------------|
| Estructura | Monolítico (.ino único) | Modular (4 módulos .h/.cpp) |
| Control de velocidad | Instantáneo (pot → delay directo) | Con rampa (pot → target → aceleración gradual) |
| Lectura ADC | Cruda (analogRead directo) | Filtrada (promedio 8 muestras) |
| Botón | Debounce básico en loop | Debounce con FSM de flanco |
| Dirección | Cambia en cualquier momento | Solo cambia en IDLE |
| Estados | 2 booleanos (motorActivo, direccion) | FSM enum 5 estados |
| Error handling | Ninguno | Estado ERROR con LED + recuperación |
| Constantes | Magic numbers | Nombradas con fórmula en config.h |
| Display | Cada 500ms sin control | Rate-limited, compilación condicional |

---

## 3. Dependencias Externas — Análisis de Source

### Arduino Core (AVR) — analogRead()

| Función | Parámetro REAL | ¿Bloqueante? | Duración | Estado interno | Quirks |
|:--------|:--------------|:---:|:---------|:---------------|:-------|
| analogRead(pin) | Pin analógico A0-A5 | **SÍ** | ~104µs | Selecciona MUX, inicia conversión ADC | Bloquea hasta que ADC completa |
| digitalRead(pin) | Pin digital 0-13 | No | ~4µs | — | Lee PORT register |
| digitalWrite(pin, val) | Pin + HIGH/LOW | No | ~4µs | — | Escribe PORT register |
| micros() | — | No | ~4µs | Timer0 overflow counter | Resolución: 4µs. Overflow: 2^32 µs = 71.58 min |
| millis() | — | No | ~4µs | Timer0 overflow counter | Resolución: 1ms. Overflow: 2^32 ms = 49.7 días |
| map(val, lo1, hi1, lo2, hi2) | 5 longs | No | ~8µs | — | Usa aritmética de enteros, puede overflow con valores grandes |

**Hallazgos del source (wiring_analog.c):**
- `analogRead()` configura el MUX del ADC (ADMUX), inicia conversión (ADCSRA |= ADSC), y **espera en un while-loop** hasta que ADSC vuelve a 0.
- Duración real: prescaler /128 → 13 ciclos ADC × (16MHz/128) = 13 × 8µs = **104µs** por lectura.
- Con nuestro filtro de 8 muestras leídas 1 por ciclo (cada 10ms), el impacto total es: 1 × 104µs cada 10ms = **1.04% del tiempo de CPU**.

**Cálculos:**
- A velocidad máxima (delay=500µs entre toggles), el loop tiene 500µs para ejecutar todo
- El loop completo (sin LCD) toma: ~110µs (ADC) + ~20µs (digitales) + ~10µs (FSM) = **~140µs**
- Margen: 500µs - 140µs = **360µs de margen** → ✅ SUFICIENTE

### Arduino Core — Timer0 (millis/micros)

**Hallazgos:**
- Timer0 está configurado con prescaler /64 en el core Arduino
- Genera interrupción cada 256 conteos = cada 1.024ms
- `millis()` se incrementa dentro de la ISR (TIMER0_OVF_vect)
- `micros()` = `timer0_overflow_count × 1024 + TCNT0 × 4`
- **QUIRK**: `micros()` tiene resolución de 4µs (no 1µs) en ATmega328P

**Impacto en nuestro proyecto:**
- El `STEP_DELAY_MIN_US = 500` es múltiplo de 4 → ✅ OK
- El `STEP_DELAY_MAX_US = 5000` es múltiplo de 4 → ✅ OK
- El overflow de `micros()` a los 71.58 min: la resta `now - _lastStepTime` sigue funcionando correctamente con aritmética unsigned de 32 bits (C++ unsigned overflow es well-defined)

### LiquidCrystal (Arduino Built-in)

| Función | ¿Bloqueante? | Duración | Quirks |
|:--------|:---:|:---------|:-------|
| lcd.begin(16,2) | Sí | ~50ms | Inicialización del HD44780, incluye delays internos |
| lcd.setCursor(col,row) | Sí | ~40µs | Envía comando al controlador |
| lcd.print(val) | Sí | ~40µs × caracteres | Envía char por char |
| lcd.write(byte) | Sí | ~40µs | Un solo carácter |
| lcd.clear() | Sí | ~2ms | Limpia display + home cursor |

**Hallazgos:**
- Cada carácter escrito al LCD toma ~40µs (enable pulse + data setup)
- Una actualización completa de 2 líneas (32 chars) = ~32 × 40µs = **~1.3ms**
- Con nuestro rate limiting de 500ms, esto ocurre solo 2 veces por segundo
- Impacto: 1.3ms × 2 = **2.6ms por segundo dedicados al LCD** (0.26% del CPU) → ✅ NEGLIGIBLE
- **QUIRK**: `lcd.clear()` toma ~2ms (mucho más que otros comandos). Usarlo solo en init.

---

## 4. Timing y Constantes Críticas

| Constante | Valor | Ubicación | Fórmula | Si ↑ | Si ↓ | Depende de |
|:----------|:------|:----------|:--------|:-----|:-----|:-----------|
| STEP_DELAY_MIN_US | 500µs | config.h:61 | 60M/(300RPM×2×200) | Motor más lento max | Motor más rápido → puede perder pasos | STEPS_PER_REV, MICROSTEP_MODE |
| STEP_DELAY_MAX_US | 5000µs | config.h:64 | 60M/(30RPM×2×200) | Arranque aún más lento | Velocidad mínima más alta | STEPS_PER_REV, MICROSTEP_MODE |
| ACCEL_INTERVAL_MS | 20ms | config.h:72 | 500ms/25ticks | Rampa más larga (suave) | Rampa más corta (brusca) | Inercia de carga |
| ACCEL_STEP_US | 180µs | config.h:74 | (5000-500)/25 | Saltos más grandes (brusco) | Saltos más pequeños (suave) | DELAY_MIN, DELAY_MAX, TICKS |
| ADC_READ_INTERVAL_MS | 10ms | config.h:83 | Seleccionado para 100Hz muestreo | Respuesta del pot más lenta | Más CPU en ADC | — |
| ADC_FILTER_SIZE | 8 | config.h:81 | Potencia de 2 para shift rápido | Más filtrado, respuesta lenta | Menos filtrado, más ruido | — |
| DEBOUNCE_MS | 50ms | config.h:93 | > rebote típico 10-30ms | Botón responde lento | Falsos positivos | Calidad del pulsador |
| LCD_UPDATE_INTERVAL_MS | 500ms | config.h:102 | 2Hz (suficiente para ojo humano) | LCD más lento | Más CPU, parpadeo | — |
| POT_DEAD_ZONE_LOW | 10 | config.h:88 | ~1% del rango ADC | Más difícil arrancar | Motor arranca con toque leve | Ruido del ADC |

**Tiempos calculados del sistema:**

```
Loop cycle completo (sin LCD): ~140µs
  ├── analogRead: 104µs (solo cada 10ms, promedio ~10µs/loop)
  ├── digitalRead ×2: 8µs
  ├── FSM switch: ~10µs
  ├── generateStep toggle: ~6µs
  └── overhead (variables, condiciones): ~10µs

Loop cycle con LCD update (cada 500ms): ~1440µs (1.4ms)
  ├── Loop normal: 140µs
  └── LCD 32 chars: ~1300µs

Tiempo crítico: delay entre pasos a velocidad máxima = 500µs
Margen sin LCD: 500 - 140 = 360µs de margen (72%)
Margen con LCD: ¡NEGATIVO! 500 - 1440 = -940µs ← PROBLEMA
```

**⚠️ BUG POTENCIAL ENCONTRADO:**
Cuando el LCD se actualiza (cada 500ms), el loop toma ~1.4ms. Si en ese preciso momento el motor está a velocidad máxima (delay=500µs), se PIERDE UN PASO porque el toggle no ocurre a tiempo.

**SOLUCIÓN (para Nivel 3):**
- Opción A: Aumentar `STEP_DELAY_MIN_US` a 1500µs (reduce RPM max a 100)
- Opción B: Actualizar LCD solo en estado IDLE o cuando delay > 2000µs
- Opción C: Usar Timer interrupt para generación de STEP (independiente del loop)
- Opción D: No usar LCD (DEBUG_SERIAL es suficiente para este proyecto educativo)

**NOTA para el usuario:** En la versión actual, si `LCD_ENABLED=0` (por defecto), este bug NO existe. Solo aparece si habilitas el LCD a velocidades >100 RPM.

---

## 5. Simulación del Loop a Velocidad Máxima

```
Configuración: STEP_DELAY_MIN_US=500, LCD_ENABLED=0

[t=0µs]     loop() inicia
[t=10µs]    inputManager.update() → no lee ADC (10ms no pasaron) → solo digitales
[t=20µs]    motorController.update() → FSM switch → STATE_RUNNING
[t=30µs]    _generateStep() → micros()-lastStep = 510µs > 500µs → TOGGLE! PIN=HIGH
[t=36µs]    _updateRPM() → calcula 300.0 RPM
[t=46µs]    displayManager.update() → no actualiza (2s no pasaron)
[t=50µs]    loop() termina → siguiente iteración

[t=50µs]    loop() inicia (segunda iteración)
[t=60µs]    inputManager.update() → no lee ADC
[t=70µs]    motorController.update() → STATE_RUNNING
[t=80µs]    _generateStep() → micros()-lastStep = 50µs < 500µs → NO toggle
[t=90µs]    displayManager.update() → no actualiza
[t=94µs]    loop() termina

... (repite ~9 veces sin toggle) ...

[t=500µs]   loop() inicia (~10ª iteración)
[t=530µs]   _generateStep() → micros()-lastStep = 500µs = 500µs → TOGGLE! PIN=LOW
[t=536µs]   ← UN PASO COMPLETO (HIGH+LOW) en ~1000µs total

Resultado: 1 paso cada ~1000µs = 1000 pasos/s = 300 RPM ✅ CORRECTO
```

---

## 6. Bugs Encontrados

### Capa 1 (estático — visibles al leer)

| # | Bug | Ubicación | Severidad | Fix propuesto | ¿Resolvió? |
|:-:|:----|:----------|:---------:|:--------------|:-----------:|
| 1 | `delay(DIR_CHANGE_DELAY_MS)` en motor_controller.cpp usa delay() | _setDirection() | BAJA | Solo se llama en IDLE → aceptable | N/A |
| 2 | `goto showBar` en display_manager.cpp (estilo cuestionable) | _updateLCD() | NINGUNA | Funciona correctamente, es un pattern válido en switch-case | N/A |

### Capa 2 (dinámico — encontrados en análisis de timing)

| # | Bug | Causa real | Cálculo que lo demuestra | Fix | ¿Resolvió? |
|:-:|:----|:-----------|:-------------------------|:----|:-----------:|
| 1 | LCD puede causar pérdida de paso a velocidad máxima | LCD write: 32×40µs=1.3ms > STEP_DELAY_MIN=500µs | Ver sección 4 | LCD_ENABLED=0 por defecto | ✅ (evitado) |
| 2 | El promedio móvil del ADC tarda 80ms en responder a cambio brusco | 8 muestras × 10ms/muestra = 80ms latencia | — | Aceptable para control manual | N/A |
| 3 | Si pot es logarítmico (tipo A), la curva de velocidad será no-intuitiva | map() asume linealidad | — | Verificar pot tipo B (lineal) | [VERIFICAR] |

### Capa 3 (eliminación — requiere hardware)

| # | Variable a aislar | Test propuesto | Resultado | ¿Resolvió? |
|:-:|:------------------|:---------------|:----------|:-----------:|
| 1 | ¿Motor gira correctamente con aceleración? | Test: arrancar con ACCEL_TICKS=5, luego 15, luego 25 | [PENDIENTE] | — |
| 2 | ¿Pierde pasos a 300 RPM sin carga? | Test: contar vueltas vs pasos enviados en 1 minuto | [PENDIENTE] | — |
| 3 | ¿El filtro ADC es suficiente para el pot real? | Test: medir estabilidad con Serial.print del valor filtrado | [PENDIENTE] | — |

---

## 7. Cadena Causal (bug LCD, principal)

```
CAUSA: LiquidCrystal::print() es BLOQUEANTE (~40µs por carácter)
   ↓
MECANISMO: 32 caracteres × 40µs = 1280µs de bloqueo del loop
   ↓
EFECTO INTERNO: _generateStep() no se ejecuta durante 1280µs
   ↓
SÍNTOMA: Si STEP_DELAY_MIN_US=500µs, se pierden 2 flancos → 1 paso perdido cada 500ms
   ↓
CONSECUENCIA: A 300 RPM = 5 rev/s = 1000 pasos/s → perder 2 pasos cada 500ms = 4 pasos/s
              Equivale a un error angular de 4×1.8° = 7.2° por segundo (INACEPTABLE para posicionador)
```

**Solución aplicada:** `LCD_ENABLED=0` por defecto. El LCD solo se habilita si la velocidad máxima se reduce o se usa Timer interrupt.

---

## 8. Tests Incrementales (para calibración Capa 3)

| # | Test | Qué cambia vs anterior | Resultado esperado | Conclusión |
|:-:|:-----|:-----------------------|:-------------------|:-----------|
| Base | Solo motor + driver, sin Arduino. Alimentar 12V, medir VREF | — | VREF=1.34V medido con multímetro | Hardware eléctrico OK |
| +1 | Arduino con código mínimo: solo toggle pin 3 cada 2ms (fijo) | Agregar Arduino | Motor gira a 150 RPM constante | Comunicación Arduino→A4988→Motor OK |
| +2 | Agregar potenciómetro, mapear a delay variable | Agregar ADC | Motor varía velocidad con pot | ADC + mapeo OK |
| +3 | Agregar filtro de 8 muestras | Agregar filtro | Velocidad más estable | Filtro funciona |
| +4 | Agregar FSM completa (IDLE/ACCEL/RUN/DECEL) | Agregar aceleración | Arranque suave, parada suave | FSM OK |
| +5 | Agregar botón + selector de dirección | Agregar entradas | Control completo funcional | Sistema completo |
| +6 | Agregar carga al plato (2 kg, luego 5 kg) | Agregar carga mecánica | ¿Pierde pasos? ¿Vibra? | Calibrar aceleración |
| +7 | Agregar poleas GT2 (3:1) + carga | Agregar reducción | Mayor torque, menor velocidad | Verificar parámetros finales |

---

## 9. Fix Final (para bug LCD)

```cpp
// En config.h, la línea:
#ifndef LCD_ENABLED
#define LCD_ENABLED  0     // ← DESHABILITADO por defecto (evita pérdida de pasos)
#endif

// Si se quiere LCD + velocidad alta, cambiar en display_manager.cpp:
void DisplayManager::_updateLCD(const MotorStatus& motor, const InputState& inputs) {
    // SOLO actualizar si el motor está lento o parado
    if (motor.currentDelay < 2000 && motor.state != STATE_IDLE) return;  // ← FIX
    // ... resto del código LCD
}
```

**Justificación:** Este fix ataca la CAUSA RAÍZ (el LCD bloquea demasiado tiempo) restringiendo cuándo se permite actualizar. No es ideal pero es pragmático para el nivel del proyecto.

---

## 10. Verificación

**Test de confirmación (cuando se tenga hardware):**
```
1. Cargar código con LCD_ENABLED=0
2. Abrir Serial Monitor a 9600 baud
3. Girar potenciómetro: debe reportar RPM crecientes (30→300)
4. Presionar botón: debe ciclar IDLE→ACCEL→RUN→DECEL→IDLE
5. Switch dirección (en IDLE): debe cambiar CW↔CCW
6. Dejar corriendo 5 min a velocidad media: no debe haber errores
```

**Resultado esperado en Serial:**
```
[FSM] IDLE -> ACCELERATING
[STATUS] State=ACCEL | RPM=45.0 | Dir=CW | Pot=512 | Delay=2750us
[FSM] ACCELERATING -> RUNNING
[STATUS] State=RUN | RPM=109.1 | Dir=CW | Pot=512 | Delay=2750us
```

---

## 11. Pendientes [VERIFICAR]

| # | Dato faltante | Necesario para | Pregunta para el usuario |
|:-:|:--------------|:---------------|:-------------------------|
| 1 | ¿Pot real es tipo B (lineal) o tipo A (logarítmico)? | Curva de velocidad correcta | ¿Qué dice la marca del potenciómetro? ¿Tiene "B" o "A" impreso? |
| 2 | ¿Los colores de cable del motor coinciden con Rojo-Azul-Verde-Negro? | Conexión correcta al driver | Medir continuidad: ¿qué pares tienen resistencia ~1-5Ω? |
| 3 | ¿La fuente DC real entrega 12V estables bajo carga de 1.68A? | Motor no pierde pasos por bajo voltaje | Medir voltaje en VMOT del driver CON motor girando |
| 4 | ¿El Rsense del A4988 específico es 0.100Ω o 0.050Ω? | Cálculo correcto de VREF | Medir resistencia marcada como R100 o R050 en la PCB del driver |
| 5 | ¿Se usará reducción GT2 3:1 o acoplamiento directo? | Ajustar constantes RPM y torque | ¿Ya se compraron las poleas GT2 20T y 60T? |

---

## 12. Nivel de Análisis Alcanzado

| Capa | Descripción | ¿Completado? | Hallazgos principales |
|:-----|:------------|:---:|:------|
| 1. Estático | Leer código, mapear, detectar inconsistencias | ✅ | delay() solo en IDLE (aceptable), goto en switch (estilo) |
| 2. Dinámico | Timing de analogRead, LCD blocking, overflow micros | ✅ | LCD puede perder pasos a >100 RPM; evitado con LCD_ENABLED=0 |
| 3. Eliminación | Tests incrementales con hardware real | ⏳ PENDIENTE | 7 tests diseñados, requiere hardware armado |

**Conclusión final:** El diseño de software Nivel 2 es sólido y compilable. El riesgo principal (LCD blocking) está mitigado por defecto. Los parámetros de aceleración y zonas muertas necesitan calibración con el hardware físico (Capa 3) una vez que la estructura metálica, motor y driver estén montados.

---

*Análisis v1.0 — Metodología: skill-analisis-profundo.md (3 capas)*
*Herramientas: guia-crecimiento-de-codigo.md (Nivel 1→2→3), vsCode-AI Context Engineering v3*
