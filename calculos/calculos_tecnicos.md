# CALCULOS TECNICOS - POSICIONADOR MOTORIZADO NEMA17

## 1. Datos del Motor Seleccionado

```
  MOTOR: JK42HS40-1684 (NEMA17)
  Par de sujecion (holding torque): 3.6 kg*cm
  Corriente nominal: 1.68 A/fase
  Angulo de paso: 1.8 grados
  Pasos por revolucion: 200
  Voltaje recomendado: 12-24V DC
```

---

## 2. Analisis de Torque Requerido

### 2.1 Datos del diseno

```
  Diametro del plato: 30 cm (radio = 0.15 m)
  Peso del plato (acero 5mm, diam 30cm): ~2.8 kg
  Carga maxima sobre el plato: 5-15 kg
  Masa total a girar: 2.8 + 15 = 17.8 kg (caso maximo)
  Eje horizontal con rodamientos (chumaceras UCP204)
```

### 2.2 Caso A: Plato horizontal (pieza apoyada encima)

Solo hay que vencer la friccion de los rodamientos.

```
  T_friccion = mu x m x g x r_eje
  T_friccion = 0.003 x 17.8 x 9.81 x 0.01
  T_friccion = 0.00524 N*m = 0.053 kg*cm

  RESULTADO: 3.6 kg*cm >> 0.053 kg*cm --> SUFICIENTE sin reduccion
```

### 2.3 Caso B: Plato vertical (posicion de soldadura)

El torque depende del desbalance de la pieza.

```
  T_desbalance = m x g x d
  (d = distancia del centro de masa al eje)

  Con 10 kg descentrada 5 cm:
  T = 10 x 9.81 x 0.05 = 4.9 N*m = 50 kg*cm

  RESULTADO: 3.6 kg*cm << 50 kg*cm --> NECESITA REDUCCION
```

### 2.4 Caso C: Inercia para arranque

```
  I_plato = (1/2) x m x r^2 = (1/2) x 2.8 x 0.15^2 = 0.0315 kg*m^2
  I_carga = m x r^2 = 10 x 0.10^2 = 0.10 kg*m^2
  I_total = 0.1315 kg*m^2

  Para acelerar de 0 a 5 RPM en 2 seg:
  alpha = 0.524/2 = 0.262 rad/s^2
  T_aceleracion = 0.1315 x 0.262 = 0.035 N*m = 0.35 kg*cm

  RESULTADO: 3.6 kg*cm > 0.35 kg*cm --> SUFICIENTE
```



### 2.5 Resumen de torques

| Escenario | Torque necesario | Motor (3.6 kg*cm) | Suficiente? |
|:----------|:----------------:|:------------------:|:-----------:|
| Plato horizontal (friccion) | 0.05 kg*cm | 3.6 kg*cm | SI |
| Aceleracion de arranque | 0.35 kg*cm | 3.6 kg*cm | SI |
| Plato vertical, carga centrada | ~5 kg*cm | 3.6 kg*cm | NO |
| Plato vertical, carga descentrada | ~50 kg*cm | 3.6 kg*cm | NO |

---

## 3. Relacion de Transmision Necesaria

### 3.1 Calculo

```
  Relacion (i) = T_requerido / T_motor x Factor_seguridad

  Para 5 kg centrada (plato vertical):
  i = (5 / 3.6) x 1.5 = 2.08 --> Usar 3:1

  Para 10 kg con desbalance ligero:
  i = (15 / 3.6) x 1.5 = 6.25 --> Usar 8:1 o 10:1

  Para uso pesado (15 kg descentrado):
  i = (50 / 3.6) x 1.5 = 20.8 --> Usar 20:1
```

### 3.2 Opciones de reduccion

| Opcion | Relacion | Torque resultado | Vel. max plato |
|:-------|:--------:|:----------------:|:--------------:|
| Acoplamiento directo | 1:1 | 3.6 kg*cm | 150 RPM |
| Poleas GT2 | 3:1 | 10.8 kg*cm | 50 RPM |
| Engranajes rectos | 5:1 | 18 kg*cm | 30 RPM |
| Correa + engranaje | 10:1 | 36 kg*cm | 15 RPM |
| Tornillo sin fin | 20:1 | 72 kg*cm | 7.5 RPM |

