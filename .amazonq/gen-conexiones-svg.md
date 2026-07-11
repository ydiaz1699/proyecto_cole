# Genera diagrama de conexiones (drawio SVG)

## Objetivo
Crear un archivo `docs/conexiones.drawio.svg` con el diagrama de conexiones del proyecto actual.

## Instrucciones

1. **Analiza el proyecto**: Lee los archivos fuente del workspace para identificar:
   - Microcontrolador/placa principal (Arduino Nano ATmega328P)
   - Sensores, actuadores y módulos conectados (A4988, NEMA17, Pot, Switch, Pulsador)
   - Pines utilizados: D3(STEP), D4(DIR), D5(EN), D7(Switch), D8(Botón), D9(MS1), D10(MS2), D11(MS3), A0(Pot), D13(LED)
   - Voltajes de operación: 5V lógica, 12V motor

2. **Genera el SVG** en formato drawio editable:
   - Rectángulo azul `#dae8fc` → Arduino Nano
   - Rectángulo verde `#d5e8d4` → A4988 Driver
   - Rectángulo naranja `#fff2cc` → Motor NEMA17
   - Rectángulos verdes pequeños → Pot, Switch, Pulsador
   - Flechas de conexión con colores:
     - Rojo `#ff0000` → alimentación (12V, 5V)
     - Negro `#000000` → GND
     - Verde `#00aa00` → señal digital (STEP, DIR, EN, MS1-3)
     - Azul `#0000ff` → señal analógica (A0)
   - Fuente Helvetica, 12px pines, 14px bold nombres

3. **Guarda** el archivo en `docs/conexiones.drawio.svg`.

## Criterios de calidad
- El SVG debe ser abrible y editable en draw.io/diagrams.net
- Cada componente muestra SOLO los pines usados en ESTE proyecto
- Layout horizontal: periféricos a la izquierda, Arduino al centro, motor a la derecha
