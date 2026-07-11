# Posicionador Motorizado NEMA17 — Hardware

## Referencias

### Placa Principal
- **Componente**: Arduino Nano (ATmega328P)
- **Referencia**: catalog/boards/arduino-nano.md
- **MCU**: ATmega328P @ 16MHz
- **RAM**: 2KB SRAM
- **Flash**: 32KB (0.5KB bootloader)
- **Voltaje lógico**: 5V
- **Alimentación**: 7-12V (VIN) / 5V (USB)

### Periféricos

| Componente | Protocolo | Cantidad | Voltaje | Notas |
|:-----------|:----------|:--------:|:-------:|:------|
| Motor NEMA17 JK42HS40-1684 | Step/Dir (vía driver) | 1 | 12V | 1.68A/fase, 3.6 kg·cm |
| Driver A4988 | Digital GPIO | 1 | 5V lógica / 12V motor | Rsense=0.1Ω |
| Potenciómetro 10kΩ lineal | ADC (analógico) | 1 | 5V | Tipo B, regulación velocidad |
| Switch SPDT | Digital INPUT_PULLUP | 1 | 5V | Selector CW/CCW |
| Pulsador N.A. | Digital INPUT_PULLUP | 1 | 5V | Marcha/Parada toggle |
| LCD 16x2 HD44780 | Paralelo 4-bit | 1 | 5V | Opcional (versión avanzada) |
| Capacitor 100µF/25V | — | 1 | — | Protección VMOT del driver |

## Tabla de Conexiones (Wiring)

| Periférico | Pin Periférico | Pin Arduino | Función | Notas |
|:-----------|:--------------|:-----------:|:--------|:------|
| A4988 | STEP | D3 | Pulsos de paso | PWM-capable, Timer2 |
| A4988 | DIR | D4 | Dirección giro | Digital output |
| A4988 | ENABLE | D5 | Habilitar driver | LOW=activo, HIGH=sleep |
| A4988 | MS1 | D9 | Microstepping bit 0 | Ver tabla microstepping |
| A4988 | MS2 | D10 | Microstepping bit 1 | Ver tabla microstepping |
| A4988 | MS3 | D11 | Microstepping bit 2 | Ver tabla microstepping |
| A4988 | VDD | 5V | Lógica driver | Desde Arduino 5V |
| A4988 | GND (lógica) | GND | Tierra lógica | Común con Arduino |
| A4988 | VMOT | +12V ext | Motor power | Desde fuente DC |
| A4988 | GND (motor) | GND ext | Tierra motor | Desde fuente DC |
| A4988 | RST | → SLP | Reset | Puentear a SLP |
| A4988 | SLP | → RST + 5V | Sleep | Puentear a RST y +5V |
| A4988 | 1A | Motor Rojo | Bobina A+ | Verificar con multímetro |
| A4988 | 1B | Motor Azul | Bobina A- | Verificar con multímetro |
| A4988 | 2A | Motor Verde | Bobina B+ | Verificar con multímetro |
| A4988 | 2B | Motor Negro | Bobina B- | Verificar con multímetro |
| Potenciómetro | Terminal 1 | 5V | Referencia alta | Extremo izquierdo |
| Potenciómetro | Cursor | A0 | Lectura velocidad | Pin analógico |
| Potenciómetro | Terminal 3 | GND | Referencia baja | Extremo derecho |
| Switch SPDT | Común | D7 | Selector dirección | INPUT_PULLUP |
| Switch SPDT | NO | GND | Posición CCW | Activa LOW |
| Pulsador | Terminal 1 | D8 | Botón marcha | INPUT_PULLUP |
| Pulsador | Terminal 2 | GND | — | Activa LOW |
| LCD RS | — | A1 | Register Select | Solo versión LCD |
| LCD EN | — | A2 | Enable | Solo versión LCD |
| LCD D4 | — | A3 | Data bit 4 | Solo versión LCD |
| LCD D5 | — | A4 | Data bit 5 | Solo versión LCD |
| LCD D6 | — | A5 | Data bit 6 | Solo versión LCD |
| LCD D7 | — | D2 | Data bit 7 | Solo versión LCD |

## Pines Libres (sin LCD)

| Pin | Tipo | Disponible para |
|:---:|:----:|:----------------|
| D2 | Digital (INT0) | Sensor endstop, encoder, emergencia |
| D6 | Digital PWM | LED indicador, buzzer |
| D12 | Digital | Sensor, relé |
| D13 | Digital (LED) | LED estado (built-in) |
| A1-A5 | Analógico/Digital | Sensores adicionales |

## Consumo Energético

| Estado | Corriente (12V) | Duración típica | Notas |
|:-------|:---------------:|:---------------:|:------|
| Idle (motor deshabilitado) | ~50 mA | Variable | Solo Arduino activo |
| Motor holding (parado con ENABLE) | ~1.68 A | Mientras sostiene carga | Máxima corriente por fase |
| Motor girando (carga nominal) | ~1.2 A | Operación normal | Depende de velocidad y carga |
| Motor girando (sin carga) | ~0.8 A | Pruebas | Corriente reducida |
| **Pico arranque** | **~2.0 A** | **<100ms** | Transitorio al arrancar |

## Alimentación
- **Fuente**: Adaptador AC-DC 12V / 2A (mínimo), recomendado 12V / 3A
- **Protección**: Fusible 2.5A slow-blow + capacitor 100µF en VMOT

## Advertencias

> ⚠️ **NUNCA conectar/desconectar el motor con el driver energizado** — Los picos inductivos destruyen el chip A4988 instantáneamente.

> ⚠️ **Capacitor 100µF OBLIGATORIO** entre VMOT y GND del driver — Sin él, los picos del motor pueden alcanzar 60V+ y destruir el driver.

> ⚠️ **Ajustar VREF ANTES de conectar motor** — Con Rsense=0.1Ω: VREF_A4988 = 1.68A × 8 × 0.1Ω = 1.34V. Exceder este valor sobrecalienta el motor y el driver.

> ⚠️ **Timer compartido en D3** — El pin D3 usa Timer2 para PWM. Si se usa tone() o alguna librería que modifique Timer2, los pulsos STEP se corrompen.

> ⚠️ **LCD ocupa A1-A5 y D2** — En versión LCD, los pines analógicos A1-A5 NO están disponibles para sensores. El pin D2 (INT0) tampoco.

> ⚠️ **2KB RAM es MUY POCO** — Usar macro F("texto") para strings en Serial. NO usar la clase String. Buffers estáticos con tamaño fijo.
