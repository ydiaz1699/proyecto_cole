# Posicionador Motorizado con Motor Paso a Paso NEMA17

> Proyecto de Grado — Colegio Nacional San Luis, Sección Industrial  
> Mecánica Industrial — Tarija, Bolivia  
> Firmware v2.0 (Nivel 2 — Robusto)

## Descripción

Sistema electromecánico con plato giratorio accionado por motor paso a paso NEMA17, controlado por Arduino Nano, para posicionar piezas metálicas en ángulos controlados durante trabajos de soldadura.

**Nivel de madurez del código:** Nivel 2 (Robusto)  
Aplicando la metodología de [guia-crecimiento-de-codigo.md](https://github.com/ydiaz1699/analisis_de_codigo/blob/main/guia-crecimiento-de-codigo.md):

```
┌─────────────────────────────────────────────┐
│ NIVEL 3: CALIBRADO CON HARDWARE REAL        │  ← Próximo paso
│   Parámetros ajustados con mediciones       │
├─────────────────────────────────────────────┤
│ NIVEL 2: ROBUSTO ← ESTAMOS AQUÍ            │
│   FSM, filtrado ADC, aceleración,           │
│   constantes documentadas, modular          │
├─────────────────────────────────────────────┤
│ NIVEL 1: FUNCIONAL MÍNIMO (v1.0)           │
│   Código monolítico, magic numbers          │
└─────────────────────────────────────────────┘
```

## Características del Firmware v2.0

- **FSM de 5 estados**: IDLE → ACCELERATING → RUNNING → DECELERATING → ERROR
- **Filtrado ADC**: Promedio móvil de 8 muestras (elimina ruido ±4 LSB)
- **Aceleración lineal**: Rampa de 500ms (evita pérdida de pasos al arrancar)
- **Protección de dirección**: Solo cambia en estado IDLE (evita daño mecánico)
- **Código modular**: 4 módulos independientes con responsabilidad única
- **Constantes documentadas**: Cada valor tiene fórmula + efectos Si↑/Si↓
- **Zero delay()**: Todo el loop es non-blocking (excepto init)
- **Compilación condicional**: LCD y Serial debug habilitables con flags

## Estructura del Proyecto

```
proyecto_cole/
├── .ai/                          # Contexto para asistentes IA (vsCode-AI v3)
│   ├── PROJECT_CONTEXT.md        # Overview del proyecto
│   ├── HARDWARE.md               # Pines, consumo, advertencias
│   ├── SOFTWARE.md               # Dependencias, compilación
│   ├── SKILL.md                  # Reglas NUNCA para la IA
│   ├── ARCHITECTURE.md           # FSM, módulos, diagrama
│   ├── DECISIONS.md              # Decisiones técnicas justificadas
│   ├── TASKS.md                  # Pendientes / completados
│   ├── CHANGELOG.md              # Historial de cambios
│   └── ROADMAP.md                # Plan futuro (v2.5, v3.0)
├── src/                          # Código fuente Nivel 2
│   ├── main.cpp                  # Entry point (setup + loop limpio)
│   ├── config.h                  # TODAS las constantes con fórmulas
│   ├── motor_controller.h/.cpp   # FSM del motor + aceleración
│   ├── input_manager.h/.cpp      # Filtrado ADC + debounce
│   └── display_manager.h/.cpp    # LCD + Serial rate-limited
├── catalog/                      # Fichas técnicas (formato vsCode-AI)
│   ├── boards/arduino-nano.md    # Specs del Arduino Nano
│   └── peripherals/stepper-a4988.md  # Specs del driver A4988
├── codigo_arduino/               # Código Nivel 1 (legacy, referencia)
│   ├── posicionador_nema17.ino   # Versión básica original
│   └── posicionador_nema17_lcd.ino   # Versión LCD original
├── esquemas/                     # Diagramas de conexión
│   └── esquema_electrico.md      # Wiring completo
├── calculos/                     # Análisis técnico mecánico/eléctrico
│   └── calculos_tecnicos.md      # Torque, reducción, RPM, esfuerzos
├── documentacion/                # Perfil de proyecto (documento escolar)
├── analisis_posicionador.md      # Análisis profundo 3 capas (timing, bugs)
└── README.md                     # Este archivo
```

## Especificaciones Técnicas

| Parámetro | Valor | Fórmula/Justificación |
|:----------|:------|:----------------------|
| Motor | NEMA17 JK42HS40-1684 | 1.68A, 3.6 kg·cm, 200 pasos/vuelta |
| Driver | A4988 (Allegro) | VREF = 1.68×8×0.1 = 1.34V |
| MCU | ATmega328P (Arduino Nano) | 16MHz, 2KB RAM, 32KB Flash |
| RPM mínima | 30 | 60M/(5000µs×2×200) |
| RPM máxima | 300 | 60M/(500µs×2×200) |
| Rampa aceleración | 500ms | 25 ticks × 20ms/tick |
| Filtro ADC | 8 muestras | Shift >>3 en vez de división |
| Debounce | 50ms | > rebote típico 10-30ms |
| Loop time | ~140µs (sin LCD) | analogRead+FSM+STEP |
| Consumo total | ~1.76A @ 12V | Motor 1.68A + lógica 0.08A |

## Quick Start

### Requisitos
- Arduino IDE 2.x (o PlatformIO)
- Arduino Nano con chip CH340 o FTDI
- NO requiere librerías externas (versión sin LCD)

### Compilar y subir

```bash
# Con Arduino IDE:
# 1. Abrir src/main.cpp
# 2. Board: Arduino Nano
# 3. Port: COM correspondiente
# 4. Upload

# Con PlatformIO:
pio run --target upload
pio device monitor
```

### Uso

1. Conectar hardware según `esquemas/esquema_electrico.md`
2. Ajustar VREF del driver a **1.34V** (ver `.ai/HARDWARE.md`)
3. Encender con interruptor general
4. Girar potenciómetro: velocidad (30-300 RPM)
5. Switch: dirección CW/CCW (solo en motor parado)
6. Botón: marcha/parada (toggle con aceleración)

### Monitor Serial (debug)

```
========================================
 POSICIONADOR MOTORIZADO NEMA17 v2.0
 Colegio Nacional San Luis - Tarija
========================================
[FSM] IDLE -> ACCELERATING
[STATUS] State=ACCEL | RPM=45.0 | Dir=CW | Pot=512 | Delay=2750us
[FSM] ACCELERATING -> RUNNING
[STATUS] State=RUN | RPM=109.1 | Dir=CW | Pot=512 | Delay=2750us
```

## Metodologías Aplicadas

Este proyecto aplica tres frameworks desarrollados en repositorios hermanos:

| Framework | Repo | Qué aporta |
|:----------|:-----|:-----------|
| Guía de Crecimiento de Código | [analisis_de_codigo](https://github.com/ydiaz1699/analisis_de_codigo) | Patrón Nivel 1→2→3 |
| Skill Análisis Profundo | [analisis_de_codigo](https://github.com/ydiaz1699/analisis_de_codigo) | 3 capas de debugging |
| Context Engineering v3 | [vsCode-AI](https://github.com/ydiaz1699/vsCode-AI) | Documentación .ai/ + catálogo |

## Autores

- **Jhonny Elías Díaz Moyata** — Estudiante 6to Secundaria
- **Rodrigo Ángel Nava Flores** — Estudiante 6to Secundaria
- **Profesor:** Marco Antonio Vargas
- **Institución:** Colegio Nacional San Luis, Sección Industrial

## Licencia

Proyecto educativo — Uso libre para fines académicos y de aprendizaje.

---
*Gestión 2025 — Tarija, Bolivia*
