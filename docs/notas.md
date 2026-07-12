# Notas de Hardware — Posicionador Motorizado NEMA17

> Documento vivo. Actualizar con cada prueba, hallazgo o calibración.
> Última actualización: 2025-07-12
> Estado: PRE-HARDWARE (basado en análisis de código y datasheets)

---

## 1. Inventario de Componentes

| # | Componente | Modelo/Especificación | Cantidad | Estado | Notas |
|:-:|:-----------|:---------------------|:--------:|:------:|:------|
| 1 | Motor paso a paso | NEMA17 JK42HS40-1684 (1.68A, 3.6 kg·cm) | 1 | [VERIFICAR] | Motor principal. 4 hilos bipolar |
| 2 | Motor alternativo | JK42HS34-0956 (1.0A, 1.6 kg·cm) | 1 | [VERIFICAR] | Solo para cargas livianas <2 kg |
| 3 | Driver | A4988 (Allegro) con PCB | 1 | [VERIFICAR] | Rsense = 0.100Ω (confirmar) |
| 4 | MCU | Arduino Nano (ATmega328P, CH340) | 1 | [VERIFICAR] | Old Bootloader o nuevo — probar ambos |
| 5 | Fuente DC | 12V / 2A mínimo (recomendado 3A) | 1 | [VERIFICAR] | Adaptador AC-DC |
| 6 | Potenciómetro | 10kΩ lineal (tipo B) | 1 | [VERIFICAR] | Confirmar tipo B, NO tipo A |
| 7 | Switch SPDT | Selector dirección | 1 | [VERIFICAR] | 3 terminales |
| 8 | Pulsador N.A. | Botón marcha/parada | 1 | [VERIFICAR] | Con terminales para soldar |
| 9 | Capacitor | 100µF / 25V electrolítico | 1 | [VERIFICAR] | OBLIGATORIO en VMOT |
| 10 | Fusible | 2.5A slow-blow | 1 | [VERIFICAR] | Con portafusible |
| 11 | Polea motriz | GT2, 20 dientes, bore 5mm | 1 | [VERIFICAR] | Para eje motor |
| 12 | Polea conducida | GT2, 60 dientes, bore 20mm | 1 | [VERIFICAR] | Para eje plato |
| 13 | Correa | GT2, 6mm ancho, ~280mm cerrada | 1 | [VERIFICAR] | Medir distancia real entre ejes |
| 14 | Chumaceras | UCP204, bore 20mm | 2 | [VERIFICAR] | Soporte del eje principal |
| 15 | Eje | Barra acero SAE 1020, Ø20mm | 1 | [VERIFICAR] | Largo ~50cm |
| 16 | LCD (opcional) | 16x2 HD44780 | 1 | [VERIFICAR] | Solo si se decide usarlo |

---

## 2. Procedimiento de Ajuste VREF (A4988)

### 2.1 Datos necesarios


```
Motor: JK42HS40-1684
Corriente nominal: 1.68 A por fase
Rsense del driver: [VERIFICAR — buscar R100 o R050 en PCB]

Si Rsense = 0.100Ω (más común):
  VREF = I × 8 × Rsense = 1.68 × 8 × 0.100 = 1.344V → Ajustar a 1.34V

Si Rsense = 0.050Ω (algunos clones):
  VREF = I × 8 × Rsense = 1.68 × 8 × 0.050 = 0.672V → Ajustar a 0.67V
```

### 2.2 Procedimiento paso a paso

1. **DESCONECTAR el motor del driver** (cables 1A, 1B, 2A, 2B)
2. Conectar fuente 12V a VMOT/GND del driver
3. Conectar 5V y GND a VDD/GND lógico del driver (puede venir del Arduino via USB)
4. Puentear RST con SLP (ambos a +5V)
5. Colocar multímetro en modo voltaje DC
6. Punta positiva: en el potenciómetro del driver (punto de test VREF)
7. Punta negativa: en GND del driver
8. Leer voltaje actual
9. Con destornillador cerámico (no metálico), girar potenciómetro hasta leer **1.34V**
10. **RECONECTAR el motor**
11. Verificar que el motor no se calienta excesivamente en reposo (tibio = OK, caliente = VREF muy alto)

