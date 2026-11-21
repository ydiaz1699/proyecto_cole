# ESQUEMA ELECTRICO - POSICIONADOR MOTORIZADO NEMA17

## Diagrama de Conexiones Completo

```
  220V AC --> [INTERRUPTOR ON/OFF] --> [FUSIBLE 2A] --> [FUENTE 12V/2A DC]
                                                           |      |
                                                         +12V    GND
                                                           |      |
              +--------------------------------------------+------+------+
              |                ARDUINO NANO/UNO                          |
              |                                                          |
              |  VIN <--- +12V fuente                                    |
              |  GND <--- GND fuente                                     |
              |                                                          |
              |  D3  (STEP)   ---------> A4988 pin STEP                  |
              |  D4  (DIR)    ---------> A4988 pin DIR                   |
              |  D5  (ENABLE) ---------> A4988 pin ENABLE                |
              |  D7  (SELECTOR) <------- Switch SPDT                     |
              |  D8  (BOTON)    <------- Pulsador N.A.                   |
              |  D9  (MS1)    ---------> A4988 pin MS1                   |
              |  D10 (MS2)    ---------> A4988 pin MS2                   |
              |  D11 (MS3)    ---------> A4988 pin MS3                   |
              |  A0  (POT)    <------- Potenciometro 10kOhm              |
              +----------------------------------------------------------+
```

---

## Conexion del Driver A4988/DRV8825

```
                    +---------------------+
                    |      A4988          |
                    |   (Vista superior)  |
                    |                     |
   Arduino D5 ----->| ENABLE        VMOT  |<---- +12V Fuente
                    |                     |
   Arduino D9 ----->| MS1           GND   |<---- GND Fuente
                    |                     |
   Arduino D10 ---->| MS2           2B    |-----> Motor Bobina B (-)
                    |                     |
   Arduino D11 ---->| MS3           2A    |-----> Motor Bobina B (+)
                    |                     |
         +5V ------>| RST           1A    |-----> Motor Bobina A (+)
                    |                     |
         +5V ------>| SLP           1B    |-----> Motor Bobina A (-)
                    |                     |
   Arduino D3 ----->| STEP          VDD   |<---- +5V Arduino
                    |                     |
   Arduino D4 ----->| DIR           GND   |<---- GND Arduino
                    |                     |
                    +---------------------+
                              |
                     +--------+--------+
                     | CAPACITOR 100uF |  (entre VMOT y GND)
                     | electrolitico   |
                     +-----------------+

  IMPORTANTE: Conectar RST y SLP juntos a +5V (VDD) para que funcione.
```



---

## Conexion del Motor NEMA17 (KS42STH34-1504A / JK42HS40-1684, 4 Hilos)

```
  IMPORTANTE: Con el conector blanco de 6 pines conectado al motor:
  
  Bobina A (Pines 1 y 4 del motor):
    Cable ROJO  ----> Bobina A1 ----> Pin 1B del driver
    Cable VERDE ----> Bobina A2 ----> Pin 1A del driver
    
  Bobina B (Pines 3 y 6 del motor):
    Cable AZUL  ----> Bobina B1 ----> Pin 2A del driver
    Cable NEGRO ----> Bobina B2 ----> Pin 2B del driver

  ADVERTENCIA DE CABLEADO:
  Los cables de 4 a 6 pines estilo impresora 3D traen en la ficha negra de 4 pines 
  el orden Rojo-Azul-Verde-Negro. Si se conecta directo, las bobinas quedan mezcladas 
  y el motor solo vibra/suena sin girar. 
  DEBEN conectarse las parejas: (Rojo + Verde) en 1B/1A y (Azul + Negro) en 2A/2B.
```

---

## Conexion del Potenciometro de Velocidad (10kOhm)

```
  +5V ----> Pin 1 (extremo izquierdo)
  A0  ----> Pin 2 (cursor central) ----> Arduino A0
  GND ----> Pin 3 (extremo derecho)

  Girar izquierda = 0V = Motor LENTO/PARADO
  Girar derecha   = 5V = Motor RAPIDO
```

---

## Conexion del Selector de Direccion

```
  Arduino D7 (INPUT_PULLUP) ----+
                                |
                            [SWITCH]
                                |
                               GND

  Switch ABIERTO: D7 lee HIGH (pull-up) = Giro HORARIO (CW)
  Switch CERRADO: D7 lee LOW (a GND)    = Giro ANTIHORARIO (CCW)
```

---

## Conexion del Boton de Marcha/Parada

