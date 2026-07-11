# Prompt: Generar README.md

## Objetivo
Analiza el proyecto proporcionado y genera un `README.md` completo, profesional y orientado al usuario final.

## Qué debe analizar
1. **Propósito**: Posicionador motorizado con NEMA17 para taller de soldadura
2. **Requisitos**: Arduino Nano, A4988, NEMA17, fuente 12V/2A, pot, switches
3. **Instalación**: PlatformIO, pio run, pio upload
4. **Arquitectura**: 4 módulos + FSM de 5 estados
5. **Configuración**: config.h con todas las constantes
6. **Problemas comunes**: VREF, capacitor, colores motor, Timer2/tone()

## Reglas de generación
1. Orientado a la acción: de "cloné" a "funciona" siguiendo el README
2. Comandos copiables en bloques de código
3. Tabla de troubleshooting: mínimo 5 problemas con solución directa
4. Emojis en headers H2 para navegabilidad
5. Tipo: Embedded/IoT → incluir diagrama de conexiones, pinout, consumo
6. Longitud apropiada: autónomo pero no excesivo
7. Incluir sección de metodologías aplicadas (vsCode-AI, analisis_de_codigo)

## Criterios de calidad
- Usuario nuevo instala y ejecuta en <5 minutos
- Cada error tiene solución concreta
- Arquitectura se entiende en 30 segundos