### 2.3 Señales de VREF incorrecto

| Síntoma | VREF demasiado ALTO | VREF demasiado BAJO |
|:--------|:-------------------:|:-------------------:|
| Motor en reposo | Se calienta rápido (>60°C) | Frío |
| Giro sin carga | Funciona pero caliente | Funciona normal |
| Giro con carga | Funciona, posible thermal shutdown | **Pierde pasos** (salta, vibra) |
| Driver | Se calienta peligrosamente | Normal |
| Consecuencia | Daño al motor/driver a largo plazo | Motor no mueve la carga |

---

## 3. Identificación de Bobinas del Motor

### 3.1 Por qué es crítico

El motor NEMA17 tiene 4 hilos (bipolar). Conectar los hilos en el orden incorrecto puede:
- Hacer que el motor vibre sin girar (bobinas en cortocircuito)
- Girar en dirección opuesta a la esperada (invertir conexión)
- En peor caso, no dañar nada pero no funcionar

### 3.2 Procedimiento con multímetro

```
Materiales: Multímetro en modo resistencia (Ω)

1. Medir resistencia entre TODOS los pares posibles:
   - Rojo-Azul:   _____ Ω
   - Rojo-Verde:  _____ Ω
   - Rojo-Negro:  _____ Ω
   - Azul-Verde:  _____ Ω
   - Azul-Negro:  _____ Ω
   - Verde-Negro: _____ Ω

2. Los dos pares que muestren ~1.5-3.5Ω son las bobinas:
   - Par 1 (Bobina A): _______ y _______ → conectar a 1A y 1B del driver
   - Par 2 (Bobina B): _______ y _______ → conectar a 2A y 2B del driver

3. Los pares que muestran infinito (OL) son de bobinas DIFERENTES — NO conectar juntos

Resistencia esperada por fase: ~1.65Ω (JK42HS40-1684)
```

### 3.3 Colores típicos (VERIFICAR — varían por fabricante)

| Fabricante | Bobina A+ | Bobina A- | Bobina B+ | Bobina B- |
|:-----------|:---------:|:---------:|:---------:|:---------:|
| Típico (datasheet) | Rojo | Azul | Verde | Negro |
| Alternativo común | Negro | Verde | Rojo | Azul |
| Otro alternativo | Rojo | Azul | Negro | Verde |

**⚠️ NO confiar en colores. SIEMPRE medir con multímetro.**

---

## 4. Notas de Calibración

### 4.1 Zona muerta del potenciómetro

```
Valor actual en código: POT_DEAD_ZONE_LOW = 10 (~1% del rango ADC)

Procedimiento de calibración:
1. Subir código con Serial habilitado (DEBUG_SERIAL=1)
2. Abrir Serial Monitor a 9600 baud
3. Girar potenciómetro completamente a la izquierda (mínimo)
4. Observar valor de Pot en el reporte: [STATUS] ... Pot=XX ...
5. El valor mínimo leído es: _____ 
6. Sumar 5 como margen → nuevo POT_DEAD_ZONE_LOW = _____
7. Actualizar en config.h

Ejemplo:
  Si pot a mínimo lee 3-7 (fluctúa): POT_DEAD_ZONE_LOW = 12
  Si pot a mínimo lee 0-2 (estable): POT_DEAD_ZONE_LOW = 5
```


### 4.2 Rampa de aceleración

```
Valores actuales:
  ACCEL_INTERVAL_MS = 20 ms (cada cuánto incrementa velocidad)
  ACCEL_TICKS_TOTAL = 25 ticks (cuántos pasos para la rampa)
  ACCEL_STEP_US = 180 µs (cuánto reduce el delay por tick)
  Tiempo total rampa = 20ms × 25 = 500ms

Procedimiento de calibración:
1. Probar sin carga: ¿Motor arranca sin vibrar? 
   - Sí → parámetros OK para vacío
   - No → Aumentar ACCEL_TICKS_TOTAL a 35 (rampa más lenta: 700ms)

2. Agregar carga 2 kg: ¿Motor arranca sin perder pasos?
   - Sí → parámetros OK para carga ligera
   - No → Aumentar ACCEL_TICKS_TOTAL a 40 (800ms)

3. Agregar carga 5 kg (con reducción GT2): ¿Arranca correctamente?
   - Sí → calibración completa
   - No → Aumentar a 50 ticks (1000ms) o reducir velocidad máxima

Señales de pérdida de pasos al arrancar:
  - Motor zumba pero no gira
  - Motor salta posiciones (giro irregular)
  - Motor gira unos pasos y se detiene
```

