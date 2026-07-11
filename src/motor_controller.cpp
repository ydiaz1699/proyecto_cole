/**
 * =============================================================================
 * motor_controller.cpp — Implementación de la FSM del Motor
 * =============================================================================
 */

#include "motor_controller.h"

void MotorController::init() {
    // Configurar pines de salida
    pinMode(PIN_STEP, OUTPUT);
    pinMode(PIN_DIR, OUTPUT);
    pinMode(PIN_ENABLE, OUTPUT);
    pinMode(PIN_MS1, OUTPUT);
    pinMode(PIN_MS2, OUTPUT);
    pinMode(PIN_MS3, OUTPUT);
    pinMode(PIN_LED_STATUS, OUTPUT);

    // Estado inicial: motor deshabilitado
    _disableMotor();
    digitalWrite(PIN_DIR, DIR_CW);
    digitalWrite(PIN_STEP, LOW);
    digitalWrite(PIN_LED_STATUS, LOW);

    // Configurar microstepping
    _setMicrostepping(MICROSTEP_MODE);

    // Inicializar variables internas
    _lastStepTime = 0;
    _stepPinState = false;
    _lastAccelTick = 0;

    // Inicializar estado público
    _status.state = STATE_IDLE;
    _status.direction = DIR_CW;
    _status.currentDelay = STEP_DELAY_MAX_US;
    _status.targetDelay = STEP_DELAY_MAX_US;
    _status.currentRPM = 0.0;
    _status.enabled = false;
}

void MotorController::update(const InputState& inputs) {
    // Actualizar el target delay desde las entradas
    _status.targetDelay = inputs.targetDelay;

    // Ejecutar el handler del estado actual
    switch (_status.state) {
        case STATE_IDLE:
            _handleIdle(inputs);
            break;
        case STATE_ACCELERATING:
            _handleAccelerating(inputs);
            break;
        case STATE_RUNNING:
            _handleRunning(inputs);
            break;
        case STATE_DECELERATING:
            _handleDecelerating(inputs);
            break;
        case STATE_ERROR:
            _handleError(inputs);
            break;
    }

    // Generar pasos si el motor está activo (ACCEL, RUN, DECEL)
    if (_status.state == STATE_ACCELERATING ||
        _status.state == STATE_RUNNING ||
        _status.state == STATE_DECELERATING) {
        _generateStep();
    }

    // Actualizar RPM para display
    _updateRPM();
}

const MotorStatus& MotorController::getStatus() const {
    return _status;
}

void MotorController::emergencyStop() {
    _disableMotor();
    _status.state = STATE_IDLE;
    _status.currentDelay = STEP_DELAY_MAX_US;
    _status.currentRPM = 0.0;
}

// =============================================================================
// FSM — Estado IDLE
// Motor deshabilitado. Espera botón START con pot fuera de zona muerta.
// Permite cambio de dirección.
// =============================================================================
void MotorController::_handleIdle(const InputState& inputs) {
    // LED apagado en IDLE
    digitalWrite(PIN_LED_STATUS, LOW);

    // Permitir cambio de dirección solo en IDLE
    if (inputs.dirChanged) {
        MotorDirection newDir = inputs.dirSwitchState ? DIR_CW : DIR_CCW;
        _setDirection(newDir);
    }

    // Transición: IDLE → ACCELERATING
    // Condición: botón presionado Y potenciómetro fuera de zona muerta
    if (inputs.btnStartPressed && !inputs.potInDeadZone) {
        _enableMotor();
        _status.currentDelay = STEP_DELAY_MAX_US;  // Arrancar desde velocidad mínima
        _lastAccelTick = millis();
        _status.state = STATE_ACCELERATING;

        #if DEBUG_SERIAL
        Serial.println(F("[FSM] IDLE -> ACCELERATING"));
        #endif
    }
}

// =============================================================================
// FSM — Estado ACCELERATING
// Incrementa velocidad (reduce delay) cada ACCEL_INTERVAL_MS hasta alcanzar target.
// =============================================================================
void MotorController::_handleAccelerating(const InputState& inputs) {
    // LED encendido fijo durante aceleración
    digitalWrite(PIN_LED_STATUS, HIGH);

    // Parada por botón o zona muerta → decelerar
    if (inputs.btnStartPressed || inputs.potInDeadZone) {
        _status.state = STATE_DECELERATING;
        _lastAccelTick = millis();
        #if DEBUG_SERIAL
        Serial.println(F("[FSM] ACCELERATING -> DECELERATING"));
        #endif
        return;
    }

    // Ejecutar rampa de aceleración
    _accelerate();

    // Transición: alcanzó velocidad target → RUNNING
    if (_status.currentDelay <= _status.targetDelay) {
        _status.currentDelay = _status.targetDelay;
        _status.state = STATE_RUNNING;
        #if DEBUG_SERIAL
        Serial.println(F("[FSM] ACCELERATING -> RUNNING"));
        #endif
    }
}

