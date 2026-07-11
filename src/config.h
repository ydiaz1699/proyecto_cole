/**
 * =============================================================================
 * config.h — Constantes del Proyecto (Posicionador Motorizado NEMA17)
 * =============================================================================
 * TODAS las constantes numéricas del proyecto viven aquí.
 * Cada valor tiene:
 *   - Nombre descriptivo (sin magic numbers)
 *   - Comentario con fórmula o justificación
 *   - Unidades explícitas
 *
 * Referencia: .ai/DECISIONS.md para justificaciones detalladas
 * =============================================================================
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// =============================================================================
// VERSION
// =============================================================================
#define FW_VERSION_MAJOR  2
#define FW_VERSION_MINOR  0
#define FW_VERSION_PATCH  0

// =============================================================================
// BUILD FLAGS (cambiar aquí para habilitar/deshabilitar features)
// =============================================================================
#ifndef LCD_ENABLED
#define LCD_ENABLED       0     // 0=sin LCD, 1=con LCD 16x2
#endif

#ifndef DEBUG_SERIAL
#define DEBUG_SERIAL      1     // 0=sin debug, 1=prints por Serial
#endif

// =============================================================================
// PINES — Driver A4988/DRV8825
// =============================================================================
static const uint8_t PIN_STEP   = 3;    // Pulsos de paso (Timer2 capable)
static const uint8_t PIN_DIR    = 4;    // Dirección de giro
static const uint8_t PIN_ENABLE = 5;    // Enable del driver (LOW=activo)
static const uint8_t PIN_MS1    = 9;    // Microstepping bit 0
static const uint8_t PIN_MS2    = 10;   // Microstepping bit 1
static const uint8_t PIN_MS3    = 11;   // Microstepping bit 2

// =============================================================================
// PINES — Controles del Usuario
// =============================================================================
static const uint8_t PIN_POT        = A0;   // Potenciómetro 10kΩ (velocidad)
static const uint8_t PIN_DIR_SWITCH = 7;    // Switch SPDT (dirección CW/CCW)
static const uint8_t PIN_BTN_START  = 8;    // Pulsador marcha/parada (N.A.)
static const uint8_t PIN_LED_STATUS = 13;   // LED built-in (indicador estado)

// =============================================================================
// PINES — LCD (solo si LCD_ENABLED=1)
// =============================================================================
#if LCD_ENABLED
static const uint8_t PIN_LCD_RS = A1;
static const uint8_t PIN_LCD_EN = A2;
static const uint8_t PIN_LCD_D4 = A3;
static const uint8_t PIN_LCD_D5 = A4;
static const uint8_t PIN_LCD_D6 = A5;
static const uint8_t PIN_LCD_D7 = 2;
#endif

// =============================================================================
// MOTOR — Parámetros físicos del NEMA17 JK42HS40-1684
// =============================================================================
static const uint16_t STEPS_PER_REV     = 200;   // 360° / 1.8° = 200 pasos/vuelta
static const uint8_t  MICROSTEP_MODE    = 1;     // 1=full, 2=half, 4=quarter, 8=eighth, 16=sixteenth
static const uint16_t EFFECTIVE_STEPS   = STEPS_PER_REV * MICROSTEP_MODE;  // Pasos efectivos por vuelta

// =============================================================================
// VELOCIDAD — Límites en microsegundos entre flancos STEP
// Fórmula: delay_us = 60,000,000 / (RPM × 2 × EFFECTIVE_STEPS)
//          El ×2 es porque cada paso = 1 flanco HIGH + 1 flanco LOW (toggle)
// =============================================================================

// RPM_MAX = 300 RPM → delay = 60M / (300×2×200) = 500 µs
static const uint16_t STEP_DELAY_MIN_US = 500;

// RPM_MIN = 30 RPM → delay = 60M / (30×2×200) = 5000 µs
static const uint16_t STEP_DELAY_MAX_US = 5000;

// =============================================================================
// ACELERACIÓN — Rampa lineal
// Tiempo total de rampa: ~500ms
// Ticks de rampa: 500ms / ACCEL_INTERVAL_MS = 25 ticks
// Cada tick reduce el delay en: (MAX-MIN) / 25 = (5000-500)/25 = 180 µs
// =============================================================================
static const uint8_t  ACCEL_INTERVAL_MS = 20;    // Intervalo entre ticks de aceleración
static const uint8_t  ACCEL_TICKS_TOTAL = 25;    // Número de ticks para rampa completa
static const uint16_t ACCEL_STEP_US     = (STEP_DELAY_MAX_US - STEP_DELAY_MIN_US) / ACCEL_TICKS_TOTAL;  // = 180 µs por tick

// =============================================================================
// FILTRADO ADC — Promedio móvil
// El ADC del ATmega328P tiene ruido de ±2-4 LSB.
// 8 muestras eliminan el ruido y permiten usar shift (>>3) en vez de división.
// Tiempo de lectura: 8 × ~110µs = ~880µs (< 1ms, despreciable)
// =============================================================================
static const uint8_t  ADC_FILTER_SIZE     = 8;     // Número de muestras (potencia de 2)
static const uint8_t  ADC_FILTER_SHIFT    = 3;     // log2(8) = 3 (para división rápida)
static const uint16_t ADC_READ_INTERVAL_MS = 10;   // Leer ADC cada 10ms (100 Hz)

// =============================================================================
// ZONA MUERTA del Potenciómetro
// El pot puede no llegar exactamente a 0V. Valores <10 (~1%) se consideran "apagado".
// Valores >1013 (~99%) se consideran "máximo" para evitar inestabilidad en el extremo.
// =============================================================================
static const uint16_t POT_DEAD_ZONE_LOW  = 10;    // Por debajo = motor OFF
static const uint16_t POT_DEAD_ZONE_HIGH = 1013;  // Por encima = velocidad máxima

// =============================================================================
// DEBOUNCE — Anti-rebote de botón y selector
// Rebote típico de pulsador: 10-30ms. Usamos 50ms para seguridad.
// =============================================================================
static const uint8_t DEBOUNCE_MS = 50;

// =============================================================================
// CAMBIO DE DIRECCIÓN
// El datasheet del A4988 requiere >200ns entre cambio de DIR y siguiente STEP.
// Usamos 5ms como margen de seguridad + protección mecánica.
// =============================================================================
static const uint8_t DIR_CHANGE_DELAY_MS = 5;

// =============================================================================
// LCD — Rate limiting de actualización
// El ojo humano distingue hasta ~10 Hz. 2 Hz (500ms) es suficiente para RPM.
// Actualizar más rápido consume CPU sin beneficio visual.
// =============================================================================
#if LCD_ENABLED
static const uint16_t LCD_UPDATE_INTERVAL_MS = 500;  // Actualizar LCD cada 500ms
#endif

// =============================================================================
// SERIAL DEBUG — Rate limiting
// =============================================================================
#if DEBUG_SERIAL
static const uint16_t SERIAL_REPORT_INTERVAL_MS = 2000;  // Reportar estado cada 2s
#endif

// =============================================================================
// PROTECCIÓN — Timeouts y límites
// =============================================================================
static const uint16_t MOTOR_TIMEOUT_MS = 5000;   // Si en estado RUN y no genera pasos por 5s → ERROR
static const uint16_t ERROR_BLINK_MS   = 250;    // LED parpadea cada 250ms en estado ERROR

// =============================================================================
// ESTADOS DE LA FSM
// =============================================================================
enum MotorState : uint8_t {
    STATE_IDLE          = 0,    // Motor deshabilitado, esperando comando
    STATE_ACCELERATING  = 1,    // Incrementando velocidad hacia target
    STATE_RUNNING       = 2,    // Velocidad constante, generando pasos
    STATE_DECELERATING  = 3,    // Reduciendo velocidad hacia 0
    STATE_ERROR         = 4     // Fallo detectado, motor deshabilitado
};

// =============================================================================
// DIRECCIÓN
// =============================================================================
enum MotorDirection : uint8_t {
    DIR_CW  = HIGH,   // Horario (clockwise)
    DIR_CCW = LOW     // Anti-horario (counter-clockwise)
};

#endif // CONFIG_H
