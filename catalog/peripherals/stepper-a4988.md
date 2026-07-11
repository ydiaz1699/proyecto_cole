---
id: "stepper-a4988"
name: "Driver A4988 para Motor Paso a Paso"
protocol: "Digital (STEP/DIR/ENABLE)"
category: "Actuador - Motor"
voltage_logic: "3.3-5V"
voltage_motor: "8-35V"
current_max: "2A por fase (con disipador)"
library: "(nativo — solo digitalWrite)"
price_usd: 3
price_bs: 25
---

# Driver A4988 (Allegro MicroSystems)

## Especificaciones

| Parámetro | Valor |
|-----------|-------|
| Chip | Allegro A4988 |
| Voltaje motor (VMOT) | 8V — 35V DC |
| Voltaje lógico (VDD) | 3.3V — 5V |
| Corriente máxima | 2A/fase (1A sin disipador) |
| Microstepping | Full, 1/2, 1/4, 1/8, 1/16 |
| Protecciones | Térmica, sobrecorriente, bajo voltaje |
| Rsense típico | 0.050Ω o 0.100Ω (verificar en PCB) |

## Pinout

```
        +-----+
 ENABLE | 1  16| VMOT
    MS1 | 2  15| GND (motor)
    MS2 | 3  14| 2B
    MS3 | 4  13| 2A
  RESET | 5  12| 1A
  SLEEP | 6  11| 1B
   STEP | 7  10| VDD
    DIR | 8   9| GND (logic)
        +-----+
```

## Tabla de Microstepping

| MS1 | MS2 | MS3 | Resolución | Pasos/vuelta (NEMA17) |
|:---:|:---:|:---:|:-----------|:---------------------:|
| L | L | L | Full step | 200 |
| H | L | L | Half step | 400 |
| L | H | L | 1/4 step | 800 |
| H | H | L | 1/8 step | 1600 |
| H | H | H | 1/16 step | 3200 |

## Cálculo de VREF

```
VREF = I_max × 8 × Rsense

Para NEMA17 JK42HS40-1684 (1.68A) con Rsense=0.100Ω:
VREF = 1.68 × 8 × 0.100 = 1.344V

Para Rsense=0.050Ω:
VREF = 1.68 × 8 × 0.050 = 0.672V
```

> ⚠️ Verificar el valor de Rsense en la PCB del driver (R100=0.1Ω, R050=0.05Ω)

## Timing Crítico

| Parámetro | Mínimo | Típico | Máximo |
|-----------|--------|--------|--------|
| Pulso STEP (HIGH) | 1µs | — | — |
| DIR setup antes de STEP | 200ns | — | — |
| ENABLE a activo | — | — | 1µs |
| Wake de SLEEP | — | 1ms | — |

## Conexión con Arduino para este proyecto

| Pin A4988 | Conecta a | Notas |
|-----------|-----------|-------|
| VMOT | +12V fuente DC | Con capacitor 100µF |
| GND (motor) | GND fuente DC | Tierra de potencia |
| VDD | 5V Arduino | Lógica |
| GND (logic) | GND Arduino | Tierra de lógica |
| STEP | D3 (Arduino) | Pulsos de paso |
| DIR | D4 (Arduino) | Dirección |
| ENABLE | D5 (Arduino) | LOW=activo |
| MS1 | D9 (Arduino) | Microstepping |
| MS2 | D10 (Arduino) | Microstepping |
| MS3 | D11 (Arduino) | Microstepping |
| RESET | → SLEEP | Puentear juntos |
| SLEEP | → RESET + 5V | Puentear a VDD |
| 1A, 1B | Motor bobina A | Verificar con multímetro |
| 2A, 2B | Motor bobina B | Verificar con multímetro |

## Advertencias

> ⚠️ **Capacitor 100µF OBLIGATORIO** entre VMOT y GND. Sin él, picos inductivos del motor destruyen el chip.

> ⚠️ **NUNCA conectar/desconectar motor con driver energizado.** Los picos de back-EMF pueden alcanzar 60V+.

> ⚠️ **Disipador recomendado** si la corriente supera 1A por fase. Sin disipador, la protección térmica limita a ~1A.

> ⚠️ **RESET y SLEEP deben estar HIGH** para que el driver funcione. Puentear ambos a VDD si no se usan.

## Diferencias con DRV8825

| Característica | A4988 | DRV8825 |
|---------------|-------|---------|
| Voltaje máximo | 35V | 45V |
| Corriente máxima | 2A | 2.5A |
| Microstepping máximo | 1/16 | 1/32 |
| VREF formula | I×8×Rs | I×5×Rs (o I/2) |
| Pin FAULT | No | Sí |
| Compatibilidad física | — | Drop-in replacement |

## Referencias

- [A4988 Datasheet (Allegro)](https://www.allegromicro.com/en/products/motor-drivers/brush-dc-motor-drivers/a4988)
- [Pololu A4988 Carrier](https://www.pololu.com/product/1182)