// =============================================================================
// FSM — Estado RUNNING
// Genera pasos a velocidad constante. Ajusta si el pot cambia.
// =============================================================================
void MotorController::_handleRunning(const InputState& inputs) {
    // LED encendido fijo durante operación
    digitalWrite(PIN_LED_STATUS, HIGH);

    // Parada por botón → decelerar
    if (inputs.btnStartPressed) {
        _status.state = STATE_DECELERATING;
        _lastAccelTick = millis();
        #if DEBUG_SERIAL
        Serial.println(F("[FSM] RUNNING -> DECELERATING (boton)"));
        #endif
        return;
    }

    // Zona muerta → decelerar
    if (inputs.potInDeadZone) {
        _status.state = STATE_DECELERATING;
        _lastAccelTick = millis();
        #if DEBUG_SERIAL
        Serial.println(F("[FSM] RUNNING -> DECELERATING (pot=0)"));
        #endif
        return;
    }

    // Ajustar velocidad suavemente si el pot cambió
    // Si target es más rápido (delay menor): acelerar gradualmente
    // Si target es más lento (delay mayor): decelerar gradualmente
    if (_status.targetDelay < _status.currentDelay) {
        // Necesita acelerar → ir a ACCELERATING
        _status.state = STATE_ACCELERATING;
        _lastAccelTick = millis();
    } else if (_status.targetDelay > _status.currentDelay + ACCEL_STEP_US) {
        // Necesita decelerar significativamente → ajustar gradualmente aquí
        // (no ir a DECEL completo, solo reducir un poco)
        unsigned long now = millis();
        if (now - _lastAccelTick >= ACCEL_INTERVAL_MS) {
            _lastAccelTick = now;
            _status.currentDelay += ACCEL_STEP_US;
            if (_status.currentDelay > STEP_DELAY_MAX_US) {
                _status.currentDelay = STEP_DELAY_MAX_US;
            }
        }
    } else {
        // Dentro de rango → seguir a velocidad actual
        _status.currentDelay = _status.targetDelay;
    }
}

// =============================================================================
// FSM — Estado DECELERATING
// Reduce velocidad (incrementa delay) hasta detenerse, luego → IDLE.
// =============================================================================
void MotorController::_handleDecelerating(const InputState& inputs) {
    // LED parpadea durante deceleración
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 200) {
        lastBlink = millis();
        digitalWrite(PIN_LED_STATUS, !digitalRead(PIN_LED_STATUS));
    }

    // Ejecutar rampa de deceleración
    _decelerate();

    // Transición: velocidad = 0 → IDLE
    if (_status.currentDelay >= STEP_DELAY_MAX_US) {
        _disableMotor();
        _status.currentDelay = STEP_DELAY_MAX_US;
        _status.state = STATE_IDLE;
        #if DEBUG_SERIAL
        Serial.println(F("[FSM] DECELERATING -> IDLE"));
        #endif
    }
}

// =============================================================================
// FSM — Estado ERROR
// Motor deshabilitado. LED parpadea rápido. Solo sale con botón STOP.
// =============================================================================
void MotorController::_handleError(const InputState& inputs) {
    // LED parpadea rápido (250ms)
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > ERROR_BLINK_MS) {
        lastBlink = millis();
        digitalWrite(PIN_LED_STATUS, !digitalRead(PIN_LED_STATUS));
    }

    // Recuperación: botón STOP → IDLE
    if (inputs.btnStartPressed) {
        _disableMotor();
        _status.currentDelay = STEP_DELAY_MAX_US;
        _status.state = STATE_IDLE;
        #if DEBUG_SERIAL
        Serial.println(F("[FSM] ERROR -> IDLE (recuperado)"));
        #endif
    }
}

// =============================================================================
// GENERACIÓN DE PASOS — Non-blocking con micros()
// Cada llamada verifica si pasó suficiente tiempo para el siguiente flanco.
// Un paso completo = 1 flanco HIGH + 1 flanco LOW (toggle).
// =============================================================================
void MotorController::_generateStep() {
    unsigned long now = micros();

    if (now - _lastStepTime >= _status.currentDelay) {
        _lastStepTime = now;
        _stepPinState = !_stepPinState;
        digitalWrite(PIN_STEP, _stepPinState ? HIGH : LOW);
    }
}

