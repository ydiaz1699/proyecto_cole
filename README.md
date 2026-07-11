# POSICIONADOR MOTORIZADO CON MOTOR PASO A PASO NEMA17

> Proyecto de Grado - Colegio Nacional San Luis, Seccion Industrial
> Mecanica Industrial - Tarija, Bolivia

## Descripcion

Posicionador motorizado de escala para uso en taller de soldadura. Utiliza un motor paso a paso NEMA17 con driver A4988/DRV8825 controlado por Arduino para girar un plato de acero con velocidad regulable y sentido de giro reversible.

**Aplicaciones:**
- Soldadura de piezas circulares, tubos y bridas
- Inspeccion visual de 360 grados de componentes mecanicos
- Pintura uniforme de piezas rotativas
- Posicionamiento angular controlado

## Estructura del Proyecto

```
proyecto_cole/
|-- codigo_arduino/
|   |-- posicionador_nema17.ino        # Codigo principal (basico)
|   |-- posicionador_nema17_lcd.ino    # Version con pantalla LCD 16x2
|-- esquemas/
|   |-- esquema_electrico.md           # Diagramas de conexion completos
|-- calculos/
|   |-- calculos_tecnicos.md           # Calculos de torque, RPM, transmision
|-- documentacion/                     # Perfil de proyecto, planos, fotos
|-- README.md                          # Este archivo
```

## Especificaciones Tecnicas

| Parametro | Valor |
|:----------|:------|
| Motor | NEMA17 JK42HS40-1684 |
| Torque | 3.6 kg*cm |
| Corriente | 1.68 A/fase |
| Pasos/vuelta | 200 (1.8 grados/paso) |
| Driver | A4988 o DRV8825 |
| Alimentacion | 12V DC / 2A |
| Controlador | Arduino Nano o Uno |
| Plato | Acero 5mm, diametro 25-30 cm |
| Capacidad | 5-15 kg (con reduccion) |



## Conexiones Rapidas (Arduino -> A4988)

| Arduino | A4988 | Funcion |
|:-------:|:-----:|:--------|
| D3 | STEP | Pulsos de paso |
| D4 | DIR | Direccion de giro |
| D5 | ENABLE | Habilitar motor |
| D9 | MS1 | Microstepping |
| D10 | MS2 | Microstepping |
| D11 | MS3 | Microstepping |
| A0 | - | Potenciometro velocidad |
| D7 | - | Selector de direccion |
| D8 | - | Boton marcha/parada |

## Como Usar

### Requisitos de Software
- Arduino IDE 1.8+ o 2.0+
- No requiere librerias externas (version basica)
- Libreria `LiquidCrystal` (solo version LCD, incluida en Arduino IDE)

### Cargar el Codigo
1. Abrir Arduino IDE
2. Abrir archivo `codigo_arduino/posicionador_nema17.ino`
3. Seleccionar placa: **Arduino Nano** (o Uno)
4. Seleccionar puerto COM correcto
5. Hacer clic en **Subir**

### Uso del Posicionador
1. **Encender** con el interruptor general ON/OFF
2. **Ajustar velocidad** girando el potenciometro (izq=lento, der=rapido)
3. **Seleccionar direccion** con el switch CW/CCW
4. **Marcha/Parada** con el boton pulsador

## Materiales Principales

| Material | Cantidad | Costo (Bs) |
|:---------|:--------:|:-----------:|
| Perfil cuadrado 40x40mm (6m) | 2 barras | 170 |
| Plancha acero 5mm (30x30cm) | 1 | 60 |
| Barra maciza acero diam 20mm | 1 (50cm) | 45 |
| Chumaceras UCP204 | 2 | 70 |
| Motor NEMA17 (disponible) | 1 | -- |
| Driver A4988/DRV8825 | 1 | 25 |
| Arduino Nano | 1 | 35 |
| Fuente DC 12V/2A | 1 | 80 |
| Componentes electricos | varios | 71 |
| Pintura anticorrosiva | 1 kit | 40 |
| **TOTAL ESTIMADO** | | **~596 Bs** |

## Notas Importantes

- NUNCA conectar/desconectar el motor con el driver energizado
- Colocar capacitor 100uF entre VMOT y GND del driver
- Ajustar VREF del driver ANTES de conectar el motor
- Para cargas >5 kg usar reduccion mecanica (poleas GT2 3:1)
- Ver `calculos/calculos_tecnicos.md` para detalles de torque

## Versiones del Codigo

| Version | Archivo | Caracteristicas |
|:--------|:--------|:----------------|
| Basica | `posicionador_nema17.ino` | Potenciometro + selector + boton |
| LCD | `posicionador_nema17_lcd.ino` | + pantalla 16x2 con RPM y estado |

## Autores

- **Jhonny Elias Diaz Moyata** - Estudiante 6to Secundaria
- **Rodrigo Angel Nava Flores** - Estudiante 6to Secundaria
- **Profesor:** Marco Antonio Vargas
- **Institucion:** Colegio Nacional San Luis, Seccion Industrial

## Licencia

Proyecto educativo - Uso libre para fines academicos y de aprendizaje.

---
*Gestion 2025 - Tarija, Bolivia*
