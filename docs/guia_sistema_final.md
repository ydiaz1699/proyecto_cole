# Guía de Conexión, Montaje y Operación — Sistema Final (v2.0)

> **Proyecto:** Posicionador Motorizado con Motor Paso a Paso NEMA17  
> **Institución:** Colegio Nacional San Luis — Mecánica Industrial — Tarija, Bolivia  
> **Firmware:** v2.0 (FSM Robusta, Rampa Lineal, Filtro ADC)  
> **Placas soportadas:** Arduino Uno R3 (Hardware actual) / Arduino Nano (ATmega328P)

---

## 1. Hardware del Sistema Final

| Componente | Código (Sawers / Ref.) | Función en el Sistema |
| :--- | :---: | :--- |
| **Arduino Uno R3 Clone SMD** (o Nano) | **`00014`** | Controlador principal con Firmware v2.0 |
| **Driver Stepper HR4988 / A4988** | **`00154`** | Control de potencia ($V_{REF} = 0.92\text{V}$, con disipador) |
| **Motor NEMA17** | **`KS42STH34-1504A`** | Actuador angular (1.50A, 200 pasos/rev, polea GT2) |
| **Potenciómetro 10kΩ lineal** | **`WH148-10K`** | Regulador de velocidad analógico (0 a 300 RPM) |
| **Switch 1 tiempo / SPDT** | **`00129`** | Selector de dirección (Giro Horario / Antihorario) |
| **Pulsador normalmente abierto (N.A.)** | **`PBS-11B`** | Botón Marcha / Parada (*Toggle Start/Stop*) |
| **Capacitor Electrolítico** | **`100uF-25V`** | Filtro de picos inductivos en entrada de potencia |
| **Fuente de Alimentación DC** | **`12V-2A`** o **`12V-3A`** | Alimentación general de potencia y lógica |
| **Interruptor general 220V + Fusible** | **`KCD1`** + 2.5A | Protección y encendido general del equipo |

---

## 2. Diagrama de Cableado Completo del Sistema Final

```
                              +---------------------------------------+
                              |         FUENTE 12V DC (>= 2A)         |
                              |   [+] 12V                     [-] GND |
                              +------+---------------------------+----+
                                     |                           |
             +-----------------------+                           |
             |                                                   |
             | (Alimenta Arduino por VIN o Jack DC)              |
             v                                                   v
     +--------------------------+               +-----------------------------------+
     |   ARDUINO UNO R3 / NANO  |               |       DRIVER HR4988 / A4988       |
     |                          |               |                                   |
     |     VIN <----------------+               |   VMOT <--- +12V Fuente           |
     |     GND <----------------+---------------+-> GND  <--- GND Fuente y Arduino  |
     |                          |               |       [Capacitor 100µF 25V] ⚠️    |
     |      5V -----------------+-------------->|   VDD  <--- +5V Lógica Arduino    |
     |                          |               |                                   |
     |      D3 (STEP) ----------+-------------->|   STEP                            |
     |      D4 (DIR) -----------+-------------->|   DIR                             |
     |      D5 (ENABLE) --------+-------------->|   ENABLE (LOW=Activo)             |
     |                          |               |                                   |
     |      D9 (MS1) -----------+-------------->|   MS1                             |
     |     D10 (MS2) -----------+-------------->|   MS2  (Control automático        |
     |     D11 (MS3) -----------+-------------->|   MS3   de micropasos en FSM)     |
     |                          |               |                                   |
     |                          |               |   RESET ──┐ (Puente con           |
     |                          |               |   SLEEP ──┘  cable jumper) ⚠️     |
     |                          |               |                                   |
     |                          |               |   SALIDAS AL MOTOR NEMA17:        |
     |                          |               |   1B ──► CABLE ROJO   (Bobina A1) |
     |                          |               |   1A ──► CABLE VERDE  (Bobina A2) |
     |                          |               |   2A ──► CABLE AZUL   (Bobina B1) |
     |                          |               |   2B ──► CABLE NEGRO  (Bobina B2) |
     +--------------------------+               +-----------------------------------+
       |    |         |
       |    |         +-------> [LED D13 INTEGRADO] (Fijo en marcha, parpadeo en ERROR)
       |    |
       |    +-----------------> [SWITCH SELECTOR DIRECCIÓN (SPDT / 1 Tiempo)]
       |                         - Pin central al Arduino Pin D7 (INPUT_PULLUP)
       |                         - Pin lateral a GND común
       |                         * Circuito abierto (D7=HIGH) = Giro HORARIO (CW)
       |                         * Cerrado a tierra (D7=LOW)  = Giro ANTIHORARIO (CCW)
       |                         * NOTA: Solo cambia dirección con el motor en IDLE
       |
       +----------------------> [PULSADOR N.A. MARCHA / PARADA]
       |                         - Una pata al Arduino Pin D8 (INPUT_PULLUP)
       |                         - Otra pata a GND común
       |                         * Presionar = Alterna Marcha / Parada
       |
       v
     [POTENCIÓMETRO 10kΩ LINEAL (VELOCIDAD)]
       - Terminal 1 (Extremo): a 5V del Arduino
       - Terminal 2 (Cursor central): al Arduino Pin A0
       - Terminal 3 (Extremo opuesto): a GND del Arduino
       * Zona muerta de seguridad: <1% apaga el motor para evitar arranques por ruido.
```