### 4.3 Velocidad máxima real

```
Valores actuales:
  STEP_DELAY_MIN_US = 500 µs → 300 RPM motor → 100 RPM plato (con 3:1)

Procedimiento de verificación:
1. Sin reducción (acoplamiento directo):
   - Girar pot al máximo
   - Contar vueltas en 1 minuto con cronómetro
   - RPM medido: _____ (esperado: ~300 RPM)
   - Si es diferente: ajustar STEP_DELAY_MIN_US

2. Con reducción GT2 3:1:
   - Repetir medición en el plato
   - RPM plato medido: _____ (esperado: ~100 RPM)
   - Error = |medido - esperado| / esperado × 100 = _____%
   
3. Si el motor pierde pasos a velocidad máxima:
   - Aumentar STEP_DELAY_MIN_US a 625 (= 240 RPM motor = 80 RPM plato)
   - O aumentar STEP_DELAY_MIN_US a 750 (= 200 RPM motor = 67 RPM plato)
```

### 4.4 Filtro ADC — Verificación de estabilidad

```
Procedimiento:
1. Subir código con DEBUG_SERIAL=1
2. Fijar potenciómetro en posición media (manualmente, no mover)
3. Observar 10 reportes consecutivos del Serial Monitor:
   [STATUS] ... Pot=512 ...
   [STATUS] ... Pot=513 ...
   [STATUS] ... Pot=511 ...
   
4. Calcular rango de variación:
   Valor máximo: _____
   Valor mínimo: _____
   Rango = max - min = _____

5. Criterio de aceptación:
   - Rango ≤ 3: EXCELENTE (filtro funciona perfecto)
   - Rango 4-8: ACEPTABLE (ajustar si causa problemas)
   - Rango > 8: PROBLEMA (verificar conexiones, agregar cap 100nF en pin A0)

Sin filtro (referencia): rango típico = 6-12 counts
Con filtro 8 muestras: rango esperado = 1-3 counts
```

---

## 5. Hallazgos del Análisis de Código (Pre-Hardware)

### 5.1 Bug LCD confirmado por cálculo

```
Problema: LiquidCrystal::print() bloquea 40µs por carácter
Cálculo: 32 caracteres × 40µs = 1280µs de bloqueo del loop
Condición: STEP_DELAY_MIN = 500µs (velocidad máxima)
Resultado: 1280µs > 500µs → el motor PIERDE 1 PASO cada update de LCD

Frecuencia del problema: Cada 500ms (LCD_UPDATE_INTERVAL)
Pasos perdidos por segundo: ~2 (1 por cada update)
Error angular por segundo: 2 × 1.8° = 3.6° (7.2° contando ambos flancos)
En 1 minuto: ~216° de error acumulado → INACEPTABLE

SOLUCIÓN APLICADA: LCD_ENABLED=0 por defecto en config.h
Si se quiere LCD: solo activar a velocidades < 100 RPM (delay > 1500µs)
```

### 5.2 Overflow de micros() — NO es bug

```
micros() usa unsigned long (32 bits)
Overflow: 2^32 µs = 4,294,967,296 µs = 71.58 minutos

¿Afecta al código?
La línea crítica es:
  if (now - _lastStepTime >= _status.currentDelay)

Ejemplo con overflow:
  _lastStepTime = 4,294,967,200 (justo antes del overflow)
  now = 96 (justo después del overflow)
  now - _lastStepTime = 96 - 4,294,967,200 = 96 + 96 = 192 (¡aritmética unsigned!)
  
  En realidad: 96 - 4294967200 en unsigned 32-bit = 
  (0x00000060 - 0xFFFFFF60) = 0x00000100 = 256 µs ← CORRECTO

CONCLUSIÓN: La resta unsigned maneja overflow automáticamente en C++.
No requiere corrección. El motor funciona >71 minutos sin problemas.
```


