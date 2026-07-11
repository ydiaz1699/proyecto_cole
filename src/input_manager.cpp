/**
 * =============================================================================
 * input_manager.cpp — Implementación del gestor de entradas
 * =============================================================================
 */

#include "input_manager.h"

void InputManager::init() {
    // Configurar pines de entrada
    pinMode(PIN_POT, INPUT);
    pinMode(PIN_DIR_SWITCH, INPUT_PULLUP);
    pinMode(PIN_BTN_START, INPUT_PULLUP);

    // Inicializar filtro ADC
    _adcIndex = 0;
    _adcSum = 0;
    _lastAdcRead = 0;
    for (uint8_t i = 0; i < ADC_FILTER_SIZE; i++) {
        _adcBuffer[i] = 0;
    }

    // Prellenar el buffer con lecturas reales (evita transitorio inicial)
    for (uint8_t i = 0; i < ADC_FILTER_SIZE; i++) {
        uint16_t reading = analogRead(PIN_POT);
        _adcBuffer[i] = reading;
        _adcSum += reading;
    }

    // Inicializar debounce
    _lastBtnRaw = HIGH;        // Pull-up: sin presionar = HIGH
    _btnFiltered = HIGH;
    _btnDebounceTime = 0;

    // Inicializar selector de dirección
    _lastDirState = digitalRead(PIN_DIR_SWITCH);

    // Inicializar estado de salida
    _state.potRaw = _adcSum >> ADC_FILTER_SHIFT;
    _state.targetDelay = STEP_DELAY_MAX_US;
    _state.btnStartPressed = false;
    _state.dirSwitchState = _lastDirState;
    _state.dirChanged = false;
    _state.potInDeadZone = true;

    _mapPotToDelay();
}

void InputManager::update() {
    // Resetear flags de un solo ciclo
    _state.btnStartPressed = false;
    _state.dirChanged = false;

    // Leer todas las entradas
    _readPotFiltered();
    _readButton();
    _readDirSwitch();
}

const InputState& InputManager::getState() const {
    return _state;
}

// =============================================================================
// FILTRADO ADC — Promedio móvil de 8 muestras
// =============================================================================
void InputManager::_readPotFiltered() {
    unsigned long now = millis();
    if (now - _lastAdcRead < ADC_READ_INTERVAL_MS) return;
    _lastAdcRead = now;

    // Leer nueva muestra
    uint16_t reading = analogRead(PIN_POT);

    // Actualizar buffer circular
    _adcSum -= _adcBuffer[_adcIndex];      // Restar muestra vieja
    _adcBuffer[_adcIndex] = reading;        // Guardar nueva
    _adcSum += reading;                     // Sumar nueva
    _adcIndex = (_adcIndex + 1) & (ADC_FILTER_SIZE - 1);  // Avanzar índice (circular con AND)

    // Calcular promedio (shift en vez de división: >>3 = /8)
    _state.potRaw = _adcSum >> ADC_FILTER_SHIFT;

    // Detectar zona muerta
    _state.potInDeadZone = (_state.potRaw < POT_DEAD_ZONE_LOW);

    // Mapear a delay
    _mapPotToDelay();
}

// =============================================================================
// MAPEO — ADC (0-1023) → Delay entre pasos (µs)
// Pot en 0 (izquierda)  → delay MÁXIMO = giro LENTO
// Pot en 1023 (derecha) → delay MÍNIMO = giro RÁPIDO
// =============================================================================
void InputManager::_mapPotToDelay() {
    if (_state.potInDeadZone) {
        _state.targetDelay = STEP_DELAY_MAX_US;  // Zona muerta = más lento posible
        return;
    }

    // Clamp al rango útil
    uint16_t potClamped = _state.potRaw;
    if (potClamped > POT_DEAD_ZONE_HIGH) potClamped = POT_DEAD_ZONE_HIGH;

    // Mapear: pot alto → delay bajo (rápido), pot bajo → delay alto (lento)
    _state.targetDelay = map(potClamped,
                             POT_DEAD_ZONE_LOW, POT_DEAD_ZONE_HIGH,
                             STEP_DELAY_MAX_US, STEP_DELAY_MIN_US);
}

// =============================================================================
// DEBOUNCE — Botón de marcha/parada
// Detecta flanco de bajada (pulsación) con anti-rebote de 50ms.
// Establece btnStartPressed=true por UN SOLO ciclo de loop().
// =============================================================================
void InputManager::_readButton() {
    bool rawState = digitalRead(PIN_BTN_START);

    // Si cambió, reiniciar timer de debounce
    if (rawState != _lastBtnRaw) {
        _btnDebounceTime = millis();
    }
    _lastBtnRaw = rawState;

    // Si pasó suficiente tiempo, aceptar el nuevo estado
    if ((millis() - _btnDebounceTime) > DEBOUNCE_MS) {
        if (rawState != _btnFiltered) {
            _btnFiltered = rawState;

            // Detectar flanco de bajada (botón presionado = LOW con pull-up)
            if (_btnFiltered == LOW) {
                _state.btnStartPressed = true;
            }
        }
    }
}

// =============================================================================
// SELECTOR DE DIRECCIÓN — Cambio de estado
// Solo reporta dirChanged=true una vez por cambio.
// =============================================================================
void InputManager::_readDirSwitch() {
    bool currentState = digitalRead(PIN_DIR_SWITCH);

    if (currentState != _lastDirState) {
        _lastDirState = currentState;
        _state.dirSwitchState = currentState;
        _state.dirChanged = true;
    }
}
