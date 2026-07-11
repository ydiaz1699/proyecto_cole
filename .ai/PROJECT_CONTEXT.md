# Posicionador Motorizado NEMA17 — Project Context

## Overview
- **Project**: Posicionador Motorizado de Escala para Taller de Soldadura
- **Description**: Sistema electromecánico con plato giratorio accionado por motor paso a paso NEMA17, controlado por Arduino, para posicionar piezas metálicas en ángulos controlados durante trabajos de soldadura.
- **Board**: Arduino Nano (ATmega328P)
- **Framework**: Arduino
- **Motor**: NEMA17 JK42HS40-1684 (1.68A, 3.6 kg·cm, 200 pasos/vuelta)
- **Driver**: A4988 o DRV8825
- **Generated**: 2025-07-11
- **Methodology**: vsCode-AI Context Engineering v3 + Análisis Profundo 3 Capas

## Hardware Summary
- **MCU**: ATmega328P (16MHz, 2KB RAM, 32KB Flash)
- **Motor**: NEMA17 bipolar 4 hilos, 1.8°/paso
- **Driver**: A4988 (Allegro) — hasta 2A/fase, 35V, microstepping 1/16
- **Alimentación**: Fuente DC 12V / 2A
- **Interfaz usuario**: Potenciómetro 10kΩ + Switch SPDT + Pulsador N.A.
- **Display**: LCD 16x2 (versión avanzada)
- **WiFi**: No
- **Bluetooth**: No

## Project Goals
1. Posicionar piezas metálicas (5-15 kg) con velocidad regulable y dirección reversible
2. Mantener código robusto Nivel 2: FSM, filtrado, aceleración, protecciones
3. Seguir las convenciones del framework vsCode-AI para documentación IA
4. Demostrar integración de mecánica + electricidad + programación

## Constraints
- **RAM**: 2KB — Prohibido usar String class, usar F() para literales
- **Flash**: 32KB — Monitorear uso con compilador
- **Interrupciones**: Solo 2 externas (D2, D3) — D3 usado para STEP (Timer)
- **Timers**: Timer0 (millis), Timer1 (disponible), Timer2 (disponible)
- **PWM**: D3, D5, D6, D9, D10, D11 — D3 usado para STEP
- **ADC**: 10-bit (0-1023), 6 canales (A0-A5)
- **Corriente motor**: 1.68A requiere ajuste VREF preciso en driver
- **Voltaje**: 5V lógica, 12V motor — NUNCA mezclar

## Context for AI Assistants
- Leer HARDWARE.md para restricciones de pines y voltaje
- Leer SKILL.md para reglas NUNCA del proyecto
- Leer ARCHITECTURE.md para entender la FSM del sistema
- El código DEBE compilar con Arduino IDE o PlatformIO
- Toda constante de tiempo debe tener fórmula documentada (ver DECISIONS.md)
