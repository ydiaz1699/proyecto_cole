# Posicionador Motorizado NEMA17 — Changelog

## [2.0.0] — 2025-07-11

### Added
- Estructura .ai/ completa siguiendo framework vsCode-AI v3
- FSM explícita con 5 estados (IDLE, ACCEL, RUN, DECEL, ERROR)
- Filtrado de ADC con promedio móvil de 8 muestras
- Rampa de aceleración/deceleración lineal (500ms)
- Archivo config.h con todas las constantes documentadas
- Análisis profundo del sistema (analisis_posicionador.md)
- Ficha de hardware referenciando catálogo vsCode-AI
- Documentación DECISIONS.md con justificaciones de cada elección

### Changed
- Código refactorizado de monolítico a modular (4 módulos)
- Lógica de booleanos reemplazada por FSM enum + switch
- Lectura de potenciómetro ahora filtrada (antes cruda)
- Generación de pasos con control de aceleración (antes velocidad instantánea)
- Cambio de dirección solo permitido en IDLE (antes en cualquier momento)

### Fixed
- Potencial corrupción de estado por cambio de dirección en movimiento
- Ruido en velocidad por lectura sin filtrar del ADC
- Arranque brusco que podía causar pérdida de pasos

## [1.0.0] — 2025-07-11

### Added
- Código Arduino funcional básico (Nivel 1)
- Control por potenciómetro + selector + botón
- Versión con LCD 16x2
- Esquema eléctrico completo
- Cálculos técnicos (torque, reducción, RPM, esfuerzos)
- README con instrucciones de uso