```
  Arduino D8 (INPUT_PULLUP) ----+
                                |
                           [PULSADOR]
                                |
                               GND

  Sin presionar: D8 = HIGH (pull-up interno)
  Presionado:    D8 = LOW  (conecta a GND)
  Cada pulsacion alterna MARCHA/PARADA
```

---

## Tabla Resumen de Conexiones

| Arduino Pin | Funcion | Conecta a | Notas |
|:-----------:|:--------|:----------|:------|
| D3 | STEP | A4988 pin STEP | Pulsos de paso |
| D4 | DIR | A4988 pin DIR | Direccion de giro |
| D5 | ENABLE | A4988 pin ENABLE | LOW=activo, HIGH=desactivado |
| D7 | Selector | Switch SPDT | INPUT_PULLUP, GND=CCW |
| D8 | Boton | Pulsador N.A. | INPUT_PULLUP, toggle marcha |
| D9 | MS1 | A4988 pin MS1 | Microstepping bit 1 |
| D10 | MS2 | A4988 pin MS2 | Microstepping bit 2 |
| D11 | MS3 | A4988 pin MS3 | Microstepping bit 3 |
| A0 | Velocidad | Pot 10kOhm (cursor) | 0-1023 -> velocidad |
| 5V | Alimentacion | VDD del A4988, Pot | Logica del driver |
| VIN | Alimentacion | +12V fuente | Alimenta Arduino |
| GND | Tierra comun | Todos los GND | IMPORTANTE: tierra comun |

---

## Ajuste del VREF (Corriente del Motor)

```
  PARA MOTOR KS42STH34-1504A (1.50A Nominal):
  Driver HR4988SQ / A4988 con resistencias Rsense = 0.100 Ohm (R100):
  
  Fórmula: VREF = IMAX x 8 x Rsense = IMAX x 0.8
  
  - Al 100% (1.50A): VREF = 1.20V
  - Al 77% (1.15A - Recomendado para régimen continuo sin sobrecalentar):
    VREF = 0.92V

  PROCEDIMIENTO:
  1. Desconectar motor del driver (fuente apagada)
  2. Energizar lógica y potencia (12V VMOT, 5V VDD, GND común)
  3. Multímetro en escala de 2V DC:
     - Punta negra a GND
     - Punta roja sobre el tornillo metálico del trimmer VR1 del driver
  4. Girar suavemente el potenciómetro hasta leer 0.92V
  5. Desconectar alimentación antes de reconectar el motor
```

---

## Notas de Seguridad

```
  1. NUNCA conectar/desconectar motor con driver energizado
  2. SIEMPRE colocar capacitor 100uF entre VMOT y GND
  3. Ajustar VREF ANTES de conectar motor
  4. Fuente minima: 12V / 2A
  5. Cable motor: AWG 20-22
  6. Fusible recomendado: 2.5A o 3A (slow-blow)
```

---

## Lista de Componentes Eléctricos

| # | Componente | Código (Sawers / Ref.) | Especificación | Cant. |
|:-:|:-----------|:----------------------:|:---------------|:-----:|
| 1 | Arduino Uno R3 SMD (o Nano) | **`00014`** | ATmega328P CH340 USB Tipo B | 1 |
| 2 | Driver Stepper A4988 / HR4988 | **`00154`** | Con disipador de aluminio ($V_{REF} = 0.92\text{V}$) | 1 |
| 3 | Motor Paso a Paso NEMA17 | **`KS42STH34-1504A`** | 1.50A/fase, 4 cables, 34mm | 1 |
| 4 | Switch selector dirección | **`00129`** | Switch 1 tiempo / SPDT / SS12D00G3 | 1 |
| 5 | Potenciómetro rotativo 10kΩ | **`WH148-10K`** (o mod. panel) | Lineal tipo B 10kΩ con perilla | 1 |
| 6 | Pulsador Marcha / Parada | **`PBS-11B`** / Push-button | Normalmente Abierto (momentáneo) | 1 |
| 7 | Fuente de poder DC | Ref. 12V 2A–3A | 12V / 2A mínimo (conector jack 2.1mm) | 1 |
| 8 | Interruptor general 220V | Rocker KCD1 | ON/OFF bipolar o unipolar 250V | 1 |
| 9 | Fusible y portafusible | 2.5A / 250V | Vidrio 5x20mm + portafusible aéreo/chasis | 1 |
| 10 | Capacitor electrolítico | 100µF / 25V–35V | Filtro de picos en bornera VMOT-GND | 1 |
| 11 | Cable para conexiones | Jumpers M-M / M-H | AWG 22 (lógica) y AWG 20 (motor) | 1 lote |
| 12 | Protoboard o Shield de tornillo | Placa borneras / PCB | Para interconexión estable | 1 |
