/**
 * =============================================================================
 * motor_controller.h — FSM del Motor Paso a Paso
 * =============================================================================
 * Responsabilidades:
 *   - Máquina de estados: IDLE → ACCEL → RUN → DECEL → IDLE
 *   - Generación de pulsos STEP non-blocking (micros)
 *   - Rampa de aceleración/deceleración lineal
 *   - Control de dirección (solo cambia en IDLE)
 *   - Estado ERROR con recuperación
 *
 * Principio: El motor NUNCA se controla directamente desde main.
 *            Todo pasa por la FSM.
 * =============================================================================
 */

#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include "config.h"
#include "input_manager.h"

// === Estructura de estado del motor (para display/debug) ===
struct MotorStatus {
    MotorState    state;             // Estado actual de la FSM
    MotorDirection direction;        // Dirección actual
    uint16_t      currentDelay;      // Delay actual entre pasos (µs)
    uint16_t      targetDelay;       // Delay objetivo (del pot)
    float         currentRPM;        // RPM calculadas actuales
    bool          enabled;           // TRUE si driver está habilitado
};

class MotorController {
public:
    void init();
    void update(const InputState& inputs);   // Llamar en cada loop()
    const MotorStatus& getStatus() const;    // Para display/debug
    void emergencyStop();                    // Parada inmediata (sin decel)

private:
    MotorStatus _status;

    // --- Generación de pasos ---
    unsigned long _lastStepTime;        // Último flanco STEP (micros)
    bool          _stepPinState;        // Estado actual del pin STEP

    void _generateStep();

    // --- Aceleración ---
    unsigned long _lastAccelTick;       // Último tick de rampa (millis)

    void _accelerate();
    void _decelerate();

    // --- FSM ---
    void _handleIdle(const InputState& inputs);
    void _handleAccelerating(const InputState& inputs);
    void _handleRunning(const InputState& inputs);
    void _handleDecelerating(const InputState& inputs);
    void _handleError(const InputState& inputs);

    // --- Utilidades ---
    void _enableMotor();
    void _disableMotor();
    void _setDirection(MotorDirection dir);
    void _updateRPM();
    void _setMicrostepping(uint8_t mode);
};

#endif // MOTOR_CONTROLLER_H
