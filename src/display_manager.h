/**
 * =============================================================================
 * display_manager.h — Gestión de Display (LCD y Serial)
 * =============================================================================
 * Responsabilidades:
 *   - Actualizar LCD 16x2 con rate limiting (500ms)
 *   - Mostrar: RPM, dirección, estado, barra de velocidad
 *   - Reportar por Serial para debug (2s)
 *   - Pantalla de bienvenida al arrancar
 *
 * Principio: El display NUNCA bloquea la generación de pasos.
 *            Solo se actualiza si pasó suficiente tiempo.
 * =============================================================================
 */

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "config.h"
#include "motor_controller.h"
#include "input_manager.h"

#if LCD_ENABLED
#include <LiquidCrystal.h>
#endif

class DisplayManager {
public:
    void init();
    void update(const MotorStatus& motor, const InputState& inputs);
    void showWelcome();

private:
    #if LCD_ENABLED
    unsigned long _lastLcdUpdate;
    void _updateLCD(const MotorStatus& motor, const InputState& inputs);
    #endif

    #if DEBUG_SERIAL
    unsigned long _lastSerialReport;
    void _reportSerial(const MotorStatus& motor, const InputState& inputs);
    #endif
};

#endif // DISPLAY_MANAGER_H