---

## 3. Tabla Resumen Pin a Pin

| Pin Arduino | Dispositivo Conectado | Pin en Dispositivo | Configuración / Función |
| :---: | :---: | :---: | :--- |
| **`VIN`** | Fuente 12V | Borne `+12V` | Entrada del regulador de voltaje interno del Arduino |
| **`5V`** | Driver y Potenciómetro | `VDD` (Driver) y Pin 1 (Pot) | Suministro regulado de 5V para electrónica lógica |
| **`GND`** | Todos los módulos | Todos los `GND` | **Masa común unificada obligatoria** |
| **`D3`** | Driver HR4988 | `STEP` | Salida de tren de pulsos (velocidad del motor) |
| **`D4`** | Driver HR4988 | `DIR` | Salida de sentido de giro (`HIGH`/`LOW`) |
| **`D5`** | Driver HR4988 | `ENABLE` | `LOW` = motor energizado; `HIGH` = deshabilitado |
| **`D7`** | Switch Selector | Pin de señal | `INPUT_PULLUP`: Abierto=CW, Cerrado a GND=CCW |
| **`D8`** | Pulsador Marcha | Pin de señal | `INPUT_PULLUP`: Pulso a GND conmuta Marcha/Parada |
| **`D9`** | Driver HR4988 | `MS1` | Salida lógica para selección de micropasos |
| **`D10`** | Driver HR4988 | `MS2` | Salida lógica para selección de micropasos |
| **`D11`** | Driver HR4988 | `MS3` | Salida lógica para selección de micropasos |
| **`A0`** | Potenciómetro 10k | Cursor central | Entrada analógica ADC con filtro móvil de 8 muestras |
| **`D13`** | Arduino (Built-in) | LED L interno | Indicador visual de estado FSM y alarmas |

---

## 4. Reglas Críticas de Seguridad antes del Primer Encendido

1. **Capacitor Electrolítico Obligatorio:** Colocar el capacitor de **$100\,\mu\text{F}$ ($\ge 25\text{V}$)** lo más pegado posible a los pines `VMOT` y `GND` del driver, cuidando la polaridad (+ y -). Si se omite, los picos de voltaje al conectar la fuente de 12V quemarán el chip HR4988.
2. **Puente RESET con SLEEP:** Debe existir un puente físico uniendo ambos pines en el driver. Sin él, el módulo permanece en modo bajo consumo y el motor no responderá.
3. **Disipador de Aluminio en el Chip:** Pegar el disipador metálico encima del chip `HR4988SQ` antes de someterlo a trabajo continuo.
4. **Calibración $V_{REF}$ Verificada:** El tornillo del trimmer `VR1` debe estar ajustado a **$0.92\text{ V}$** (corriente nominal resultante: $1.15\text{ A}$).
5. **No desconectar cables bajo tensión:** Jamás desconectar los 4 cables del motor ni los cables del driver mientras la fuente de 12V esté conectada a la red.

---

## 5. Carga del Firmware Final v2.0 desde PlatformIO

Una vez armado el circuito:

### A) Si usas el Arduino Uno R3 (Comprado):
```bash
pio run -e uno -t upload
```

### B) Si usas el Arduino Nano:
```bash
pio run -e nano -t upload
```

*(También puedes seleccionarlo gráficamente en la barra azul inferior de VS Code haciendo clic sobre el nombre del entorno actual y seleccionando **`env:uno`**, luego haciendo clic en la flecha de Subir `→`).*

---

## 6. Monitoreo y Diagnóstico por Serial

El firmware v2.0 transmite automáticamente reportes de estado cada 2 segundos a **9600 baudios**:
```bash
pio device monitor --baud 9600
```

### Mensaje de arranque esperado:
```
========================================
POSICIONADOR MOTORIZADO NEMA17 v2.0.0
Colegio Nacional San Luis - Tarija
FSM: IDLE | Motor: OFF | RPM: 0.0
========================================
```

---

## 7. Lógica de Operación del Posicionador en Taller

1. **Reposo (Estado `IDLE`):**
   * El motor permanece desenergizado (`ENABLE = HIGH`), el eje está libre y el driver frío.
   * El operario puede colocar la pieza sobre el plato giratorio y seleccionar el sentido de giro deseado con el switch `D7`.
2. **Puesta en Marcha:**
   * El operario ajusta la velocidad deseada en el potenciómetro (ej. 50%).
   * Presiona una vez el pulsador `D8` (*START*).
   * El sistema pasa a **`ACCELERATING`**: el motor arranca suavemente aplicando una rampa de aceleración de 500 ms (sin tirones que desalineen la pieza soldada).
   * Al alcanzar la velocidad objetivo, pasa automáticamente a **`RUNNING`**.
3. **Ajuste en caliente:**
   * Durante el giro, el operario puede girar el potenciómetro para acelerar o desacelerar según el cordón de soldadura.
4. **Parada:**
   * Al presionar nuevamente el botón `D8` o llevar el potenciómetro al mínimo (<1%), el sistema pasa a **`DECELERATING`**, frena suavemente hasta velocidad cero y regresa a **`IDLE`**.
5. **Protección de Giro:**
   * Si el operario cambia el switch de sentido mientras el plato está girando, el firmware **ignora el cambio** hasta que el motor se detenga por completo, protegiendo los engranajes, la correa GT2 y el motor.
