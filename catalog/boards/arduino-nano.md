---
id: "arduino-nano"
name: "Arduino Nano"
mcu: "ATmega328P"
architecture: "AVR 8-bit"
cores: 1
clock_mhz: 16
ram_kb: 2
flash_kb: 32
voltage_logic: 5.0
voltage_vin: "7-12V (VIN) / 5V (USB Mini-B)"
gpio_total: 22
gpio_restricted: [0, 1]
gpio_input_only: []
adc_bits: 10
adc_channels: 8
dac_channels: 0
pwm_channels: 6
wifi: false
bluetooth: false
usb_native: false
uart_chip: "CH340 / FTDI FT232RL"
platform: "atmelavr"
board: "nanoatmega328"
framework: "arduino"
monitor_speed: 9600
current_active_ma: 19
current_sleep_ua: 0
price_usd: 6
price_bs: 35
---

# Arduino Nano

## Especificaciones

| Parámetro | Valor |
|-----------|-------|
| MCU | ATmega328P |
| Arquitectura | AVR 8-bit |
| Núcleos | 1 |
| Frecuencia | 16 MHz |
| RAM | 2 KB SRAM |
| Flash | 32 KB (2KB bootloader) |
| EEPROM | 1 KB |
| Voltaje Lógico | 5V |
| Voltaje Alimentación | 7-12V (VIN) / 5V (USB) |
| Dimensiones | 18mm × 45mm |

## Conectividad

- WiFi: No disponible
- Bluetooth: No disponible
- USB: Mini-B (CH340 o FTDI como bridge)
- UART: 1x hardware (pines D0/D1)

## Mapeo de Pines

| Función | Pines |
|---------|-------|
| GPIO Totales | 22 (D0-D13 + A0-A7) |
| GPIO Restringidos | D0/D1 (UART — evitar si se usa Serial) |
| Solo Entrada | A6, A7 (solo ADC, no tienen digital) |
| ADC (8 canales) | A0-A7 (10 bits, 0-5V) |
| DAC | No disponible |
| PWM (6 canales) | D3, D5, D6, D9, D10, D11 (~) |
| I2C | SDA=A4, SCL=A5 |
| SPI | MOSI=D11, MISO=D12, SCK=D13, SS=D10 |
| UART | TX=D1, RX=D0 |
| Interrupciones externas | INT0=D2, INT1=D3 (solo estos dos) |

## Niveles de Voltaje

- Lógica: 5V (HIGH ≥ 3V, LOW ≤ 1.5V)
- Entrada: 5V tolerant en todos los pines digitales
- ADC: Referencia interna 1.1V o AVCC (5V) seleccionable
- Corriente máxima por pin: 40mA (recomendado 20mA)

## Timers

| Timer | Bits | Canales PWM | Uso default Arduino |
|-------|------|-------------|---------------------|
| Timer0 | 8-bit | D5, D6 | millis(), micros(), delay() |
| Timer1 | 16-bit | D9, D10 | Servo library |
| Timer2 | 8-bit | D3, D11 | tone() |

> ⚠️ **PWM en D5/D6 tiene frecuencia diferente** (976Hz) que D3/D9/D10/D11 (490Hz) porque Timer0 tiene prescaler diferente para mantener millis() preciso.

## Consideraciones Críticas para Este Proyecto

1. **2KB RAM es MUY poco** — Usar macro `F("texto")` para strings en flash. NUNCA usar la clase String (fragmenta heap). Buffers estáticos con tamaño fijo.

2. **Solo 2 interrupciones externas** — INT0 (D2) e INT1 (D3). En este proyecto D3 se usa para STEP, lo cual ocupa INT1. Solo queda INT0 (D2) libre para endstop o encoder futuro.

3. **Timer2 comparte con D3 PWM** — El pin STEP (D3) comparte Timer2 con tone(). Si se usa tone() para alarma sonora, corrompe la generación de pasos. **NUNCA usar tone() con este proyecto.**

4. **analogRead() es bloqueante** — Cada lectura toma ~104µs. Con nuestro filtro de 1 lectura cada 10ms, el impacto es mínimo (1% CPU).

5. **A6 y A7 son solo analógicos** — No tienen función digital. No pueden usarse para botones o LEDs, solo para sensores analógicos adicionales.

6. **CH340 vs FTDI** — Los Nano clones chinos usan CH340. En Windows requiere driver CH340G. En Linux/Mac suele funcionar sin driver adicional.

7. **VIN soporta 7-12V** — El regulador interno AMS1117 (SMD) puede manejar ~500mA a 12V sin sobrecalentarse. Nuestro Arduino consume ~19mA + periféricos.

## Consumo Energético

| Estado | Corriente | Notas |
|--------|-----------|-------|
| Activo (16MHz, periféricos ON) | ~19 mA | Sin carga en pines |
| Con A4988 lógica + pot + switches | ~25 mA | Estimado para este proyecto |
| Sleep Power-Down | ~0.1 µA | No aplicable (siempre activo) |

## platformio.ini de referencia

```ini
[env:nano]
platform = atmelavr
board = nanoatmega328
framework = arduino
monitor_speed = 9600
upload_speed = 115200
build_flags =
    -DLCD_ENABLED=0
    -DDEBUG_SERIAL=1
```

## Proyectos que usan esta placa

- **Posicionador Motorizado NEMA17** — Control de motor paso a paso con A4988, potenciómetro, y FSM de 5 estados.

## Referencias

- [Arduino Nano Pinout](https://www.arduino.cc/en/uploads/Main/ArduinoNanoManual23.pdf)
- [ATmega328P Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)
- [CH340G Driver](http://www.wch-ic.com/downloads/CH341SER_ZIP.html)
