# Posicionador Motorizado NEMA17 — Architecture

## System Overview

```
┌─────────────────────────────────────────────────────────────┐
│                   Arduino Nano (ATmega328P)                  │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────────────────────────────────────────────┐  │
│  │              APPLICATION LAYER                        │  │
│  │                                                      │  │
│  │  main.cpp                                            │  │
│  │  ├── setup() → initHardware, initDisplay, startFSM  │  │
│  │  └── loop() → readInputs, updateFSM, generateSteps  │  │
│  └──────────────────────────────────────────────────────┘  │
│                           │                                 │
│  ┌────────────┬───────────┼───────────┬──────────────────┐ │
│  │            │           │           │                  │ │
│  ▼            ▼           ▼           ▼                  │ │
│ ┌────────┐ ┌────────┐ ┌────────┐ ┌────────────────────┐ │ │
│ │ Input  │ │ Motor  │ │Display │ │   Config (const)   │ │ │
│ │Manager │ │Control │ │Manager │ │                    │ │ │
│ │        │ │  (FSM) │ │        │ │ Pines, tiempos,    │ │ │
│ │• ADC   │ │• IDLE  │ │• LCD   │ │ límites, fórmulas  │ │ │
│ │• Debounce│• ACCEL │ │• Serial│ │                    │ │ │
│ │• Filter│ │• RUN   │ │• Rate  │ │                    │ │ │
│ │        │ │• DECEL │ │  Limit │ │                    │ │ │
│ │        │ │• ERROR │ │        │ │                    │ │ │
│ └────┬───┘ └────┬───┘ └────┬───┘ └────────────────────┘ │ │
│      │          │          │                             │ │
│  ┌───┴──────────┴──────────┴─────────────────────────────┘ │
│  │           HARDWARE ABSTRACTION LAYER                     │
│  │                                                         │
│  │  GPIO: digitalRead/Write, analogRead                    │
│  │  Timer: micros(), millis()                              │
│  │  Serial: 9600 baud (debug)                              │
│  │  LCD: LiquidCrystal (paralelo 4-bit)                    │
│  └─────────────────────────────────────────────────────────┘
│                                                             │
├─────────────────────────────────────────────────────────────┤
│  HARDWARE EXTERNO                                           │
│  ┌────────┐  ┌──────────┐  ┌──────────┐  ┌─────────────┐  │
│  │ A4988  │  │NEMA17    │  │ Pot 10kΩ │  │ Switches    │  │
│  │ Driver │──│Motor     │  │ (veloc.) │  │ DIR + START │  │
│  └────────┘  └──────────┘  └──────────┘  └─────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## Máquina de Estados (FSM) del Motor

```
                    ┌──────────────────────────────────────┐
                    │                                      │
                    ▼                                      │
             ┌──────────┐                                 │
     ┌──────►│   IDLE   │◄──────────────────────┐        │
     │       └──────┬───┘                       │        │
     │              │                            │        │
     │              │ [botón=MARCHA              │        │
     │              │  && pot>ZONA_MUERTA]       │        │
     │              ▼                            │        │
     │       ┌──────────────┐                   │        │
     │       │ ACCELERATING │                   │        │
     │       └──────┬───────┘                   │        │
     │              │                            │        │
     │              │ [velocidad_actual          │        │
     │              │  >= velocidad_target]      │        │
     │              ▼                            │        │
     │       ┌──────────┐     [pot<ZONA_MUERTA  │        │
     │       │ RUNNING  │──────|| botón=STOP]───┤        │
     │       └──────┬───┘                       │        │
     │              │                            │        │
     │              │ [cambio de velocidad       ▼        │
     │              │  requiere decelerar] ┌──────────────┐
     │              └─────────────────────►│ DECELERATING │
     │                                     └──────┬───────┘
     │                                            │
     │              [velocidad=0]                  │
     │              ┌─────────────────────────────┘
     │              ▼
     │       ┌──────────┐
     │       │   IDLE   │
     │       └──────────┘
     │
     │       ┌──────────┐
     └───────│  ERROR   │◄── [timeout sin pulsos, sobrecorriente, etc.]
             └──────────┘
             Recuperación: botón STOP → IDLE
```

### Estados y Responsabilidades

| Estado | Qué hace | Transición de salida |
|:-------|:---------|:---------------------|
| IDLE | Motor DISABLED, espera comando | → ACCELERATING (botón + pot>0) |
| ACCELERATING | Incrementa velocidad cada 20ms hasta target | → RUNNING (vel=target) |
| RUNNING | Genera pasos a velocidad constante | → DECELERATING (stop/cambio) |
| DECELERATING | Reduce velocidad cada 20ms hasta 0 | → IDLE (vel=0) |
| ERROR | Motor DISABLED, LED parpadea | → IDLE (botón STOP) |

### Eventos que la FSM procesa

| Evento | Source | Efecto |
|:-------|:-------|:-------|
| BTN_START | Pulsador (flanco bajada, debounce 50ms) | IDLE→ACCEL, RUNNING→DECEL |
| POT_CHANGE | Potenciómetro (filtrado, cambio >5) | Actualiza velocidad_target |
| DIR_CHANGE | Switch SPDT (cambio de estado) | Solo en IDLE: cambia DIR pin |
| POT_ZERO | Potenciómetro en zona muerta (<10) | Cualquier estado → DECEL → IDLE |
| TIMEOUT | Sin pulsos generados por >1s | → ERROR |

## Directory Structure

```
proyecto_cole/
├── .ai/                          # Contexto para asistentes IA
│   ├── PROJECT_CONTEXT.md        # Descripción general
│   ├── HARDWARE.md               # Hardware, pines, consumo
│   ├── SOFTWARE.md               # Dependencias, compilación
│   ├── SKILL.md                  # Reglas NUNCA, criterios
│   ├── ARCHITECTURE.md           # Este archivo (FSM, módulos)
│   ├── DECISIONS.md              # Decisiones técnicas con justificación
│   ├── TASKS.md                  # Pendientes y completados
│   ├── CHANGELOG.md              # Historial de cambios
│   └── ROADMAP.md                # Plan futuro
├── src/                          # Código fuente (Nivel 2)
│   ├── main.cpp                  # Entry point (o .ino)
│   ├── config.h                  # Constantes con fórmulas
│   ├── motor_controller.h        # FSM del motor
│   ├── motor_controller.cpp      #
│   ├── input_manager.h           # Filtrado + debounce
│   ├── input_manager.cpp         #
│   ├── display_manager.h         # LCD con rate limiting
│   └── display_manager.cpp       #
├── esquemas/                     # Diagramas de conexión
├── calculos/                     # Análisis técnico
├── documentacion/                # Perfil de proyecto original
├── analisis_posicionador.md      # Análisis profundo (3 capas)
└── README.md                     # Documentación principal
```

## Design Principles

1. **Non-blocking** — NUNCA delay() en loop. Todo con micros()/millis()
2. **FSM explícita** — Enum de estados + switch-case, NO booleanos sueltos
3. **Filtrado en entrada** — ADC con promedio móvil, botones con debounce
4. **Constantes documentadas** — Cada valor numérico tiene nombre + fórmula en config.h
5. **Modular** — Cada responsabilidad en su propio archivo .h/.cpp
6. **Fail-safe** — Estado ERROR con recuperación, motor se deshabilita ante anomalía
