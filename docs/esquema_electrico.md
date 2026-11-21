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

## Conexion del Motor NEMA17 (JK42HS40-1684, 4 Hilos)

```
  Cable ROJO  ----> Bobina A+ ----> Pin 1A del driver
  Cable AZUL  ----> Bobina A- ----> Pin 1B del driver
  Cable VERDE ----> Bobina B+ ----> Pin 2A del driver
  Cable NEGRO ----> Bobina B- ----> Pin 2B del driver

  NOTA: Los colores pueden variar segun fabricante.
  Para identificar bobinas: medir continuidad con multimetro.
  Los dos cables con continuidad entre si = UNA bobina.
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
  PARA MOTOR JK42HS40-1684 (1.68A):

  A4988 (Rsense = 0.100 Ohm):
  VREF = 1.68A x 8 x 0.100 = 1.34V

  DRV8825 (Rsense = 0.100 Ohm):
  VREF = 1.68A / 2 = 0.84V

  PROCEDIMIENTO:
  1. Desconectar motor del driver
  2. Energizar driver (12V VMOT, 5V VDD)
  3. Multimetro: punta roja en potenciometro del driver, negra en GND
  4. Girar potenciometro hasta el voltaje calculado
  5. Reconectar motor
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

## Lista de Componentes Electricos

| # | Componente | Especificacion | Cant. |
|:-:|:-----------|:---------------|:-----:|
| 1 | Arduino Nano/Uno | ATmega328P | 1 |
| 2 | Driver A4988 o DRV8825 | Con disipador | 1 |
| 3 | Motor NEMA17 | JK42HS40-1684, 1.68A | 1 |
| 4 | Fuente DC | 12V / 2A (minimo) | 1 |
| 5 | Potenciometro | 10kOhm lineal (tipo B) | 1 |
| 6 | Switch SPDT | Selector de direccion | 1 |
| 7 | Pulsador N.A. | Boton de marcha | 1 |
| 8 | Interruptor | ON/OFF general 220V | 1 |
| 9 | Fusible | 2.5A / 250V + portafusible | 1 |
| 10 | Capacitor | 100uF / 25V electrolitico | 1 |
| 11 | Cable AWG 22 | Senales logicas | 2m |
| 12 | Cable AWG 20 | Motor y potencia | 1m |