### 5.3 delay() en _setDirection() — Aceptable

```
Ubicación: motor_controller.cpp → _setDirection()
Línea: delay(DIR_CHANGE_DELAY_MS);  // = 5ms

¿Viola la regla "NUNCA delay() en loop"?
NO, porque:
  - _setDirection() solo se llama desde _handleIdle()
  - En estado IDLE, el motor NO genera pasos (_generateStep no se ejecuta)
  - Los 5ms de bloqueo no afectan timing de motor (está parado)
  - El datasheet A4988 requiere >200ns entre DIR y STEP
  - Usamos 5ms como margen de seguridad mecánica (correa + poleas)

Si en el futuro se permitiera cambio de dirección con motor en movimiento:
  → Primero decelerar a 0, luego cambiar DIR, luego re-acelerar
  → El delay(5) sería válido porque se ejecutaría entre DECEL→IDLE→ACCEL
```

### 5.4 goto en display_manager.cpp — Estilo, no bug

```
Ubicación: display_manager.cpp → _updateLCD() → switch statement

    case STATE_ACCELERATING:
        lcd.print(F("ACC "));
        goto showBar;
    case STATE_RUNNING:
        lcd.print(F("RUN "));
        goto showBar;
    case STATE_DECELERATING:
        lcd.print(F("DEC "));
        goto showBar;
    ...
    showBar:
    {
        uint8_t bars = map(inputs.potRaw, 0, 1023, 0, 10);
        ...
    }

¿Es problemático?
NO. Es un pattern válido en switch-case para evitar duplicación de código.
La alternativa (función separada) consumiría más stack por el overhead de llamada.
En firmware embebido con 2KB RAM, esta optimización es justificable.
```

---

## 6. Notas de Seguridad Eléctrica

### 6.1 ADVERTENCIAS CRÍTICAS

```
⚠️ NUNCA conectar/desconectar motor con driver energizado
   Razón: Los picos inductivos (back-EMF) del motor alcanzan 40-60V
   Consecuencia: Destrucción INSTANTÁNEA del chip A4988 (~25 Bs + 1-2 semanas de espera)
   Solución: SIEMPRE apagar fuente 12V antes de tocar cables del motor

⚠️ Capacitor 100µF OBLIGATORIO entre VMOT y GND del driver
   Razón: Los picos del switching del driver se propagan por la alimentación
   Consecuencia: Sin capacitor, picos de 60V+ destruyen el driver
   Solución: Soldar capacitor lo más cerca posible de los pines VMOT/GND
   Tipo: Electrolítico 100µF/25V mínimo (polaridad: + a VMOT, - a GND)

⚠️ NUNCA exceder VREF calculado (1.34V para este motor)
   Razón: Mayor corriente = mayor calentamiento en bobinas + driver
   Consecuencia: Thermal shutdown del driver o daño permanente al motor
   Solución: Medir con multímetro ANTES de conectar motor. Ajustar con precisión

⚠️ Puentear RST con SLP en el driver A4988
   Razón: Si RST queda flotante (no conectado), el driver puede resetear aleatoriamente
   Si SLP queda flotante, el driver puede entrar en modo sleep
   Solución: Puentear ambos pines entre sí Y conectar a +5V
```

### 6.2 Protección del sistema

```
Componentes de protección instalados:
  1. Fusible 2.5A slow-blow → Protege contra cortocircuito
  2. Capacitor 100µF → Protege driver contra picos inductivos
  3. ENABLE pin → Software puede desactivar motor ante fallo
  4. Estado ERROR en FSM → Detiene motor si detecta anomalía
  5. INPUT_PULLUP → Botones no flotan (estado definido sin presionar)

Protecciones NO implementadas (posibles mejoras v3.0):
  - Sensor de temperatura en driver/motor
  - Detección de pérdida de pasos (requiere encoder)
  - Watchdog timer del ATmega328P
  - Parada de emergencia por hardware (botón que corta ENABLE directamente)
```

---

## 7. Registro de Pruebas

### Formato de registro