// =============================================================================
// ACELERACIÓN — Reducir delay en ACCEL_STEP_US cada ACCEL_INTERVAL_MS
// =============================================================================
void MotorController::_accelerate() {
    unsigned long now = millis();
    if (now - _lastAccelTick >= ACCEL_INTERVAL_MS) {
        _lastAccelTick = now;

        if (_status.currentDelay > _status.targetDelay + ACCEL_STEP_US) {
            _status.currentDelay -= ACCEL_STEP_US;
        } else {
            _status.currentDelay = _status.targetDelay;
        }
    }
}

// =============================================================================
// DECELERACIÓN — Incrementar delay en ACCEL_STEP_US cada ACCEL_INTERVAL_MS
// =============================================================================
void MotorController::_decelerate() {
    unsigned long now = millis();
    if (now - _lastAccelTick >= ACCEL_INTERVAL_MS) {
        _lastAccelTick = now;

        if (_status.currentDelay < STEP_DELAY_MAX_US - ACCEL_STEP_US) {
            _status.currentDelay += ACCEL_STEP_US;
        } else {
            _status.currentDelay = STEP_DELAY_MAX_US;
        }
    }
}

// =============================================================================
// UTILIDADES
// =============================================================================
void MotorController::_enableMotor() {
    digitalWrite(PIN_ENABLE, LOW);   // A4988: LOW = activo
    _status.enabled = true;
}

void MotorController::_disableMotor() {
    digitalWrite(PIN_ENABLE, HIGH);  // A4988: HIGH = deshabilitado
    _status.enabled = false;
}

void MotorController::_setDirection(MotorDirection dir) {
    _status.direction = dir;
    digitalWrite(PIN_DIR, dir);
    delay(DIR_CHANGE_DELAY_MS);  // Timing requerido por A4988 (>200ns, usamos 5ms por seguridad mecánica)
    #if DEBUG_SERIAL
    Serial.print(F("[MOTOR] Dir: "));
    Serial.println(dir == DIR_CW ? F("CW") : F("CCW"));
    #endif
}

void MotorController::_updateRPM() {
    if (_status.state == STATE_IDLE || _status.state == STATE_ERROR) {
        _status.currentRPM = 0.0;
        return;
    }
    // RPM = 60,000,000 / (currentDelay × 2 × EFFECTIVE_STEPS)
    // El ×2 es porque cada paso = 2 toggles (HIGH+LOW)
    float timePerStepSec = (_status.currentDelay * 2.0f) / 1000000.0f;
    float timePerRev = timePerStepSec * EFFECTIVE_STEPS;
    _status.currentRPM = (timePerRev > 0) ? (60.0f / timePerRev) : 0.0f;
}

void MotorController::_setMicrostepping(uint8_t mode) {
    // A4988 microstepping table:
    // MS1=L MS2=L MS3=L → Full step
    // MS1=H MS2=L MS3=L → Half step
    // MS1=L MS2=H MS3=L → Quarter step
    // MS1=H MS2=H MS3=L → Eighth step
    // MS1=H MS2=H MS3=H → Sixteenth step
    switch (mode) {
        case 1:
            digitalWrite(PIN_MS1, LOW);
            digitalWrite(PIN_MS2, LOW);
            digitalWrite(PIN_MS3, LOW);
            break;
        case 2:
            digitalWrite(PIN_MS1, HIGH);
            digitalWrite(PIN_MS2, LOW);
            digitalWrite(PIN_MS3, LOW);
            break;
        case 4:
            digitalWrite(PIN_MS1, LOW);
            digitalWrite(PIN_MS2, HIGH);
            digitalWrite(PIN_MS3, LOW);
            break;
        case 8:
            digitalWrite(PIN_MS1, HIGH);
            digitalWrite(PIN_MS2, HIGH);
            digitalWrite(PIN_MS3, LOW);
            break;
        case 16:
            digitalWrite(PIN_MS1, HIGH);
            digitalWrite(PIN_MS2, HIGH);
            digitalWrite(PIN_MS3, HIGH);
            break;
        default:
            digitalWrite(PIN_MS1, LOW);
            digitalWrite(PIN_MS2, LOW);
            digitalWrite(PIN_MS3, LOW);
            break;
    }
}
