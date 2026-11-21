# Posicionador Motorizado con Motor Paso a Paso NEMA17

> Proyecto de Grado — Colegio Nacional San Luis, Sección Industrial
> Mecánica Industrial — Tarija, Bolivia
> Firmware v2.0 (Nivel 2 — Robusto)

---

## Descripción

Sistema electromecánico con plato giratorio accionado por motor paso a paso NEMA17, controlado por Arduino Nano, para posicionar piezas metálicas en ángulos controlados durante trabajos de soldadura.

El firmware implementa una **máquina de estados finitos (FSM)** de 5 estados con generación manual de pulsos, filtrado digital del potenciómetro, y rampa de aceleración/deceleración lineal.

```
┌─────────────────────────────────────────────┐
│ NIVEL 3: CALIBRADO CON HARDWARE REAL        │  ← Próximo paso
│   Parámetros ajustados con mediciones       │
├─────────────────────────────────────────────┤
│ NIVEL 2: ROBUSTO  ← ESTAMOS AQUÍ            │
│   FSM, filtrado ADC, aceleración,           │
│   constantes documentadas, modular          │
├─────────────────────────────────────────────┤
│ NIVEL 1: FUNCIONAL MÍNIMO (v1.0)            │
│   Código monolítico, magic numbers          │
└─────────────────────────────────────────────┘
```

---

## Características del Firmware v2.0

| Característica | Implementación |
|:---------------|:---------------|
| FSM de 5 estados | IDLE → ACCELERATING → RUNNING → DECELERATING → ERROR |
| Filtrado ADC | Promedio móvil de 8 muestras con shift `>>3` |
| Aceleración lineal | Rampa de 500ms (25 ticks × 20ms × 180µs/tick) |
| Protección de dirección | Solo cambia en estado IDLE (evita daño mecánico) |
| Código modular | 4 módulos independientes con responsabilidad única |
| Constantes documentadas | Cada valor tiene fórmula + análisis Si↑/Si↓ |
| Zero `delay()` | Todo el loop es non-blocking (~140µs/ciclo) |
| Compilación condicional | LCD y Serial debug habilitables con flags |
| Estado ERROR | Motor se deshabilita, LED parpadea, recuperación con botón |
| Zona muerta | Pot <1% = motor OFF (evita arranque por ruido) |

---

## Especificaciones Técnicas

| Parámetro | Valor | Fórmula/Fuente |
|:----------|:------|:---------------|
| Motor | NEMA17 JK42HS40-1684 | 1.68A, 3.6 kg·cm, 200 pasos/vuelta |
| Driver | A4988 (Allegro) | VREF = 1.68×8×0.1 = **1.34V** |
| MCU | ATmega328P (Arduino Nano) | 16MHz, 2KB RAM, 32KB Flash |
| RPM motor (min/max) | 30 / 300 | 60M/(5000×2×200) / 60M/(500×2×200) |
| RPM plato (con GT2 3:1) | 10 / 100 | RPM_motor / 3 |
| Rampa aceleración | 500ms | 25 ticks × 20ms/tick |
| Filtro ADC | 8 muestras | Shift >>3 (sin división) |
| Debounce | 50ms | > rebote típico 10-30ms |
| Loop time (sin LCD) | ~140µs | ADC(104µs) + FSM(20µs) + misc(16µs) |
| Consumo total | ~1.76A @ 12V | Motor 1.68A + lógica 0.08A |
| Torque en plato | ~10.8 kg·cm | 3.6 × reducción 3:1 |
| Carga máx horizontal | 5 kg | Con poleas GT2 20T/60T |
| Costo total | ~616 Bs (~89 USD) | Todos los componentes |


---

## Arquitectura del Sistema

```
┌─────────────────────────────────────────────────────────────┐
│                   Arduino Nano (ATmega328P)                 │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  main.cpp ─── setup() + loop() (coordinación)               │
│      │                                                      │
│      ├── InputManager        ← Filtrado ADC, debounce       │
│      │     └── struct InputState                            │
│      │                                                      │
│      ├── MotorController     ← FSM + generación STEP        │
│      │     └── struct MotorStatus                           │
│      │                                                      │
│      └── DisplayManager      ← LCD + Serial (rate-limited)  │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│  HARDWARE EXTERNO                                           │
│  ┌────────┐  ┌──────────┐  ┌──────────┐  ┌─────────────┐    │
│  │ A4988  │  │NEMA17    │  │ Pot 10kΩ │  │ Switches    │    │
│  │ Driver │──│Motor     │  │ (veloc.) │  │ DIR + START │    │
│  └────────┘  └──────────┘  └──────────┘  └─────────────┘    │
└─────────────────────────────────────────────────────────────┘
```

