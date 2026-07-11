# Posicionador Motorizado NEMA17 — Roadmap

## Versión Actual: v2.0 (Nivel 2 — Robusto)

```
┌─────────┐     ┌─────────┐     ┌─────────┐     ┌─────────┐
│  v1.0   │────►│  v2.0   │────►│  v2.5   │────►│  v3.0   │
│ Funcional│     │ Robusto │     │ Calibrado│    │ Pro     │
│ (DONE)  │     │(ACTUAL) │     │ (NEXT)  │     │(FUTURE) │
└─────────┘     └─────────┘     └─────────┘     └─────────┘
```

## v2.5 — Calibrado con Hardware Real (Próxima)

Objetivo: Ajustar todos los parámetros con el sistema físico armado.

| Feature | Prioridad | Complejidad | Depende de |
|:--------|:---------:|:-----------:|:-----------|
| Calibrar zona muerta real del pot | Alta | Baja | Hardware armado |
| Medir RPM reales vs calculadas | Alta | Baja | Motor funcionando |
| Ajustar rampa de aceleración con carga | Alta | Media | Reducción instalada |
| Verificar que no pierde pasos a 50 RPM | Alta | Media | Carga en plato |
| Ajustar LCD update rate según CPU load | Baja | Baja | LCD instalado |

## v3.0 — Características Pro (Futuro)

| Feature | Prioridad | Complejidad | Beneficio |
|:--------|:---------:|:-----------:|:----------|
| Modo posicionamiento angular (pot=ángulo) | Alta | Media | Soldar en ángulo fijo |
| Pedal de pie (start/stop sin manos) | Alta | Baja | Manos libres para soldar |
| Fin de carrera (endstop) como referencia 0° | Media | Baja | Repetibilidad |
| Escala angular en LCD (0°-360°) | Media | Baja | Feedback visual |
| Memoria de velocidad (guardar en EEPROM) | Baja | Baja | No reajustar cada vez |
| Comunicación Serial para control externo | Baja | Media | Automatización |
| Protector anti-salpicadura del motor | Media | — | Mecánico, no SW |
| Base con ruedas | Baja | — | Mecánico, no SW |
| Alimentación 12V batería (portátil) | Baja | Baja | Uso en campo |

## Criterios de Avance de Versión

| De → A | Criterio |
|:-------|:---------|
| v2.0 → v2.5 | Hardware armado + parámetros calibrados con mediciones reales |
| v2.5 → v3.0 | Al menos 2 features Pro implementadas y probadas |

## Deuda Técnica

| Item | Impacto | Esfuerzo para resolver |
|:-----|:--------|:----------------------|
| Sin detección de pérdida de pasos | Medio (raro con buena reducción) | Alto (requiere encoder) |
| Sin watchdog timer | Bajo (sistema supervisado) | Bajo (agregar wdt_enable) |
| Serial debug siempre activo | Bajo (consume CPU mínimo) | Bajo (flag DEBUG_SERIAL) |
