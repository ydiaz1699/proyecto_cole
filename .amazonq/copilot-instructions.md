# Copilot Instructions: Posicionador Motorizado NEMA17

Este repositorio es un proyecto de **control de motor paso a paso NEMA17 para posicionador de soldadura con Arduino Nano**.

## Objetivo
Controlar un motor NEMA17 con FSM de 5 estados, aceleración lineal y filtrado ADC para posicionar piezas metálicas en trabajos de soldadura.

## Configuración del proyecto
- Plataforma: `Arduino Nano (ATmega328P, 16MHz, 2KB RAM, 32KB Flash)`
- Framework/Librerías principales: `Arduino Core AVR, LiquidCrystal (opcional)`
- Herramienta de build: `PlatformIO`
- Board/Entorno: `nanoatmega328`

## Datos clave del proyecto
- STEP_DELAY_MIN_US: `500` (µs — máxima velocidad = 300 RPM motor)
- STEP_DELAY_MAX_US: `5000` (µs — mínima velocidad = 30 RPM motor)
- ACCEL_INTERVAL_MS: `20` (ms — tick de rampa de aceleración)
- ACCEL_TICKS_TOTAL: `25` (ticks — rampa completa = 500ms)
- ADC_FILTER_SIZE: `8` (muestras — promedio móvil con shift >>3)
- DEBOUNCE_MS: `50` (ms — anti-rebote de pulsador)
- VREF del A4988: `1.34V` (fórmula: 1.68A × 8 × 0.1Ω)

## Reglas de implementación
- NUNCA usar `delay()` en loop() — usar `micros()`/`millis()` non-blocking.
- NUNCA usar la clase `String` — fragmenta heap en 2KB RAM. Usar `F("texto")`.
- NUNCA cambiar dirección del motor mientras gira — solo en estado IDLE.
- NUNCA usar `tone()` — comparte Timer2 con pin STEP (D3).
- Pines ocupados: D3(STEP), D4(DIR), D5(EN), D7(Switch), D8(Botón), D9-D11(MS1-3), A0(Pot).
- Toda constante de tiempo DEBE tener comentario con fórmula justificativa.
- Idioma: comentarios en español, identificadores en inglés (camelCase).
- El código debe compilar con `pio run -e nano`.
- Preferir FSM con enum+switch, NO booleanos sueltos.

## Estilo de respuesta
- Responder en español.
- Incluir solo el código necesario y una breve explicación.
- No agregar contenido irrelevante.

## Nota
Este archivo es una guía para agentes de asistencia de código y no forma parte del software. Mantenerlo actualizado con la configuración del proyecto.
