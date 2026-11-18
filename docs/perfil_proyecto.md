# POSICIONADOR MOTORIZADO DE ESCALA PARA USO EN TALLER DE SOLDADURA

## COLEGIO NACIONAL SAN LUIS — SECCIÓN INDUSTRIAL

### TÉCNICA TECNOLÓGICA ESPECIALIZADA: MECÁNICA INDUSTRIAL

---

**PERFIL DE PROYECTO DE GRADO**

| | |
|:--|:--|
| **Título** | Posicionador Motorizado de Escala para Uso en Taller de Soldadura |
| **Estudiante 1** | Jhonny Elías Díaz Moyata |
| **Estudiante 2** | Rodrigo Ángel Nava Flores |
| **Curso** | 6to de Secundaria |
| **Profesor** | Marco Antonio Vargas |
| **Gestión** | 2025 |
| **Ciudad** | Tarija, Bolivia |

---

## ÍNDICE

1. [Introducción](#1-introducción)
2. [Planteamiento del Proyecto](#2-planteamiento-del-proyecto)
3. [Justificación y Validez](#3-justificación-y-validez)
4. [Objetivos](#4-objetivos)
5. [Marco Teórico](#5-marco-teórico)
6. [Diseño del Proyecto](#6-diseño-del-proyecto)
7. [Sistema de Control (Software)](#7-sistema-de-control-software)
8. [Cotización de Materiales](#8-cotización-de-materiales)
9. [Cronograma de Trabajo](#9-cronograma-de-trabajo)
10. [Pruebas de Funcionamiento](#10-pruebas-de-funcionamiento)
11. [Resultados Esperados](#11-resultados-esperados)
12. [Conclusiones](#12-conclusiones)
13. [Bibliografía y Referencias](#13-bibliografía-y-referencias)

---


## 1. INTRODUCCIÓN

La automatización de procesos en la industria metalmecánica ha transformado significativamente la manera en que se realizan trabajos de soldadura, corte, pintura e inspección de piezas. Dentro de este contexto, el **posicionador motorizado** emerge como una herramienta fundamental que permite mejorar la ergonomía del operario, la calidad del producto terminado y la eficiencia del proceso productivo.

Un posicionador motorizado es una máquina industrial que utiliza un motor eléctrico para girar, inclinar o sostener una pieza en la posición de trabajo más adecuada. Su principio de funcionamiento se basa en la transmisión de movimiento desde un motor hacia un plato o sistema de rodillos que sostienen y desplazan la pieza de manera controlada.

El presente proyecto consiste en el **diseño, fabricación y programación** de un posicionador motorizado con plato giratorio a escala, utilizando como unidad motriz un motor paso a paso NEMA17 controlado por un microcontrolador Arduino Nano. Esta elección tecnológica permite un control preciso de la velocidad y el ángulo de giro mediante una **máquina de estados finitos (FSM)** programada en firmware, superando ampliamente las limitaciones de los sistemas de control analógicos convencionales.

El motor paso a paso divide cada revolución en 200 pasos discretos de 1.8°, ofreciendo repetibilidad y control digital sin necesidad de sensores de retroalimentación (encoders). El sistema incorpora:

- **Aceleración gradual** para evitar pérdida de pasos al arrancar con carga.
- **Filtrado digital** del potenciómetro para eliminación de ruido eléctrico.
- **Protecciones de software** contra cambios bruscos de dirección y sobrecarga.
- **Interfaz de usuario** intuitiva con potenciómetro, selector y pulsador.

El proyecto integra **cuatro áreas** del conocimiento técnico:

1. **Soldadura y fabricación metálica** — Estructura soporte del posicionador.
2. **Electricidad industrial** — Sistema de alimentación y protección.
3. **Mecánica de transmisión** — Acoplamiento motor-plato con reducción.
4. **Programación de microcontroladores** — Control digital con Arduino (FSM).

Esta convergencia de saberes lo convierte en un proyecto técnico integral que demuestra la aplicación práctica de las competencias adquiridas durante la formación técnica.

### Aplicaciones principales

| Aplicación | Descripción |
|:-----------|:------------|
| Soldadura de piezas circulares | Tubos, bridas, cilindros — cordón uniforme a velocidad constante |
| Inspección visual 360° | Revisión completa sin manipular la pieza manualmente |
| Pintura uniforme | Giro constante para acabado parejo en piezas rotativas |
| Posicionamiento angular | Fijar el plato en un ángulo específico para soldar en posición plana |
| Fabricación de estructuras | Portones, rejas, marcos — girar la pieza para acceder a todas las caras |

---


## 2. PLANTEAMIENTO DEL PROYECTO

### 2.1 ¿Qué se va a construir?

Un posicionador motorizado de escala para uso en taller de soldadura. El dispositivo cuenta con:

- **Plato giratorio** de acero (Ø25–30 cm, espesor 5 mm) capaz de sostener piezas de 5–15 kg.
- **Motor paso a paso NEMA17** (modelo JK42HS40-1684, 1.68A, 3.6 kg·cm) como unidad motriz.
- **Sistema de reducción** por poleas GT2 (relación 3:1) para multiplicar el torque.
- **Arduino Nano** como controlador digital con firmware modular (FSM de 5 estados).
- **Driver A4988** para amplificación de corriente y control de microstepping.
- **Interfaz de usuario**: potenciómetro (velocidad), switch (dirección), pulsador (marcha/parada).

La velocidad de giro es regulable de 10 a 100 RPM en el plato (30–300 RPM en el motor), el sentido es reversible, y el arranque/parada es gradual gracias a una rampa de aceleración de 500 ms programada en el firmware.

### 2.2 ¿Por qué se va a hacer?

En los talleres de soldadura, mantener una pieza en la posición correcta durante el proceso es uno de los mayores desafíos, especialmente cuando se trabaja solo. Los posicionadores comerciales tienen las siguientes limitaciones:

| Problema | Impacto |
|:---------|:--------|
| Costo elevado (300–800 USD) | Inaccesible para talleres pequeños e instituciones educativas |
| Control analógico básico | Sin aceleración gradual, pierde pasos con carga |
| Sin protecciones de software | Cambio de dirección brusco daña mecanismo |
| Importación obligatoria | No disponible localmente en Tarija |

Este proyecto propone una versión **económica (~85 USD), portátil, con control digital avanzado** que integra conocimientos de mecánica, soldadura, electricidad y programación.

### 2.3 Beneficios concretos

1. **Ergonomía**: Elimina el esfuerzo manual de girar piezas pesadas.
2. **Calidad**: Mejora el cordón de soldadura al mantener posición plana constante.
3. **Seguridad**: Reduce riesgo de accidentes por manipulación manual de piezas calientes.
4. **Productividad**: Reduce tiempo de trabajo en un 30–40% estimado.
5. **Integración técnica**: Demuestra dominio de 4 áreas técnicas simultáneamente.
6. **Control digital**: Arranque suave, velocidad precisa, protecciones automáticas.

### 2.4 ¿Dónde se va a hacer?

El proyecto se desarrollará en las instalaciones del taller de soldadura y electricidad de la institución educativa. Se utilizarán:

- Soldadora eléctrica o MIG (Metal Inert Gas)
- Esmeril angular y de banco
- Taladro de banco
- Mesa de trabajo y herramientas manuales
- Computadora con Arduino IDE (para programación)
- Multímetro digital (para ajuste de VREF del driver)

Las pruebas de funcionamiento se realizarán en el mismo espacio bajo supervisión del docente.

---


## 3. JUSTIFICACIÓN Y VALIDEZ

### 3.1 Justificación técnica

Los posicionadores motorizados representan tecnología ampliamente utilizada en la industria, pero escasamente accesible para talleres medianos y pequeños. Este proyecto demuestra que es posible fabricar una versión funcional con control digital avanzado utilizando:

- Un motor paso a paso NEMA17 con driver A4988 (control de 200 pasos/vuelta sin encoder).
- Un microcontrolador Arduino Nano (programación de FSM con aceleración).
- Reducción mecánica por poleas GT2 (multiplicación de torque 3:1).

El **diferencial tecnológico** respecto a proyectos similares es el firmware de Nivel 2 que incluye:
- Máquina de estados finitos (FSM) con 5 estados y transiciones definidas.
- Filtrado digital del ADC (promedio móvil de 8 muestras).
- Rampa de aceleración lineal (evita pérdida de pasos al arrancar).
- Protección contra cambio de dirección en movimiento.

### 3.2 Justificación económica

| Concepto | Posicionador comercial | Este proyecto | Ahorro |
|:---------|:----------------------:|:-------------:|:------:|
| Costo | 300–800 USD | ~85 USD (596 Bs) | **70–89%** |
| Disponibilidad | Importación (2–4 semanas) | Fabricación local (5 semanas) | Inmediata |
| Reparabilidad | Requiere repuestos importados | Componentes locales | **100% local** |
| Control | Analógico básico | Digital con FSM | **Superior** |

### 3.3 Justificación educativa

El proyecto permite aplicar de forma integrada los contenidos de:

- **Soldadura**: Fabricación de la estructura metálica (perfil 40×40 mm).
- **Electricidad**: Sistema de alimentación, protección (fusible) y cableado.
- **Mecánica**: Transmisión de movimiento (poleas, rodamientos, eje).
- **Programación**: Control digital con Arduino (nuevo aporte al perfil técnico).

### 3.4 Validez del proyecto

| Criterio | Sustento |
|:---------|:---------|
| Factibilidad técnica | Todos los componentes disponibles localmente o ya en posesión |
| Factibilidad económica | Presupuesto de 596 Bs viable para proyecto escolar |
| Pertinencia curricular | Integra competencias de soldadura, electricidad y mecánica del plan de estudios |
| Impacto práctico | El producto puede usarse inmediatamente en el taller de la institución |
| Innovación | Control digital con FSM + aceleración (no presente en proyectos similares del nivel) |
| Reproducibilidad | Código fuente y documentación disponibles en GitHub para replicar |

### 3.5 Comparación técnica: Motor DC convencional vs. NEMA17

| Característica | Motor DC convencional | NEMA17 (este proyecto) |
|:---------------|:---------------------:|:----------------------:|
| Control de velocidad | Impreciso (PWM analógico) | 200 pasos/vuelta, digital |
| Posicionamiento angular | Requiere encoder ($$$) | Intrínseco (1.8°/paso) |
| Repetibilidad | Baja (depende de carga) | Alta (pasos discretos) |
| Control de dirección | Simple (inversión polaridad) | Digital (pin DIR) |
| Aceleración programable | No (arranque brusco) | Sí (rampa por firmware) |
| Costo del sistema completo | Similar | Similar (~25 Bs el driver) |
| Complejidad de control | Baja | Media (requiere Arduino) |

---


## 4. OBJETIVOS

### 4.1 Objetivo General

Diseñar, fabricar, programar y probar un posicionador motorizado a escala con plato giratorio, accionado por un motor paso a paso NEMA17 controlado mediante firmware con máquina de estados finitos (FSM) en Arduino Nano, que permita posicionar piezas metálicas con velocidad regulable (10–100 RPM), sentido de giro reversible y arranque/parada gradual.

### 4.2 Objetivos Específicos

1. **Estructura mecánica**: Diseñar y fabricar la estructura metálica soporte con perfil cuadrado 40×40 mm según las necesidades del proceso (base 50×40×30 cm).

2. **Plato giratorio**: Fabricar el plato en plancha de acero de 5 mm (Ø25–30 cm) y acoplarlo al eje de transmisión mediante chumaceras UCP204.

3. **Transmisión mecánica**: Implementar sistema de reducción por poleas GT2 (20T/60T, relación 3:1) para multiplicar el torque del motor de 3.6 a ~10.8 kg·cm.

4. **Sistema eléctrico**: Montar el sistema de alimentación (fuente 12V/2A), protección (fusible 2.5A) y driver A4988 con ajuste preciso de VREF a 1.34V.

5. **Firmware de control**: Programar el microcontrolador Arduino Nano con:
   - Máquina de estados (IDLE → ACCEL → RUN → DECEL → ERROR)
   - Filtrado ADC del potenciómetro (promedio de 8 muestras)
   - Rampa de aceleración lineal (500 ms de 0 a velocidad objetivo)
   - Protección contra cambio de dirección en movimiento

6. **Pruebas y validación**: Realizar pruebas de carga (en vacío y con 2–5 kg) para validar el diseño, medir RPM reales y verificar ausencia de pérdida de pasos.

---


## 5. MARCO TEÓRICO

### 5.1 El posicionador motorizado

Un posicionador motorizado es un dispositivo electromecánico cuyo principio se basa en la transmisión de par (torque) desde un motor eléctrico hacia un plato o sistema de rodillos que sostiene la pieza de trabajo. Permite al operario acceder a todas las caras de una pieza sin esfuerzo manual, manteniendo velocidad constante y posición controlada.

**Tipos de posicionadores según su configuración:**

| Tipo | Descripción | Aplicación |
|:-----|:------------|:-----------|
| Horizontal con plato | Giro en plano horizontal, pieza apoyada | Piezas planas, bridas, tapas |
| Inclinable | Permite bascular el plato 0°–90° además de girar | Tubos, cilindros, piezas 3D |
| Rotador (roller) | Usa rodillos para girar tubos y cilindros grandes | Tanques, ductos, tubería gruesa |
| Automático sincronizado | Conectado a equipo MIG/TIG para soldadura continua | Producción en serie |

Para este proyecto se adopta el tipo **horizontal con capacidad de inclinación manual**, accionado por motor paso a paso NEMA17.

### 5.2 Motor paso a paso NEMA17

#### ¿Qué es?

Un motor paso a paso es un motor eléctrico que divide una revolución completa en un número fijo de pasos iguales. A diferencia de un motor DC convencional que gira continuamente al aplicar voltaje, el motor paso a paso avanza un ángulo preciso por cada pulso eléctrico que recibe.

**NEMA 17** es una clasificación de tamaño estandarizada por la *National Electrical Manufacturers Association* (NEMA). El número "17" indica que la cara frontal del motor mide 1.7 × 1.7 pulgadas (42 × 42 mm).

#### Motor seleccionado: JK42HS40-1684

| Parámetro | Valor | Significado práctico |
|:----------|:------|:---------------------|
| Modelo | JK42HS40-1684 | Serie JK, 42mm, 40mm alto, 1684=1.68A |
| Fases | 2 | Dos bobinas independientes (A y B) |
| Hilos | 4 | Bipolar (2 hilos por bobina) |
| Ángulo de paso | 1.8° | 360°/1.8° = 200 pasos por vuelta |
| Par de sujeción | 3.6 kg·cm (0.35 N·m) | Fuerza para mantener posición con motor energizado |
| Corriente nominal | 1.68 A por fase | Corriente máxima en cada bobina |
| Resistencia por fase | ~1.65 Ω (estimada) | Determina voltaje mínimo (V=I×R) |
| Altura | 42 mm | Cuerpo del motor |

#### ¿Por qué este motor y no un motor DC?

```
Motor DC:     Voltaje → Velocidad (imprecisa, depende de carga)
Motor NEMA17: 1 pulso = 1 paso = 1.8° (siempre, independiente de carga*)

* Siempre que no se exceda el torque disponible a esa velocidad.
```

#### Motor alternativo disponible: JK42HS34-0956

| Parámetro | JK42HS40-1684 (principal) | JK42HS34-0956 (alternativo) |
|:----------|:------------------------:|:---------------------------:|
| Par de sujeción | **3.6 kg·cm** | 1.6 kg·cm |
| Corriente | 1.68 A | 1.0 A |
| Hilos | 4 (bipolar) | 6 (unipolar/bipolar) |
| Altura | 42 mm | 34 mm |
| Uso recomendado | **Cargas hasta 5 kg con reducción** | Cargas livianas (<2 kg) |

**Selección**: Se usa el JK42HS40-1684 como motor principal por su mayor torque (2.25× superior).

### 5.3 Driver A4988

#### ¿Qué es y por qué se necesita?

El Arduino Nano no puede alimentar directamente un motor NEMA17 porque:
- El motor necesita 1.68A; el Arduino solo entrega 40mA por pin.
- El motor funciona a 12V; el Arduino trabaja a 5V en sus pines digitales.

El **A4988** (fabricado por Allegro MicroSystems) es un circuito integrado que actúa como "amplificador de potencia controlado". Recibe señales digitales pequeñas del Arduino (5V, <1mA) y las convierte en corrientes de hasta 2A a 12–35V para mover el motor.

#### Funcionamiento simplificado

```
Arduino                    A4988                      Motor NEMA17
────────                   ─────                      ────────────
Pin D3 → [STEP] ──────► Genera secuencia    ──────► Bobinas A y B
Pin D4 → [DIR]  ──────► de corriente        ──────► se energizan
Pin D5 → [ENABLE]─────► en las 2 fases      ──────► Motor gira 1.8°
```

Cada **pulso** en el pin STEP hace que el motor avance exactamente **un paso** (1.8°).
El pin **DIR** determina si gira en sentido horario o antihorario.
El pin **ENABLE** permite apagar el motor para ahorrar energía cuando no se usa.

#### Microstepping

El A4988 puede subdividir cada paso en fracciones más pequeñas:

| Configuración (MS1/MS2/MS3) | Resolución | Pasos/vuelta | Suavidad |
|:----------------------------:|:-----------|:------------:|:--------:|
| L / L / L | Paso completo | 200 | Baja (vibra más) |
| H / L / L | Medio paso | 400 | Media |
| L / H / L | 1/4 de paso | 800 | Buena |
| H / H / L | 1/8 de paso | 1600 | Muy buena |
| H / H / H | 1/16 de paso | 3200 | Excelente (más lento) |

Para este proyecto se usa **paso completo** (200 pasos/vuelta) porque:
- Ofrece el máximo torque disponible por paso.
- La velocidad requerida (10–100 RPM en plato) no necesita mayor resolución.
- Simplifica el cálculo de RPM y posición angular.

#### Ajuste de corriente (VREF)

El A4988 tiene un potenciómetro pequeño que limita la corriente al motor. Se ajusta midiendo el voltaje en ese punto:

```
Fórmula:  VREF = I_motor × 8 × R_sense

Para nuestro motor (1.68A) con Rsense = 0.100Ω:
VREF = 1.68 × 8 × 0.100 = 1.34V

Procedimiento:
1. Desconectar motor del driver
2. Energizar driver (12V en VMOT, 5V en VDD)
3. Medir voltaje en el potenciómetro del driver con multímetro
4. Girar hasta leer 1.34V
5. Reconectar motor
```

### 5.4 Arduino Nano (microcontrolador)

#### ¿Qué es?

El Arduino Nano es una placa electrónica basada en el microcontrolador ATmega328P. Es esencialmente una computadora miniatura que ejecuta un programa (firmware) almacenado en su memoria. En este proyecto, el Arduino es el "cerebro" que decide cuándo y cómo hacer girar el motor según las entradas del usuario.

| Parámetro | Valor | Relevancia para este proyecto |
|:----------|:------|:------------------------------|
| Procesador | ATmega328P, 16 MHz | Suficiente para generar pulsos a 300 RPM |
| RAM | 2 KB | Limitada — requiere código eficiente |
| Flash | 32 KB | Suficiente para el firmware (~8 KB usado) |
| Pines digitales | 14 (D0–D13) | 6 usados para driver + 2 para controles |
| Pines analógicos | 8 (A0–A7) | 1 usado para potenciómetro |
| Voltaje lógico | 5V | Compatible con A4988 |
| Alimentación | 7–12V por VIN | Se alimenta de la misma fuente del motor |
| Dimensiones | 18 × 45 mm | Cabe en cualquier espacio del tablero |
| Costo | ~35 Bs (~5 USD) | Muy económico |

#### ¿Por qué Arduino y no un circuito con temporizador 555?

| Aspecto | Circuito 555 | Arduino Nano |
|:--------|:-------------|:-------------|
| Control de velocidad | Sí (potenciómetro → frecuencia) | Sí (potenciómetro → firmware) |
| Aceleración gradual | No (arranque brusco) | **Sí (rampa programable)** |
| Cambio de dirección seguro | No (manual con switch) | **Sí (solo en estado IDLE)** |
| Display de RPM | No | **Sí (Serial o LCD)** |
| Protección contra errores | No | **Sí (estado ERROR en FSM)** |
| Modos de operación | 1 (giro continuo) | **Expandible (angular, velocidad, automático)** |
| Costo adicional | ~5 Bs (555 + componentes) | ~35 Bs (Arduino Nano) |

La diferencia de 30 Bs justifica ampliamente las capacidades adicionales que aporta el Arduino.

### 5.5 Máquina de Estados Finitos (FSM)

#### ¿Qué es una FSM?

Una máquina de estados finitos es un modelo de programación donde el sistema solo puede estar en **uno de varios estados predefinidos** en cualquier momento. Las **transiciones** entre estados ocurren cuando se cumplen condiciones específicas (eventos).

Es como un semáforo: solo puede estar en ROJO, AMARILLO o VERDE. Nunca en dos colores a la vez. Y cambia de uno a otro siguiendo reglas fijas.

#### FSM del posicionador (5 estados)

```
                         [Botón MARCHA + Pot>0]
                    ┌────────────────────────────────┐
                    │                                │
                    ▼                                │
             ┌──────────┐                           │
             │   IDLE   │ ← Motor apagado,          │
             │(REPOSO)  │   esperando comando        │
             └─────┬────┘                           │
                   │                                │
                   │ Botón presionado               │
                   │ + pot fuera de zona muerta     │
                   ▼                                │
             ┌──────────────┐                       │
             │ ACCELERATING │ ← Incrementa          │
             │ (ACELERANDO) │   velocidad cada 20ms │
             └─────┬────────┘                       │
                   │                                │
                   │ Velocidad alcanzó objetivo     │
                   ▼                                │
             ┌──────────┐                           │
             │ RUNNING  │ ← Gira a velocidad       │
             │(OPERANDO)│   constante               │
             └─────┬────┘                           │
                   │                                │
                   │ Botón STOP o pot a cero        │
                   ▼                                │
             ┌──────────────┐                       │
             │ DECELERATING │ ← Reduce velocidad    │
             │(DESACELERANDO)│  gradualmente         │
             └─────┬────────┘                       │
                   │                                │
                   │ Velocidad llegó a cero         │
                   └────────────────────────────────┘

             ┌──────────┐
             │  ERROR   │ ← Fallo detectado
             │          │   (LED parpadea)
             └──────────┘   Botón STOP → vuelve a IDLE
```

#### ¿Por qué usar FSM en vez de simple ON/OFF?

| Situación | Sin FSM (ON/OFF) | Con FSM |
|:----------|:-----------------|:--------|
| Arrancar motor con carga | Arranca de golpe → pierde pasos | Acelera gradualmente → funciona |
| Parar motor girando rápido | Para de golpe → inercia mueve pieza | Desacelera → parada controlada |
| Cambiar dirección girando | Se permite → daña mecanismo | Solo en IDLE → protege poleas/correa |
| Error eléctrico | Motor sigue girando | Motor se apaga, LED avisa, requiere reset |

---


## 6. DISEÑO DEL PROYECTO

### 6.1 Diseño mecánico — Estructura

#### Dimensiones generales

| Parámetro | Medida | Material |
|:----------|:-------|:---------|
| Base (largo × ancho × alto) | 50 × 40 × 30 cm | Perfil cuadrado 40×40 mm |
| Plato giratorio | Ø 25–30 cm, espesor 5 mm | Plancha de acero al carbono |
| Eje central | Ø 20 mm, largo 50 cm | Barra maciza de acero SAE 1020 |
| Soportes laterales | Altura 35–40 cm | Perfil cuadrado 40×40 mm |
| Rodamientos | UCP204 (bore 20 mm) | 2 unidades, tipo chumacera |

#### Cálculos mecánicos fundamentales

**Peso del plato:**
```
Material: Acero (densidad = 7850 kg/m³)
Volumen = π × 0.15² × 0.005 = 0.000353 m³
Masa = 7850 × 0.000353 = 2.77 kg ≈ 2.8 kg
```

**Esfuerzo en el eje (peor caso: 17.8 kg centrado):**
```
Momento flector: M = (174.6 N × 0.30 m) / 4 = 13.1 N·m
Módulo de sección: W = (π × 0.020³) / 32 = 7.85 × 10⁻⁷ m³
Esfuerzo: σ = 13.1 / 7.85×10⁻⁷ = 16.7 MPa

Esfuerzo admisible acero SAE 1020: 140 MPa
Factor de seguridad: 140 / 16.7 = 8.4 → MUY SEGURO ✓
```

**Verificación de rodamientos UCP204:**
```
Carga por rodamiento: 174.6 N / 2 = 87.3 N
Capacidad estática UCP204: ~6500 N
Factor de seguridad: 6500 / 87.3 = 74.5 → SOBREDIMENSIONADO ✓
```

### 6.2 Diseño mecánico — Transmisión

#### ¿Por qué se necesita reducción?

El motor NEMA17 tiene un torque de 3.6 kg·cm. Para mover cargas de 5+ kg en el plato (especialmente con desbalance), se necesita multiplicar este torque mediante una reducción mecánica.

**Análisis de torque por escenario:**

| Escenario | Torque necesario | Motor directo (3.6 kg·cm) | ¿Suficiente? |
|:----------|:----------------:|:-------------------------:|:------------:|
| Plato horizontal, solo fricción | 0.05 kg·cm | 3.6 kg·cm | ✅ Sí |
| Aceleración de arranque | 0.35 kg·cm | 3.6 kg·cm | ✅ Sí |
| Plato vertical, 5 kg centrada | ~5 kg·cm | 3.6 kg·cm | ❌ No |
| Plato vertical, 10 kg descentrada | ~50 kg·cm | 3.6 kg·cm | ❌ No |

#### Solución: Poleas GT2 con relación 3:1

| Componente | Especificación | Función |
|:-----------|:---------------|:--------|
| Polea motriz (motor) | GT2, 20 dientes, bore 5 mm | Engrana con la correa |
| Polea conducida (plato) | GT2, 60 dientes, bore 20 mm | Gira el eje del plato |
| Correa dentada | GT2, 6 mm ancho, ~280 mm cerrada | Transmite movimiento |

**Resultado con reducción 3:1:**
- Torque en plato: 3.6 × 3 = **10.8 kg·cm** (suficiente para 5 kg horizontal)
- Velocidad en plato: RPM_motor / 3 = 10–100 RPM (ideal para posicionamiento)
- Costo de poleas + correa: ~60–100 Bs

### 6.3 Diseño eléctrico

#### Esquema general del sistema

```
220V AC ──► [INTERRUPTOR ON/OFF] ──► [FUSIBLE 2.5A] ──► [FUENTE 12V/2A DC]
                                                              │      │
                                                            +12V    GND
                                                              │      │
     ┌────────────────────────────────────────────────────────┼──────┼────┐
     │                     ARDUINO NANO                       │      │    │
     │                                                        │      │    │
     │  VIN ◄─────────────────────────────────────────────────┘      │    │
     │  GND ◄────────────────────────────────────────────────────────┘    │
     │                                                                    │
     │  D3  (STEP)    ─────────────────────────────────────► A4988       │
     │  D4  (DIR)     ─────────────────────────────────────► A4988       │
     │  D5  (ENABLE)  ─────────────────────────────────────► A4988       │
     │  D9  (MS1)     ─────────────────────────────────────► A4988       │
     │  D10 (MS2)     ─────────────────────────────────────► A4988       │
     │  D11 (MS3)     ─────────────────────────────────────► A4988       │
     │                                                                    │
     │  A0  ◄───── Potenciómetro 10kΩ (velocidad)                        │
     │  D7  ◄───── Switch SPDT (dirección CW/CCW)                        │
     │  D8  ◄───── Pulsador N.A. (marcha/parada)                         │
     │  D13 ────► LED indicador de estado                                 │
     │                                                                    │
     └────────────────────────────────────────────────────────────────────┘
                              │
                     ┌────────┴────────┐
                     │     A4988       │──────► MOTOR NEMA17
                     │ + Cap 100µF     │        (4 hilos)
                     └─────────────────┘
```

#### Consumo eléctrico total

| Componente | Corriente | Voltaje |
|:-----------|:---------:|:-------:|
| Motor NEMA17 (operando) | 1.68 A | 12V |
| Arduino Nano | 0.05 A | 12V (VIN) |
| Driver A4988 (lógica) | 0.01 A | 5V |
| Potenciómetro + switches | <0.001 A | 5V |
| **TOTAL** | **~1.76 A** | **12V** |

**Potencia total:** 12V × 1.76A = **21.1 W**
**Fuente recomendada:** 12V / 2A (mínimo), ideal 12V / 3A
**Fusible:** 2.5A slow-blow (protección contra cortocircuito)

---


## 7. SISTEMA DE CONTROL (SOFTWARE)

### 7.1 Arquitectura del firmware

El código del posicionador está organizado en **4 módulos independientes**, cada uno con una responsabilidad específica:

```
main.cpp ─── Punto de entrada
    │
    ├── InputManager      → Lee y filtra las entradas del usuario
    │   ├── Potenciómetro (filtro promedio móvil, 8 muestras)
    │   ├── Botón marcha/parada (debounce 50ms)
    │   └── Selector de dirección (detección de cambio)
    │
    ├── MotorController   → Controla el motor con FSM de 5 estados
    │   ├── Generación de pulsos STEP (non-blocking con micros())
    │   ├── Rampa de aceleración/deceleración (20ms por tick)
    │   ├── Protección de dirección (solo en IDLE)
    │   └── Estado ERROR con recuperación
    │
    └── DisplayManager    → Muestra información al usuario
        ├── Monitor Serial (debug, cada 2 segundos)
        └── LCD 16×2 (opcional, cada 500ms)
```

### 7.2 Constantes críticas del sistema

Cada constante numérica del código tiene una fórmula que la justifica:

| Constante | Valor | Fórmula | Efecto si se aumenta | Efecto si se reduce |
|:----------|:------|:--------|:---------------------|:--------------------|
| Delay mínimo (vel. máx.) | 500 µs | 60M/(300RPM×2×200) | Motor más lento | Puede perder pasos |
| Delay máximo (vel. mín.) | 5000 µs | 60M/(30RPM×2×200) | Arranque más lento | Vel. mínima más alta |
| Intervalo aceleración | 20 ms | 500ms/25 ticks | Rampa más larga | Rampa más brusca |
| Filtro ADC | 8 muestras | Potencia de 2 (shift >>3) | Más filtrado, lento | Más ruido, rápido |
| Debounce | 50 ms | > rebote típico 10–30ms | Botón responde lento | Falsos positivos |
| Zona muerta pot | 10/1023 | ~1% del rango | Difícil arrancar | Arranca con toque leve |

### 7.3 Ciclo de ejecución (loop)

El programa principal ejecuta este ciclo continuamente (~10,000 veces por segundo):

```
┌─ loop() ──────────────────────────────────────────────┐
│                                                        │
│  1. LEER entradas (filtrado + debounce)     ~110 µs   │
│  2. ACTUALIZAR FSM del motor               ~20 µs    │
│  3. GENERAR paso si es momento             ~6 µs     │
│  4. ACTUALIZAR display (si toca)           ~0-1300 µs │
│                                                        │
│  Tiempo total por ciclo: ~140 µs (sin LCD)            │
│  Frecuencia: ~7000 Hz (7000 ciclos/segundo)           │
│                                                        │
└────────────────────────────────────────────────────────┘
```

**Dato clave:** A la velocidad máxima del motor (300 RPM), un paso ocurre cada 500 µs. Como el loop tarda solo 140 µs, siempre hay tiempo de sobra para generar el siguiente paso sin retrasarse.

### 7.4 Código fuente

El código fuente completo está disponible en el repositorio del proyecto:

- **Repositorio:** `github.com/ydiaz1699/proyecto_cole`
- **Carpeta:** `src/` (versión Nivel 2 modular)
- **Carpeta legacy:** `codigo_arduino/` (versión Nivel 1 monolítica, para referencia)

Los archivos principales son:

| Archivo | Líneas | Responsabilidad |
|:--------|:------:|:----------------|
| `config.h` | ~130 | Todas las constantes con fórmulas |
| `input_manager.h/.cpp` | ~120 | Filtrado ADC + debounce + selector |
| `motor_controller.h/.cpp` | ~250 | FSM completa con aceleración |
| `display_manager.h/.cpp` | ~140 | LCD + Serial con rate limiting |
| `main.cpp` | ~40 | Entry point (setup + loop limpio) |

---


## 8. COTIZACIÓN DE MATERIALES

| # | Material / Componente | Cantidad | P. Unit. (Bs) | Total (Bs) |
|:-:|:----------------------|:--------:|:-------------:|:----------:|
| 1 | Perfil cuadrado 40×40 mm (barra 6 m) | 2 | 85.00 | 170.00 |
| 2 | Plancha de acero 5 mm (30×30 cm) | 1 | 60.00 | 60.00 |
| 3 | Barra maciza de acero Ø20 mm (50 cm) | 1 | 45.00 | 45.00 |
| 4 | Chumaceras UCP204 (rodamientos) | 2 | 35.00 | 70.00 |
| 5 | Motor NEMA17 JK42HS40-1684 (disponible) | 1 | — | 0.00 |
| 6 | Driver A4988 con disipador | 1 | 25.00 | 25.00 |
| 7 | Arduino Nano (ATmega328P, CH340) | 1 | 35.00 | 35.00 |
| 8 | Fuente DC 12V / 2A (adaptador) | 1 | 80.00 | 80.00 |
| 9 | Interruptor ON/OFF 220V | 1 | 8.00 | 8.00 |
| 10 | Potenciómetro 10kΩ lineal (tipo B) | 1 | 5.00 | 5.00 |
| 11 | Switch SPDT (selector dirección) | 1 | 10.00 | 10.00 |
| 12 | Pulsador normalmente abierto | 1 | 5.00 | 5.00 |
| 13 | Fusible 2.5A + portafusible | 1 | 6.00 | 6.00 |
| 14 | Capacitor 100µF/25V electrolítico | 1 | 3.00 | 3.00 |
| 15 | Cable eléctrico AWG 20-22 (metros) | 3 | 4.00 | 12.00 |
| 16 | Tornillos, tuercas, arandelas (varios) | lote | 20.00 | 20.00 |
| 17 | Electrodos E6013 Ø2.5mm | 1 kg | 15.00 | 15.00 |
| 18 | Pintura anticorrosiva + thinner | 1 kit | 40.00 | 40.00 |
| 19 | Placa perforada + borneras (tablero) | 1 | 15.00 | 15.00 |
| | | | **TOTAL** | **624.00** |

**Costo total estimado: 624 Bs ≈ 90 USD**

> **Nota:** El motor NEMA17 no tiene costo adicional (aportado por el estudiante). Los precios son estimativos para Tarija, Bolivia y pueden variar según proveedor.

### Fuentes de adquisición

| Componente | Proveedor sugerido |
|:-----------|:-------------------|
| Perfiles, planchas, barra acero | Ferreterías industriales / distribuidores de acero |
| Motor NEMA17, driver A4988, Arduino | Ya disponibles (aportados por estudiante) |
| Fuente DC, componentes eléctricos | Tiendas de electrónica / distribuidoras eléctricas |
| Rodamientos UCP204 | Distribuidores de rodamientos / ferreterías industriales |
| Pintura anticorrosiva | Ferreterías generales |
| Poleas GT2 y correa (si se agregan) | Tiendas de electrónica / compra online (~60–100 Bs adicional) |

---

## 9. CRONOGRAMA DE TRABAJO

| Semana | Etapa | Actividades | Entregable |
|:------:|:------|:------------|:-----------|
| **1** | Planificación y diseño | Definir medidas, elaborar planos, seleccionar motor, cotizar y adquirir materiales | Plano + lista de materiales |
| **2** | Fabricación estructura | Corte de perfiles 40×40mm, soldadura de base y soportes, mecanizado de eje | Estructura soldada |
| **3** | Montaje mecánico | Instalar chumaceras, acoplar plato al eje, fijar motor, instalar poleas GT2 | Sistema mecánico armado |
| **4** | Sistema eléctrico + software | Montar tablero (driver, Arduino, fuente), cablear, cargar firmware, ajustar VREF | Sistema eléctrico funcionando |
| **5** | Pruebas y acabado | Prueba en vacío, prueba con carga, ajustes de parámetros, pintura, presentación | Proyecto terminado |

### Diagrama de Gantt simplificado

```
Semana:     1         2         3         4         5
          ┌─────┐
Diseño    │█████│
          └─────┘
                  ┌─────┐
Estructura│       │█████│
                  └─────┘
                          ┌─────┐
Mecánica  │               │█████│
                          └─────┘
                                  ┌─────┐
Eléctrico │                       │█████│
+ Software                        └─────┘
                                          ┌─────┐
Pruebas   │                               │█████│
                                          └─────┘
```

---


## 10. PRUEBAS DE FUNCIONAMIENTO

### 10.1 Protocolo de pruebas incrementales

Las pruebas se realizarán de forma **incremental** (una variable a la vez), siguiendo la metodología de análisis profundo de 3 capas:

| # | Test | Qué se verifica | Criterio de éxito |
|:-:|:-----|:----------------|:------------------|
| 1 | Solo driver + motor (sin Arduino) | Alimentación y VREF correctos | Motor sostiene posición al energizar |
| 2 | Arduino + pulso fijo 2ms | Comunicación Arduino→A4988→Motor | Motor gira a 150 RPM constante |
| 3 | Agregar potenciómetro | ADC + mapeo de velocidad | Velocidad varía con el potenciómetro |
| 4 | Agregar filtro ADC (8 muestras) | Estabilidad de velocidad | Sin oscilaciones visibles |
| 5 | Agregar FSM completa | Aceleración/deceleración | Arranque suave, parada suave |
| 6 | Agregar botón + selector | Control completo | Todas las funciones operativas |
| 7 | Agregar carga (2 kg, luego 5 kg) | Capacidad mecánica | Sin pérdida de pasos |

### 10.2 Prueba en vacío (sin carga)

**Procedimiento:**
1. Energizar el sistema (12V, verificar LED de estado).
2. Verificar en Monitor Serial: `[FSM] Estado = IDLE`.
3. Girar potenciómetro lentamente → motor debe arrancar suavemente.
4. Verificar RPM en Serial vs. cronómetro manual (contar vueltas en 30 seg).
5. Cambiar dirección (switch) — solo debe funcionar cuando motor está parado.
6. Presionar botón STOP → motor debe desacelerar gradualmente.

**Criterios de aceptación:**
- ✓ El plato gira en ambas direcciones sin interrupciones.
- ✓ La velocidad responde al potenciómetro (30–300 RPM motor / 10–100 RPM plato).
- ✓ El arranque es gradual (no brusco).
- ✓ La parada es gradual (no brusca).
- ✓ El cambio de dirección solo funciona en IDLE.
- ✓ El Monitor Serial muestra estados correctos de la FSM.

### 10.3 Prueba con carga

**Procedimiento:**
1. Colocar pieza de 2 kg centrada en el plato.
2. Repetir todas las pruebas de vacío.
3. Incrementar a 5 kg.
4. Verificar que no hay pérdida de pasos (marcar posición inicial, girar 10 vueltas, verificar que vuelve al mismo punto).
5. Registrar: RPM reales, ruido, vibración, temperatura del motor (después de 5 min).

**Criterios de aceptación:**
- ✓ No se producen pérdidas de pasos bajo carga nominal (5 kg).
- ✓ El motor no supera 60°C de temperatura de carcasa.
- ✓ El sistema eléctrico no presenta sobrecalentamiento.
- ✓ La estructura no presenta deflexión ni vibración excesiva.
- ✓ La correa GT2 no salta dientes (si se usa reducción).

### 10.4 Mediciones a registrar

| Parámetro | Instrumento | Valor esperado |
|:----------|:------------|:---------------|
| VREF del driver | Multímetro DC | 1.34V ±0.05V |
| RPM en vacío (pot a 50%) | Cronómetro + cálculo | ~65 RPM motor / ~22 RPM plato |
| RPM con 5 kg (pot a 50%) | Cronómetro + cálculo | ~65 RPM (igual, sin pérdida) |
| Temperatura motor (5 min) | Termómetro / mano | <60°C |
| Voltaje en VMOT bajo carga | Multímetro DC | >11.5V (no caer debajo) |
| Corriente total del sistema | Amperímetro | ~1.2–1.7A |

---

## 11. RESULTADOS ESPERADOS

Al completar el proyecto se espera obtener:

1. **Un posicionador funcional** capaz de girar piezas de hasta 5 kg (10 kg con reducción superior) de forma controlada, con velocidad regulable y dirección reversible.

2. **Un sistema de control digital** que demuestre las ventajas de la programación de microcontroladores aplicada a la mecánica industrial (FSM, filtrado, protecciones).

3. **Documentación técnica completa** que permita replicar el proyecto en otras instituciones educativas o talleres pequeños.

4. **Un ahorro del 70–89%** respecto a posicionadores comerciales equivalentes.

5. **Una herramienta de uso inmediato** para el taller de la institución educativa.

### Mejoras opcionales implementables

| Mejora | Complejidad | Beneficio | Estado |
|:-------|:-----------:|:----------|:------:|
| Pedal de control de pie | Baja | Libera manos del soldador | Planificado v3.0 |
| Pantalla LCD (RPM + estado) | Baja | Feedback visual directo | Código listo, hardware opcional |
| Escala angular en el plato (0°–360°) | Baja | Referencia visual para posicionamiento | Mecánico |
| Base con ruedas | Baja | Portabilidad | Mecánico |
| Protector anti-salpicadura del motor | Baja | Protege el motor de la soldadura | Mecánico |
| Alimentación por batería 12V | Baja | Uso portátil en campo | Eléctrico |
| Modo posicionamiento angular | Media | Control por ángulo en vez de velocidad | Software (futuro) |

---

## 12. CONCLUSIONES

1. **Es viable fabricar** un posicionador motorizado funcional con un presupuesto de ~624 Bs (~90 USD), demostrando que la tecnología industrial es accesible para talleres pequeños e instituciones educativas.

2. **El motor NEMA17** (3.6 kg·cm) combinado con reducción GT2 3:1 proporciona torque suficiente (~10.8 kg·cm) para cargas de hasta 5 kg en posición horizontal, cubriendo las necesidades de un taller educativo.

3. **El control digital con Arduino** aporta un diferencial significativo respecto a soluciones con motor DC: aceleración programable, protecciones automáticas, y expandibilidad futura sin cambiar hardware.

4. **La máquina de estados finitos (FSM)** garantiza un comportamiento predecible y seguro del sistema, evitando los problemas de cambios de dirección bruscos y arranques que causan pérdida de pasos.

5. **El proyecto integra exitosamente** cuatro áreas técnicas (soldadura, electricidad, mecánica y programación), constituyendo un ejemplo tangible de los aprendizajes adquiridos durante la formación técnica.

6. **La documentación y el código fuente** están disponibles públicamente en GitHub, permitiendo que otros estudiantes y talleres repliquen o mejoren el diseño.

---

## 13. BIBLIOGRAFÍA Y REFERENCIAS

### Referencias técnicas

1. Allegro MicroSystems. *A4988 DMOS Microstepping Driver with Translator and Overcurrent Protection — Datasheet*. Disponible en: https://www.allegromicro.com/en/products/motor-drivers/brush-dc-motor-drivers/a4988

2. Microchip Technology. *ATmega328P — 8-bit AVR Microcontroller Datasheet*. Disponible en: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf

3. Arduino. *Arduino Nano Documentation*. Disponible en: https://docs.arduino.cc/hardware/nano

4. Pololu. *A4988 Stepper Motor Driver Carrier — Wiring and Configuration Guide*. Disponible en: https://www.pololu.com/product/1182

### Repositorios del proyecto

5. Díaz Y. *proyecto_cole — Posicionador Motorizado NEMA17*. GitHub. Disponible en: https://github.com/ydiaz1699/proyecto_cole

6. Díaz Y. *analisis_de_codigo — Skill de Análisis Profundo y Guía de Crecimiento*. GitHub. Disponible en: https://github.com/ydiaz1699/analisis_de_codigo

7. Díaz Y. *vsCode-AI — Framework de Context Engineering para Firmware Embebido*. GitHub. Disponible en: https://github.com/ydiaz1699/vsCode-AI

### Referencias metodológicas

8. Díaz Y. *Guía: Crecimiento de Código — De Prototipo a Producción*. En: analisis_de_codigo/guia-crecimiento-de-codigo.md. Patrón de 3 niveles aplicado al firmware del posicionador.

9. Díaz Y. *Skill: Análisis Profundo y Exhaustivo de Código*. En: analisis_de_codigo/skill-analisis-profundo.md. Metodología de 3 capas aplicada al análisis de timing del sistema.

### Normativa

10. NEMA (National Electrical Manufacturers Association). *NEMA ICS 16 — Motion/Position Control Motors, Controls and Feedback Devices*. Estándar de dimensiones para motores paso a paso.

---

*Documento generado: Julio 2025*
*Versión: 2.0 (reestructurado con datos técnicos reales del análisis profundo)*
*Repositorio: github.com/ydiaz1699/proyecto_cole*
