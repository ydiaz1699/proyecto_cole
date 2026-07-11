/**
 * =============================================================================
 * display_manager.cpp — Implementación del gestor de display
 * =============================================================================
 */

#include "display_manager.h"

#if LCD_ENABLED
// Instancia global del LCD (pines definidos en config.h)
static LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);
#endif

void DisplayManager::init() {
    #if LCD_ENABLED
    lcd.begin(16, 2);
    _lastLcdUpdate = 0;
    #endif

    #if DEBUG_SERIAL
    Serial.begin(9600);
    _lastSerialReport = 0;
    #endif
}

void DisplayManager::showWelcome() {
    #if LCD_ENABLED
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("POSICIONADOR"));
    lcd.setCursor(0, 1);
    lcd.print(F("NEMA17 v"));
    lcd.print(FW_VERSION_MAJOR);
    lcd.print('.');
    lcd.print(FW_VERSION_MINOR);
    delay(2000);  // Solo en setup, no en loop
    lcd.clear();
    #endif

    #if DEBUG_SERIAL
    Serial.println(F(""));
    Serial.println(F("========================================"));
    Serial.println(F(" POSICIONADOR MOTORIZADO NEMA17 v2.0"));
    Serial.println(F(" Colegio Nacional San Luis - Tarija"));
    Serial.println(F("========================================"));
    Serial.println(F(" FSM: IDLE -> ACCEL -> RUN -> DECEL"));
    Serial.println(F(" ADC: Filtro promedio movil (8 muestras)"));
    Serial.println(F(" Aceleración: Rampa lineal 500ms"));
    Serial.println(F("========================================"));
    Serial.println(F(" Controles:"));
    Serial.println(F("   Pot A0  = Velocidad (30-300 RPM)"));
    Serial.println(F("   SW  D7  = Direccion (CW/CCW)"));
    Serial.println(F("   BTN D8  = Marcha/Parada"));
    Serial.println(F("========================================"));
    Serial.println(F(""));
    #endif
}

void DisplayManager::update(const MotorStatus& motor, const InputState& inputs) {
    #if LCD_ENABLED
    _updateLCD(motor, inputs);
    #endif

    #if DEBUG_SERIAL
    _reportSerial(motor, inputs);
    #endif
}

// =============================================================================
// LCD — Actualización con rate limiting (cada 500ms)
// Línea 1: "RPM:XXX.X   CW " o "CCW"
// Línea 2: Estado + barra de velocidad
// =============================================================================
#if LCD_ENABLED
void DisplayManager::_updateLCD(const MotorStatus& motor, const InputState& inputs) {
    unsigned long now = millis();
    if (now - _lastLcdUpdate < LCD_UPDATE_INTERVAL_MS) return;
    _lastLcdUpdate = now;

    // --- Línea 1: RPM y dirección ---
    lcd.setCursor(0, 0);
    lcd.print(F("RPM:"));

    if (motor.state == STATE_IDLE || motor.state == STATE_ERROR) {
        lcd.print(F("  0.0"));
    } else {
        if (motor.currentRPM < 100.0f) lcd.print(' ');
        if (motor.currentRPM < 10.0f) lcd.print(' ');
        lcd.print(motor.currentRPM, 1);
    }

    lcd.print(F("  "));
    lcd.setCursor(13, 0);
    lcd.print(motor.direction == DIR_CW ? F("CW ") : F("CCW"));

    // --- Línea 2: Estado + barra ---
    lcd.setCursor(0, 1);

    switch (motor.state) {
        case STATE_IDLE:
            lcd.print(F("STOP            "));
            break;
        case STATE_ACCELERATING:
            lcd.print(F("ACC "));
            goto showBar;
        case STATE_RUNNING:
            lcd.print(F("RUN "));
            goto showBar;
        case STATE_DECELERATING:
            lcd.print(F("DEC "));
            goto showBar;
        case STATE_ERROR:
            lcd.print(F("ERR!            "));
            break;
        showBar:
        {
            // Barra de velocidad (0-10 bloques)
            uint8_t bars = map(inputs.potRaw, 0, 1023, 0, 10);
            for (uint8_t i = 0; i < 10; i++) {
                lcd.write(i < bars ? (uint8_t)255 : ' ');
            }
            lcd.print(F("  "));
            break;
        }
    }
}
#endif

// =============================================================================
// SERIAL — Reporte de estado cada 2 segundos
// =============================================================================
#if DEBUG_SERIAL
void DisplayManager::_reportSerial(const MotorStatus& motor, const InputState& inputs) {
    unsigned long now = millis();
    if (now - _lastSerialReport < SERIAL_REPORT_INTERVAL_MS) return;
    _lastSerialReport = now;

    // Nombre del estado
    const __FlashStringHelper* stateName;
    switch (motor.state) {
        case STATE_IDLE:         stateName = F("IDLE"); break;
        case STATE_ACCELERATING: stateName = F("ACCEL"); break;
        case STATE_RUNNING:      stateName = F("RUN"); break;
        case STATE_DECELERATING: stateName = F("DECEL"); break;
        case STATE_ERROR:        stateName = F("ERROR"); break;
        default:                 stateName = F("???"); break;
    }

    Serial.print(F("[STATUS] State="));
    Serial.print(stateName);
    Serial.print(F(" | RPM="));
    Serial.print(motor.currentRPM, 1);
    Serial.print(F(" | Dir="));
    Serial.print(motor.direction == DIR_CW ? F("CW") : F("CCW"));
    Serial.print(F(" | Pot="));
    Serial.print(inputs.potRaw);
    Serial.print(F(" | Delay="));
    Serial.print(motor.currentDelay);
    Serial.println(F("us"));
}
#endif
