# Prompt: Generar documentación completa para proyecto_cole

> Pega este prompt en un chat NUEVO sin contexto previo.
> Adjunta o pega el Repomix del proyecto después de este prompt.

---

## INSTRUCCIONES

Eres un generador de documentación especializado en firmware embebido.
Te voy a dar el código completo de un proyecto (formato Repomix).
Tu trabajo es EJECUTAR cada template de la carpeta `.amazonq/` contra el código
y producir los archivos de resultado LLENADOS con datos reales del proyecto.

## REGLAS INVIOLABLES

1. **Todo dato viene del código** — NO inventar. Si no puedes determinar algo, escribe `[VERIFICAR]`.
2. **Nivel de detalle EXHAUSTIVO** — Como el ejemplo que te muestro abajo. No resumir, no simplificar.
3. **Un archivo a la vez** — Genera cada archivo completo, luego pasa al siguiente.
4. **Idioma español** — Excepto código y términos técnicos.

## ARCHIVOS A GENERAR (en este orden)

| # | Template (instrucciones) | Archivo de salida | Ubicación final |
|---|--------------------------|-------------------|-----------------|
| 1 | `.amazonq/generar-archivo-mapa.md` | `archivo-mapa.yml` | Raíz del proyecto |
| 2 | `.amazonq/gen-notas-hw.md` | `notas.md` | `docs/notas.md` |
| 3 | `.amazonq/copilot-instructions-template.md` | `copilot-instructions.md` | `docs/copilot-instructions.md` |
| 4 | `.amazonq/SKILL-template.md` | `SKILL.md` | `.ai/SKILL.md` |
| 5 | `.amazonq/generar-readme.md` | `README.md` | Raíz del proyecto |
| 6 | `.amazonq/gen-conexiones-svg.md` | `conexiones.drawio.svg` | `docs/conexiones.drawio.svg` |

## PROCEDIMIENTO POR ARCHIVO

Para cada archivo:
1. Lee el template correspondiente de `.amazonq/` (está en el código que te paso)
2. Lee TODO el código fuente del proyecto (especialmente `src/`, `platformio.ini`, `.ai/`)
3. Genera el archivo de resultado LLENADO siguiendo las instrucciones del template
4. Muestra el archivo completo en un bloque de código
5. Pasa al siguiente

## NIVEL DE DETALLE ESPERADO

El `archivo-mapa.yml` debe tener el nivel de detalle de este ejemplo (de otro proyecto):

```yaml
# Ejemplo de NIVEL DE DETALLE esperado (extracto):

limitaciones_criticas:
  - id: "lcd_blocking"
    descripcion: >
      LiquidCrystal::print() bloquea ~40µs por carácter.
      32 chars × 40µs = 1280µs. Si STEP_DELAY_MIN=500µs,
      se pierde 1 paso cada vez que el LCD actualiza.
    impacto: "Error angular de 7.2°/segundo a velocidad máxima"
    solucion: "LCD_ENABLED=0 por defecto. Solo activar si velocidad <100 RPM"

  - id: "timer2_compartido"
    descripcion: >
      Pin D3 (STEP) usa Timer2 para PWM. Si se usa tone() o
      cualquier librería que modifique Timer2, los pulsos STEP
      se corrompen silenciosamente.
    impacto: "Motor se detiene o gira erráticamente sin error visible"
    solucion: "NUNCA usar tone(). Si necesitas buzzer, usar Timer1 en D9/D10"

flujos_principales:
  diagrama_conceptual: |
    setup():
      └── inputManager.init() → configura pines, pre-llena filtro ADC (8 muestras)
      └── motorController.init() → pines salida, deshabilita motor, microstepping
      └── displayManager.init() → Serial.begin(9600), bienvenida
    
    loop() infinito (~50-140µs por ciclo):
      1. inputManager.update()
         ├── Cada 10ms: analogRead(A0) → actualizar buffer circular → promedio >>3
         ├── Cada ciclo: debounce botón D8 (50ms window)
         └── Cada ciclo: leer switch D7 (detectar cambio)
      2. motorController.update(inputs)
         ├── FSM switch: ejecutar handler del estado actual
         ├── _generateStep(): if (micros()-last >= currentDelay) toggle PIN_STEP
         └── _updateRPM(): calcular 60M/(currentDelay×2×200)
      3. displayManager.update(status, inputs)
         └── Cada 2000ms: Serial.print status line
```

El `SKILL.md` debe tener el nivel de este ejemplo:

```markdown
## Decisiones clave
- Generación manual de pulsos STEP con micros() toggle (NO AccelStepper) — transparencia educativa
- FSM con enum+switch porque booleanos permiten estados imposibles (motorActivo=true && acelerando=true && decelerando=true)
- Filtro >>3 en vez de /8 porque ATmega328P no tiene división hardware

## Criterios de salida
- Código compila con `pio run -e nano` sin warnings (-Wall)
- RAM < 70% (< 1433 bytes de 2048)
- Flash < 80% (< 25600 bytes de 32768)
- No hay delay() en loop() ni en funciones llamadas desde loop()
- Toda constante tiene comentario con fórmula: `// 60M/(300×2×200) = 500µs`
```

El `copilot-instructions.md` debe tener el nivel de este ejemplo:

```markdown
## Datos clave del proyecto
- STEP_DELAY_MIN_US: `500` (µs — 300 RPM motor = 60M/(300×2×200))
- STEP_DELAY_MAX_US: `5000` (µs — 30 RPM motor = 60M/(30×2×200))
- ACCEL_STEP_US: `180` (µs por tick — (5000-500)/25 ticks)
- VREF A4988: `1.34V` (1.68A × 8 × 0.1Ω)
- Loop time sin LCD: `~140µs` (analogRead 104µs + FSM 20µs + misc 16µs)
- Loop time con LCD: `~1440µs` (¡MAYOR que STEP_DELAY_MIN! → pierde pasos)

## Reglas de implementación
- NUNCA delay() en loop — bloquea _generateStep() y pierde pulsos STEP
- NUNCA String class — 2KB RAM, heap fragmentation inevitable
- NUNCA cambiar DIR mientras motor gira — daño mecánico a poleas GT2
- NUNCA tone() — Timer2 compartido con pin D3 (STEP)
- NUNCA LCD a >100 RPM — 1.3ms de bloqueo > 500µs entre pasos = pierde pasos
```

## CONTEXTO DEL PROYECTO

El proyecto es un **Posicionador Motorizado con NEMA17** para taller de soldadura:
- Arduino Nano (ATmega328P) controla motor paso a paso via driver A4988
- FSM de 5 estados: IDLE → ACCELERATING → RUNNING → DECELERATING → ERROR
- Potenciómetro para velocidad (30-300 RPM), switch para dirección, botón para marcha
- Código modular: main.cpp + config.h + motor_controller + input_manager + display_manager
- Proyecto educativo (Colegio Nacional San Luis, Tarija, Bolivia)

## AHORA PEGA EL CÓDIGO DEL PROYECTO (Repomix)

[Pegar aquí el contenido de Repomix del proyecto]