### Máquina de Estados (FSM)

```
         ┌──[btn + pot>0]──→ ACCELERATING ──[vel=target]──→ RUNNING
         │                        │                            │
  ──→ IDLE ←──[vel=0]── DECELERATING ←──[btn || pot=0]────────┘
         │                        ↑
         │                        │
         └──────── ERROR ←────────┘ (timeout / fallo)
                     │
                     └──[btn]──→ IDLE (recuperación)
```

---

## Estructura del Proyecto

```
proyecto_cole/
├── .ai/                              # Contexto para asistentes IA
│   ├── PROJECT_CONTEXT.md            # Overview del proyecto
│   ├── ARCHITECTURE.md               # FSM, módulos, diagramas
│   ├── HARDWARE.md                   # Pines, consumo, advertencias
│   ├── SOFTWARE.md                   # Dependencias, compilación
│   ├── SKILL.md                      # Reglas NUNCA, criterios de salida
│   ├── DECISIONS.md                  # Decisiones técnicas justificadas
│   ├── TASKS.md                      # Pendientes / completados
│   ├── CHANGELOG.md                  # Historial de cambios
│   └── ROADMAP.md                    # Plan futuro (v2.5, v3.0)
├── src/                              # Código fuente Nivel 2 (PRINCIPAL)
│   ├── main.cpp                      # Entry point (setup + loop limpio)
│   ├── config.h                      # TODAS las constantes con fórmulas
│   ├── motor_controller.h/.cpp       # FSM del motor + aceleración
│   ├── input_manager.h/.cpp          # Filtrado ADC + debounce
│   └── display_manager.h/.cpp        # LCD + Serial rate-limited
├── docs/                             # Documentación técnica
│   ├── guia_sistema_final.md         # Guía de conexión y puesta en marcha final
│   ├── guia_prueba_motor.md          # Guía del banco de pruebas por Serial
│   ├── esquema_electrico.md          # Esquema eléctrico detallado y lista de componentes
│   ├── notas.md                      # Notas HW, calibración, troubleshooting
│   ├── copilot-instructions.md       # Instrucciones para asistentes IA
│   └── conexiones.drawio.svg         # Diagrama de conexiones visual
├── codigo_arduino/                   # Código Nivel 1 (legacy, referencia)
│   ├── posicionador_nema17.ino       # Versión básica original
│   └── posicionador_nema17_lcd.ino   # Versión LCD original
├── catalog/                          # Fichas técnicas (formato vsCode-AI)
│   ├── boards/arduino-nano.md        # Specs del Arduino Nano
│   └── peripherals/stepper-a4988.md  # Specs del driver A4988
├── esquemas/                         # Diagramas de conexión
├── calculos/                         # Análisis técnico (torque, RPM, esfuerzos)
│   └── calculos_tecnicos.md          # Cálculos completos
├── documentacion/                    # Perfil de proyecto (documento escolar)
│   └── perfil_proyecto.md            # Documento formal de grado
├── archivo-mapa.yml                  # Mapa exhaustivo del sistema
├── analisis_posicionador.md          # Análisis profundo 3 capas (timing, bugs)
└── README.md                         # Este archivo
```


---

## Quick Start

### Requisitos

- Arduino IDE 2.x (o PlatformIO con VSCode)
- Arduino Nano (chip CH340 o FTDI)
- Cable USB Mini-B
- NO requiere librerías externas (versión sin LCD)

### Compilar y subir

**Arduino IDE:**
1. Abrir `src/main.cpp` (o copiar contenido de `src/` a un sketch)
2. Board: `Arduino Nano`
3. Processor: `ATmega328P` (probar "Old Bootloader" si falla)
4. Port: COM correspondiente
5. Upload

**PlatformIO:**
```bash
pio run -e nano                    # Compilar
pio run -e nano --target upload    # Subir al Arduino
pio device monitor --baud 9600     # Monitor serial
```

