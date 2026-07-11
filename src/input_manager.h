/**
 * =============================================================================
 * input_manager.h — Lectura Filtrada de Entradas del Usuario
 * =============================================================================
 * Responsabilidades:
 *   - Leer potenciómetro con filtro promedio móvil (8 muestras)
 *   - Debounce del botón de marcha/parada
 *   - Lectura del selector de dirección con detección de cambio
 *   - Mapeo de valor ADC a delay entre pasos (µs)
 *
 * Principio: TODA entrada física se filtra antes de usarse.
 * =============================================================================
 */

#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "config.h"

// === Estructura de estado de entradas ===
struct InputState {
    uint16_t potRaw;            // Valor crudo del ADC (0-1023) — promedio filtrado
    uint16_t targetDelay;       // Delay objetivo en µs (mapeado del pot)
    bool     btnStartPressed;   // TRUE solo en el flanco de bajada (un ciclo)
    bool     dirSwitchState;    // Estado actual del selector (HIGH=CW, LOW=CCW)
    bool     dirChanged;        // TRUE solo cuando el selector cambió (un ciclo)
    bool     potInDeadZone;     // TRUE si el pot está en zona muerta (<10)
};

class InputManager {
public:
    void init();
    void update();                      // Llamar en cada loop()
    const InputState& getState() const; // Obtener estado actual de entradas

private:
    InputState _state;

    // --- Filtro ADC (promedio móvil) ---
    uint16_t _adcBuffer[ADC_FILTER_SIZE];
    uint8_t  _adcIndex;
    uint16_t _adcSum;
    unsigned long _lastAdcRead;

    void _readPotFiltered();
    void _mapPotToDelay();

    // --- Debounce del botón ---
    bool _lastBtnRaw;
    bool _btnFiltered;
    unsigned long _btnDebounceTime;

    void _readButton();

    // --- Selector de dirección ---
    bool _lastDirState;

    void _readDirSwitch();
};

#endif // INPUT_MANAGER_H
