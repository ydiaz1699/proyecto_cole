# Guía de Prueba Rápida y Calibración — Motor NEMA17 + Driver HR4988

Esta guía documenta el banco de pruebas inicial para verificar el funcionamiento del motor paso a paso y el driver antes de montar todo el cableado del posicionador de soldadura.

---

## 1. Hardware Utilizado en la Prueba

* **Motor Paso a Paso:** KS MOTOR `KS42STH34-1504A` (NEMA 17, longitud 34mm, 1.50A/fase, 200 pasos/vuelta, $1.8^\circ$).
* **Driver:** `HR4988SQ` en módulo `HW-134` (reemplazo 100% compatible del Allegro A4988, resistencias de sensado $R_s = 0.100\,\Omega$ / `R100`).
* **Microcontrolador:** Arduino Nano (ATmega328P).
* **Alimentación:** Fuente 12V DC (con capacitor electrolítico de $100\,\mu\text{F} \ge 25\text{V}$ en paralelo a la entrada del driver).

---

## 2. Calibración Crítica del Driver ($V_{REF}$)

Para evitar sobrecalentamiento del driver HR4988 y operar el motor al ~77% de su corriente nominal en régimen continuo:

$$I_{MAX} = \frac{V_{REF}}{8 \times R_s} = \frac{V_{REF}}{0.8} \implies V_{REF} = I_{MAX} \times 0.8$$

* **Ajuste fijado:** $\mathbf{V_{REF} = 0.92\text{ V}}$
* **Corriente resultante:** $I = 1.15\text{ A}$ (óptima para el motor de 1.5A sin riesgo de corte térmico).
* **Medición:** Punta roja del multímetro en el tornillo metálico del trimmer `VR1`, punta negra en `GND`.

---

## 3. Asignación de Cables del Motor (Solución a "Solo suena / vibra pero no gira")

El cable estándar de 4 a 6 pines suele venir cruzado de fábrica (`Rojo - Azul - Verde - Negro`), mezclando las fases. 

### Correspondencia correcta de Bobinas:
* **Bobina A (Fase 1):** Cables **ROJO** y **VERDE** (Pines 1 y 4 del conector blanco del motor) $\rightarrow$ Conectar a **`1B` y `1A`** del driver.
* **Bobina B (Fase 2):** Cables **AZUL** y **NEGRO** (Pines 3 y 6 del conector blanco del motor) $\rightarrow$ Conectar a **`2A` y `2B`** del driver.

---

## 4. Esquema de Conexión del Banco de Prueba (Mínimo)

Este esquema permite probar el motor **sin necesidad de potenciómetro ni botones físicos**, controlándolo íntegramente desde la PC por Monitor Serial:

```
        ARDUINO NANO                              DRIVER HR4988 / A4988
       ┌────────────┐                            ┌───────────────────────────┐
       │         D3 ├───────────────────────────►│ STEP                      │
       │         D4 ├───────────────────────────►│ DIR                       │
       │         D5 ├───────────────────────────►│ ENABLE                    │
       │         5V ├───────────────────────────►│ VDD                       │
       │        GND ├───────────────────────────►│ GND (Lógica)              │
       └────────────┘                            │                           │
                                                 │ RESET ──┐ (Puente directo │
                                                 │ SLEEP ──┘  con jumper) ⚠️ │
                                                 │                           │
                                                 │ MS1, MS2, MS3 (Ver nota)  │
                                                 │                           │
        FUENTE 12V DC                            │                           │
       ┌────────────┐                            │                           │
       │        +12V├───────────────────────────►│ VMOT  [+]                 │
       │            │                            │       [Capacitor 100µF]   │
       │        GND ├───────────────────────────►│ GND   [-] (Potencia)      │
       └────────────┘                            │                           │
             │                                   │ SALIDAS AL MOTOR:         │
             └─────────── (Unir a GND común) ───►│ 1B ──► CABLE ROJO         │
                                                 │ 1A ──► CABLE VERDE        │
                                                 │ 2A ──► CABLE AZUL         │
                                                 │ 2B ──► CABLE NEGRO        │
                                                 └───────────────────────────┘
```

> [!WARNING]
> * **RESET y SLEEP deben estar puenteados entre sí:** Sin este puente, el driver entra en modo de bajo consumo y el motor no responde.
> * **GND Común:** El terminal GND del Arduino y el polo negativo (GND) de la fuente de 12V deben conectarse entre sí.
> * **Capacitor 100µF:** Colocarlo lo más cerca posible de los pines `VMOT` y `GND` respetando la polaridad.

---

## 5. Suavidad a Baja Velocidad: Configuración de Micropasos

Si a baja velocidad el giro se percibe a "saltitos" o con vibración, es debido a que en **Paso Completo** el motor salta $1.8^\circ$ por paso:

| Modo | Conexión de Pines MS | Pasos por Vuelta | Comportamiento |
| :--- | :---: | :---: | :--- |
| **Paso Completo (1/1)** | MS1, MS2, MS3 desconectados | 200 | Áspero a baja velocidad |
| **1/4 de Paso** | **MS2 conectado a 5V** (MS1 y MS3 libres) | 800 | Giro suave con buen torque |
| **1/16 de Paso** | **MS1, MS2 y MS3 los tres a 5V** | 3.200 | Ultra suave, silencioso |

---

## 6. Carga y Uso del Firmware de Prueba

### Compilación y Subida desde PlatformIO:
```bash
# Subir el entorno de prueba serial
pio run -e prueba -t upload
```

### Comandos del Monitor Serial (115200 Baudios):
Abrir el monitor serie en PlatformIO o Arduino IDE configurado a **115200 baudios**:

| Comando | Acción |
| :---: | :--- |
| **`1`** | Gira **1 vuelta completa** (200 pasos) en sentido Horario (CW) y frena. |
| **`2`** | Gira **1 vuelta completa** en sentido Antihorario (CCW) y frena. |
| **`m`** | **Marcha continua**: Inicia o detiene el giro constante. |
| **`p`** | Parada inmediata de emergencia. |
| **`d`** | Invierte el sentido de giro (Horario $\leftrightarrow$ Antihorario). |
| **`+`** | Aumenta la velocidad (reduce tiempo entre pasos). |
| **`-`** | Reduce la velocidad (ideal para arranques suaves). |
| **`e`** | Habilita / Deshabilita el driver (afloja el eje para rotarlo a mano y enfriar). |
| **`h`** | Muestra el menú de ayuda en pantalla. |