### Configuración platformio.ini

```ini
[env:nano]
platform = atmelavr
board = nanoatmega328
framework = arduino
monitor_speed = 9600
upload_speed = 115200
build_flags =
    -DLCD_ENABLED=0
    -DDEBUG_SERIAL=1
    -Wall
```

---

## Uso

### Controles

| Control | Pin | Función |
|:--------|:---:|:--------|
| Potenciómetro 10kΩ | A0 | Velocidad (girar derecha = más rápido) |
| Switch SPDT | D7 | Dirección CW/CCW (solo cambia con motor parado) |
| Pulsador | D8 | Marcha/Parada (toggle: presionar = arrancar/parar) |
| LED built-in | D13 | Indicador: OFF=parado, ON=girando, BLINK=error |

### Secuencia de operación

1. Conectar hardware según la [Guía de Conexión del Sistema Final](docs/guia_sistema_final.md) o diagrama (`docs/conexiones.drawio.svg`).
2. **ANTES DE ENCENDER**: Ajustar VREF del driver a **0.92V** (para motor KS42STH34-1504A, ver [docs/guia_prueba_motor.md](docs/guia_prueba_motor.md)).
3. Encender fuente 12V.
4. Girar potenciómetro a la posición de velocidad deseada
5. Seleccionar dirección con switch (CW o CCW)
6. Presionar botón → Motor arranca con aceleración gradual (500ms)
7. Ajustar velocidad en tiempo real con potenciómetro
8. Presionar botón de nuevo → Motor se detiene gradualmente
9. Cambiar dirección (solo con motor parado)

### Monitor Serial (debug)

Conectar USB y abrir monitor serial a 9600 baud:

```
========================================
 POSICIONADOR MOTORIZADO NEMA17 v2.0
 Colegio Nacional San Luis - Tarija
========================================
 FSM: IDLE -> ACCEL -> RUN -> DECEL
 ADC: Filtro promedio movil (8 muestras)
 Aceleración: Rampa lineal 500ms
========================================

[FSM] IDLE -> ACCELERATING
[STATUS] State=ACCEL | RPM=45.0 | Dir=CW | Pot=512 | Delay=2750us
[FSM] ACCELERATING -> RUNNING
[STATUS] State=RUN | RPM=109.1 | Dir=CW | Pot=512 | Delay=2750us
[STATUS] State=RUN | RPM=109.1 | Dir=CW | Pot=512 | Delay=2750us
[FSM] RUNNING -> DECELERATING (boton)
[FSM] DECELERATING -> IDLE
```

---

## Conexiones Principales

```
Arduino Nano          A4988 Driver          NEMA17 Motor
────────────          ────────────          ────────────
D3  ──────────────→  STEP
D4  ──────────────→  DIR
D5  ──────────────→  ENABLE
D9  ──────────────→  MS1 (LOW)
D10 ──────────────→  MS2 (LOW)
D11 ──────────────→  MS3 (LOW)
5V  ──────────────→  VDD
GND ──────────────→  GND (lógica)
                     RST ←→ SLP (puenteados + 5V)
                     VMOT ←── +12V fuente       1A ────→ Bobina A+ (Rojo)
                     GND  ←── GND fuente        1B ────→ Bobina A- (Azul)
                     [100µF entre VMOT y GND]   2A ────→ Bobina B+ (Verde)
                                                2B ────→ Bobina B- (Negro)

Arduino Nano          Controles
────────────          ─────────
A0  ←─────── Cursor del Potenciómetro 10kΩ (extremos: 5V y GND)
D7  ←─────── Switch SPDT (COM a D7, NO a GND)
D8  ←─────── Pulsador (un terminal a D8, otro a GND)
D13 ─────→   LED indicador (built-in)
```

> ⚠️ **Ver diagrama completo:** `docs/conexiones.drawio.svg`
> ⚠️ **Colores de cables del motor varían por fabricante — VERIFICAR con multímetro**


---

## Advertencias Críticas

> ⚠️ **NUNCA conectar/desconectar el motor con el driver energizado** — Los picos inductivos destruyen el chip A4988 instantáneamente.

> ⚠️ **Capacitor 100µF OBLIGATORIO** entre VMOT y GND del driver — Sin él, los picos del motor destruyen el driver.