```
| Fecha | Test ID | Configuración | Resultado | Acción |
|:------|:--------|:--------------|:----------|:-------|
| [FECHA] | T0 | VREF medido | ___V (esperado 1.34V) | [Ajustar/OK] |
| [FECHA] | T1 | Toggle fijo 2ms | RPM=___ (esperado 150) | [OK/Ajustar] |
| [FECHA] | T2 | Pot variable | Rango RPM: ___-___ | [OK/Ajustar] |
| [FECHA] | T3 | Filtro ADC | Rango Pot: ___ counts | [OK/Cap] |
| [FECHA] | T4 | FSM completa | Transiciones: ___ | [OK/Bug] |
| [FECHA] | T5 | Dirección | Solo en IDLE: ___ | [OK/Bug] |
| [FECHA] | T6 | Carga 2kg/5kg | Pérdida pasos: ___ | [OK/Accel] |
| [FECHA] | T7 | Reducción GT2 | RPM plato: ___ | [OK/Ajustar] |
```

### Espacio para notas de cada prueba

**Test T0 — VREF:**
```
Fecha: ___
VREF medido: ___V
Rsense confirmado: ___ Ω
Temperatura driver después de 5 min holding: ___°C
Notas: 
```

**Test T1 — Toggle básico:**
```
Fecha: ___
Código usado: toggle D3 cada 2000µs (fijo, sin FSM)
RPM medido (contar vueltas en 60s): ___
Sonido del motor: [Suave / Áspero / Vibra]
Dirección: [CW / CCW]
Notas:
```

**Test T2 — Potenciómetro:**
```
Fecha: ___
RPM mínimo (pot a la izquierda): ___
RPM máximo (pot a la derecha): ___
¿Transición suave? [Sí / No / Saltos]
Notas:
```

**Test T4 — FSM completa:**
```
Fecha: ___
Transición IDLE→ACCEL: [OK / No responde / Delay excesivo]
Transición ACCEL→RUN: [Suave / Brusca / No llega]
Transición RUN→DECEL (botón): [Suave / Instantánea / No responde]
Transición DECEL→IDLE: [OK / Motor sigue girando]
Serial Monitor muestra transiciones: [Sí / No]
Notas:
```

---

## 8. Troubleshooting Rápido

| Síntoma | Causa probable | Solución |
|:--------|:--------------|:---------|
| Motor no gira, no hace ruido | ENABLE en HIGH (desactivado) | Verificar que ENABLE=LOW al iniciar. Revisar conexión D5 |
| Motor vibra pero no gira | Bobinas mal conectadas (par cruzado) | Re-identificar bobinas con multímetro (sección 3) |
| Motor gira en dirección opuesta | Cables de una bobina invertidos | Intercambiar los 2 cables de UNA bobina (A o B, no ambas) |
| Motor pierde pasos a alta velocidad | VREF bajo o velocidad excesiva | Verificar VREF. Aumentar STEP_DELAY_MIN_US |
| Motor se calienta mucho (>60°C) | VREF demasiado alto | Reducir VREF. Verificar cálculo con Rsense correcto |
| Driver se apaga solo (thermal shutdown) | Corriente excesiva o sin disipador | Reducir VREF. Agregar disipador al A4988 |
| Arduino no detecta botón | Pull-up no activado o cable suelto | Verificar INPUT_PULLUP en código. Medir con multímetro |
| Velocidad inestable (oscila) | Pot tipo A (logarítmico) o conexión suelta | Verificar tipo pot. Agregar cap 100nF en A0 |
| Serial Monitor basura | Baud rate incorrecto | Configurar monitor a 9600 baud |
| No sube el código al Arduino | Bootloader o puerto incorrecto | Probar "Old Bootloader" en Arduino IDE. Verificar driver CH340 |
| Motor gira pero LCD no actualiza | LCD_ENABLED=0 en config.h | Cambiar a LCD_ENABLED=1 (solo si vel<100 RPM) |
| Motor se detiene después de 1+ hora | [NO debería pasar — micros overflow es safe] | Verificar fuente (puede ser thermal protection de la fuente) |

---

*Documento preparado para completar con datos reales durante construcción.*
*Metodología: skill-analisis-profundo.md + vsCode-AI Context Engineering v3*
