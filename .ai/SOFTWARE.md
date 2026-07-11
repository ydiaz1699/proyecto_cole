# Posicionador Motorizado NEMA17 — Software

## Entorno de Desarrollo

| Parámetro | Valor |
|:----------|:------|
| IDE | Arduino IDE 2.x / PlatformIO |
| Framework | Arduino |
| Board | Arduino Nano (ATmega328P) |
| Compilador | avr-gcc |
| Upload speed | 115200 baud |
| Monitor speed | 9600 baud |

## Dependencias

### Librerías Externas

| Librería | Versión | Uso | Obligatoria |
|:---------|:--------|:----|:-----------:|
| LiquidCrystal | Built-in Arduino | Display LCD 16x2 | Solo versión LCD |

### Librerías del Proyecto (Custom)

| Módulo | Archivo | Responsabilidad |
|:-------|:--------|:----------------|
| MotorController | `src/motor_controller.h/.cpp` | FSM del motor, generación de pasos, aceleración |
| InputManager | `src/input_manager.h/.cpp` | Lectura filtrada de pot, debounce botón, selector |
| DisplayManager | `src/display_manager.h/.cpp` | Actualización LCD con rate limiting (solo v. LCD) |
| Config | `src/config.h` | Todas las constantes del proyecto (pines, timing, límites) |

## Configuración de Compilación

```ini
; platformio.ini (equivalente)
[env:nano]
platform = atmelavr
board = nanoatmega328
framework = arduino
monitor_speed = 9600
upload_speed = 115200
build_flags =
    -DVERSION_MAJOR=2
    -DVERSION_MINOR=0
    -DLCD_ENABLED=0
```

## Uso de Memoria (estimado)

| Recurso | Usado | Disponible | % |
|:--------|:-----:|:----------:|:-:|
| Flash (sin LCD) | ~8 KB | 32 KB | 25% |
| Flash (con LCD) | ~12 KB | 32 KB | 37% |
| RAM (sin LCD) | ~400 bytes | 2048 bytes | 20% |
| RAM (con LCD) | ~600 bytes | 2048 bytes | 29% |

## Build Flags

| Flag | Valor | Propósito |
|:-----|:------|:----------|
| `LCD_ENABLED` | 0 o 1 | Habilitar/deshabilitar código LCD |
| `DEBUG_SERIAL` | 0 o 1 | Habilitar/deshabilitar prints de debug |
| `VERSION_MAJOR` | 2 | Versión mayor del firmware |
| `VERSION_MINOR` | 0 | Versión menor del firmware |

## Notas Técnicas

1. **No usar delay()** — Todo el timing es non-blocking con `micros()` y `millis()`
2. **analogRead() filtrado** — Promedio móvil de 8 muestras para evitar ruido del ADC
3. **FSM explícita** — Estados del motor definidos con enum, transiciones con switch-case
4. **Aceleración lineal** — Rampa de velocidad para evitar pérdida de pasos al arrancar
5. **Debounce por software** — 50ms para botones, filtrado para potenciómetro