### 3.3 Recomendacion

```
  OPCION RECOMENDADA: Poleas GT2, relacion 3:1
  - Polea motor: 20 dientes (bore 5mm)
  - Polea plato: 60 dientes (bore 20mm)
  - Correa GT2 cerrada, 6mm ancho, ~280mm
  - Torque en plato: ~10.8 kg*cm
  - Velocidad maxima en plato: ~50 RPM
  - Costo: ~60-100 Bs total
```

---

## 4. Calculos de Velocidad (RPM)

### 4.1 RPM segun el codigo Arduino

```
  Formula: RPM = 60,000,000 / (delay_us x 2 x pasos_por_vuelta)

  Con paso completo (200 pasos/vuelta):
  RPM_min = 60,000,000 / (5000 x 2 x 200) = 30 RPM
  RPM_max = 60,000,000 / (500 x 2 x 200) = 300 RPM

  Con reduccion 3:1 (poleas GT2):
  RPM_min en plato = 30 / 3 = 10 RPM
  RPM_max en plato = 300 / 3 = 100 RPM

  Con reduccion 10:1:
  RPM_min en plato = 3 RPM (ideal para soldadura)
  RPM_max en plato = 30 RPM
```

### 4.2 Velocidad para posicionamiento manual

```
  Para GIRAR la pieza y luego soldar en posicion fija:
  Velocidades de 5-30 RPM son adecuadas.
  Este es el CASO PRINCIPAL de este proyecto.
```

---

## 5. Calculos Electricos

### 5.1 Consumo de corriente

```
  Motor NEMA17:     1.68 A  (12V)
  Arduino Nano:     0.05 A  (12V via VIN)
  Driver A4988:     0.01 A  (5V logica)
  Potenciometro:    0.0005 A
  -----------------------------------------
  TOTAL:            ~1.76 A a 12V

  Fuente recomendada: 12V / 2A (minimo)
  Fuente ideal: 12V / 3A (con margen)
```

### 5.2 Potencia total

```
  P_total = 12V x 1.76A = 21.1 W
  Fusible recomendado: 2.5A o 3A (slow-blow)
  Cable motor: AWG 20-22 (soporta hasta 3-5A)
```

---

## 6. Calculos Mecanicos

### 6.1 Peso del plato

```
  Material: Acero (densidad = 7850 kg/m^3)
  Diametro: 30 cm, Espesor: 5 mm

  Volumen = pi x 0.15^2 x 0.005 = 0.000353 m^3
  Masa = 7850 x 0.000353 = 2.77 kg (aprox 2.8 kg)
```

### 6.2 Esfuerzo en el eje

```
  Eje: Barra maciza acero diam 20mm
  Carga: 17.8 kg = 174.6 N (entre 2 chumaceras a 30cm)

  Momento flector: M = (F x L)/4 = (174.6 x 0.30)/4 = 13.1 N*m
  Modulo seccion: W = (pi x d^3)/32 = 7.85 x 10^-7 m^3
  Esfuerzo: sigma = M/W = 16.7 MPa

  Esfuerzo admisible acero SAE 1020: ~140 MPa
  Factor de seguridad: 140/16.7 = 8.4

  CONCLUSION: Eje de diam 20mm es MAS QUE SUFICIENTE.
```

### 6.3 Rodamientos UCP204

```
  Carga por rodamiento: 87.3 N
  Capacidad estatica UCP204: ~6500 N
  Factor de seguridad: 6500/87.3 = 74.5

  CONCLUSION: Rodamientos sobredimensionados, perfectamente adecuados.
```

---

## 7. Conclusiones

```
  1. Motor NEMA17 (3.6 kg*cm) es ADECUADO con reduccion mecanica.
  2. Con poleas GT2 3:1 se obtiene ~10 kg*cm (cargas hasta 5 kg horizontal).
  3. Eje diam 20mm: factor de seguridad 8.4 (muy seguro).
  4. Fuente 12V/2A es suficiente para todo el sistema.
  5. Para cargas >10 kg en vertical, usar reduccion 10:1 o mayor.
  6. Recomendacion: empezar con acoplamiento directo para pruebas,
     luego agregar poleas GT2 3:1 para uso con carga.
```