> ⚠️ **Ajustar VREF ANTES de conectar motor** — Con Rsense=0.1Ω: VREF = 1.34V. Exceder este valor sobrecalienta motor y driver.

> ⚠️ **Timer compartido en D3** — NUNCA usar `tone()` ni ninguna librería que modifique Timer2.

> ⚠️ **2KB RAM es MUY POCO** — Usar macro `F("texto")` para strings. NUNCA usar la clase `String`.

---

## Metodologías Aplicadas

Este proyecto aplica tres frameworks desarrollados en repositorios hermanos:

| Framework | Repositorio | Qué aporta |
|:----------|:------------|:-----------|
| Guía de Crecimiento de Código | [analisis_de_codigo](https://github.com/ydiaz1699/analisis_de_codigo) | Patrón Nivel 1→2→3 de madurez |
| Skill Análisis Profundo | [analisis_de_codigo](https://github.com/ydiaz1699/analisis_de_codigo) | Debugging en 3 capas (estático/dinámico/eliminación) |
| Context Engineering v3 | [Context_Engineering_V2](https://github.com/ydiaz1699/Context_Engineering_V2) | Documentación .ai/ + catálogo hardware |

---

## Roadmap

```
┌─────────┐     ┌─────────┐     ┌─────────┐     ┌─────────┐
│  v1.0   │────→│  v2.0   │────→│  v2.5   │────→│  v3.0   │
│Funcional│     │ Robusto │     │Calibrado│     │   Pro   │
│ (DONE)  │     │(ACTUAL) │     │ (NEXT)  │     │(FUTURE) │
└─────────┘     └─────────┘     └─────────┘     └─────────┘
```

| Versión | Objetivo | Criterio de avance |
|:--------|:---------|:-------------------|
| **v2.0** (actual) | FSM + filtrado + aceleración + modular | Código compilable, documentado |
| **v2.5** (próxima) | Calibrar parámetros con hardware real | Hardware armado + mediciones reales |
| **v3.0** (futuro) | Modo angular, pedal, EEPROM | 2+ features Pro implementadas |

### Features v3.0 planificados:
- Modo posicionamiento angular (pot = ángulo 0-360°)
- Pedal de pie (start/stop sin manos para soldar)
- Fin de carrera (endstop) como referencia 0°
- Memoria de velocidad en EEPROM
- Escala angular en LCD

---

## Cotización de Materiales

| # | Componente | Código (Sawers / Ref.) | Precio (Bs) | Precio (USD) |
|:-:|:-----------|:----------------------:|:-----------:|:------------:|
| 1 | Motor NEMA17 KS42STH34-1504A | `KS42STH34-1504A` | 100 | ~14.5 |
| 2 | Driver A4988 / HR4988 | **`00154`** | 25 | ~3.6 |
| 3 | Arduino Uno R3 SMD (CH340) | **`00014`** | 55 | ~8.0 |
| 4 | Fuente 12V/3A | Ref. 12V-3A | 50 | ~7.2 |
| 5 | Poleas GT2 20T+60T + correa | GT2-Set | 80 | ~11.6 |
| 6 | Chumaceras UCP204 ×2 | UCP204 | 80 | ~11.6 |
| 7 | Perfil 40×40mm (varios metros) | Ferretería | 120 | ~17.4 |
| 8 | Plancha acero 5mm (plato) | Maestranza | 60 | ~8.7 |
| 9 | Switch selector dirección (1 tiempo) | **`00129`** | 10 | ~1.4 |
| 10 | Electrónica restante (pot, botón, cables, fusible, cap) | Sawers / Varios | 36 | ~5.3 |
| | **TOTAL** | | **~616 Bs** | **~89 USD** |

*Precios referencia Tarija, Bolivia — Gestión 2025*

---

## Autores

- **Jhonny Elías Díaz Moyata** — Estudiante 6to Secundaria
- **Rodrigo Ángel Nava Flores** — Estudiante 6to Secundaria
- **Profesor:** Marco Antonio Vargas
- **Institución:** Colegio Nacional San Luis, Sección Industrial

## Licencia

Proyecto educativo — Uso libre para fines académicos y de aprendizaje.

---

*Gestión 2025 — Tarija, Bolivia*
