# Copilot Instructions

## Proyecto
Control de motor paso a paso NEMA17 con Arduino Nano + A4988. FSM de 5 estados, filtrado ADC, aceleración lineal. Framework: Arduino sobre PlatformIO. Plataforma: atmelavr (ATmega328P).

## Reglas de Código
- Framework: Arduino (PlatformIO, NO Arduino IDE)
- MCU: ATmega328P (2KB RAM, 32KB Flash) — optimizar memoria siempre
- Idioma: comentarios en español, identificadores en inglés
- Estilo: camelCase variables, PascalCase clases, SCREAMING_CASE constantes
- Indentación: 4 espacios
- Usar `static const` o `constexpr` en vez de `#define` para constantes tipadas
- Usar `micros()`/`millis()` para timing, NUNCA `delay()` en loop

## Anti-patrones (NO hacer)
- NO usar `delay()` en loop() — bloquea generación de pasos STEP
- NO usar `String` class — fragmenta heap en 2KB RAM
- NO cambiar dirección mientras motor gira — daña mecánica
- NO usar `tone()` — comparte Timer2 con pin STEP (D3)
- NO exceder 300 RPM motor / 100 RPM plato — torque insuficiente
- NO energizar driver sin capacitor 100µF en VMOT

## Contexto
Lee estos archivos antes de sugerir código:
- `.ai/PROJECT_CONTEXT.md` — restricciones y goals
- `.ai/HARDWARE.md` — pines ocupados y voltajes
- `.ai/SKILL.md` — reglas NUNCA del proyecto
- `.ai/ARCHITECTURE.md` — FSM y módulos

## Patrones Preferidos
- FSM con `enum` + `switch` (no booleanos sueltos)
- Timers no bloqueantes: `if (micros() - last >= interval)`
- Filtrado ADC: promedio móvil con shift (>>3) en vez de división
- Constantes con fórmula documentada en config.h
- `F("texto")` para strings en Serial (ahorra RAM)

## Compilación
```bash
pio run -e nano
pio run -e nano --target upload
pio device monitor --baud 9600
```
