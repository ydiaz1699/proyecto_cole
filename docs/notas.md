# Notas de Hardware

## Voltajes Críticos
- Driver A4988 VMOT: **12V DC** (mínimo 8V, máximo 35V)
- Lógica A4988 VDD: **5V** (desde Arduino)
- Motor NEMA17: controlado por driver, NUNCA directo desde Arduino (1.68A >> 40mA pin)

## Errores Comunes
- **Sin capacitor 100µF en VMOT** → picos inductivos destruyen A4988 instantáneamente
- **Conectar/desconectar motor con driver energizado** → back-EMF de 60V+ quema chip
- **VREF mal ajustado** → muy alto: sobrecalienta motor/driver. Muy bajo: pierde pasos
- **Colores de cable del motor incorrectos** → medir continuidad con multímetro SIEMPRE

## Tips de Debugging
- Motor no gira: verificar ENABLE=LOW, VREF>0.5V, conexión VMOT+capacitor
- Motor vibra pero no gira: bobinas invertidas (intercambiar 1A↔1B o 2A↔2B)
- Pierde pasos: VREF muy bajo, velocidad muy alta, o carga excesiva
- Serial no imprime: verificar baud rate 9600, CH340 driver instalado

## Quirks
- `delay(5)` en cambio de dirección: solo se ejecuta en IDLE (seguro)
- LCD_ENABLED=1 puede perder pasos a >100 RPM (LCD bloquea 1.3ms)
- `micros()` overflow a 71.58 min: aritmética unsigned lo maneja correctamente
- Pin D3 (STEP) comparte Timer2: NUNCA usar `tone()` en este proyecto
