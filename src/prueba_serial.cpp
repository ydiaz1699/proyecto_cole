/**
 * =============================================================================
 * prueba_serial.cpp — Control y Prueba del Motor NEMA17 por Monitor Serial
 * =============================================================================
 * Proyecto: Posicionador Motorizado NEMA17
 * Colegio Nacional San Luis — Tarija, Bolivia
 * 
 * Hardware requerido para esta prueba:
 *   - Arduino Nano
 *   - Driver HR4988 / A4988 (con VREF ajustado a ~0.92V)
 *   - Motor paso a paso KS42STH34-1504A
 *   - Fuente de 12V (con capacitor 100uF en VMOT)
 *   - (NO requiere potenciómetro ni pulsadores físicos)
 * =============================================================================
 */

#include <Arduino.h>

// Definición de pines (iguales al proyecto final)
const uint8_t PIN_STEP   = 3;
const uint8_t PIN_DIR    = 4;
const uint8_t PIN_ENABLE = 5;

// Variables de estado
bool isDriverEnabled = true;       // LOW = habilitado en A4988/HR4988
bool isRunningContinuously = false;
bool currentDirection = HIGH;       // HIGH = CW, LOW = CCW
unsigned long stepIntervalMicros = 2000; // 2000µs = ~75 RPM (arranque suave y seguro)
unsigned long lastStepTime = 0;

void printMenu() {
    Serial.println();
    Serial.println(F("=================================================="));
    Serial.println(F("   PRUEBA MOTOR NEMA17 — COMANDOS POR SERIAL      "));
    Serial.println(F("=================================================="));
    Serial.println(F(" [1] Dar 1 vuelta completa en sentido Horario (CW)"));
    Serial.println(F(" [2] Dar 1 vuelta completa en sentido Antihorario (CCW)"));
    Serial.println(F(" [m] Marcha continua: INICIAR / PARAR (Toggle)"));
    Serial.println(F(" [d] Invertir dirección de giro"));
    Serial.println(F(" [+] Aumentar velocidad"));
    Serial.println(F(" [-] Disminuir velocidad"));
    Serial.println(F(" [e] Habilitar / Deshabilitar motor (Enable/Disable)"));
    Serial.println(F(" [h] Volver a mostrar este menú"));
    Serial.println(F("=================================================="));
    Serial.print(F("Estado actual: "));
    Serial.print(isDriverEnabled ? F("ACTIVO | ") : F("DESHABILITADO (suelto) | "));
    Serial.print(currentDirection ? F("Dir: HORARIO | ") : F("Dir: ANTIHORARIO | "));
    Serial.print(F("Intervalo: "));
    Serial.print(stepIntervalMicros);
    Serial.println(F(" us/paso"));
    Serial.println();
}

void stepMotor() {
    digitalWrite(PIN_STEP, HIGH);
    delayMicroseconds(10); // Pulso mínimo del A4988 es 1us
    digitalWrite(PIN_STEP, LOW);
}

void rotateSteps(int steps) {
    if (!isDriverEnabled) {
        digitalWrite(PIN_ENABLE, LOW); // Activamos temporalmente
        isDriverEnabled = true;
        delay(5);
    }
    
    digitalWrite(PIN_DIR, currentDirection);
    
    for (int i = 0; i < steps; i++) {
        stepMotor();
        delayMicroseconds(stepIntervalMicros);
    }
}

void setup() {
    // Configurar pines
    pinMode(PIN_STEP, OUTPUT);
    pinMode(PIN_DIR, OUTPUT);
    pinMode(PIN_ENABLE, OUTPUT);

    // Activar driver por defecto (LOW = activo)
    digitalWrite(PIN_ENABLE, LOW);
    digitalWrite(PIN_DIR, currentDirection);
    digitalWrite(PIN_STEP, LOW);

    // Iniciar comunicación Serial a 115200 baudios
    Serial.begin(115200);
    delay(200);

    printMenu();
}

void loop() {
    // 1. Escuchar comandos por el Monitor Serial
    if (Serial.available() > 0) {
        char cmd = Serial.read();

        // Ignorar saltos de línea y retorno de carro
        if (cmd == '\r' || cmd == '\n') return;

        switch (cmd) {
            case '1':
                isRunningContinuously = false;
                currentDirection = HIGH;
                Serial.println(F(">> Girando 1 vuelta (200 pasos) Horario..."));
                rotateSteps(200);
                Serial.println(F(">> Listo."));
                break;

            case '2':
                isRunningContinuously = false;
                currentDirection = LOW;
                Serial.println(F(">> Girando 1 vuelta (200 pasos) Antihorario..."));
                rotateSteps(200);
                Serial.println(F(">> Listo."));
                break;

            case 'm':
            case 'M':
                isRunningContinuously = !isRunningContinuously;
                if (isRunningContinuously) {
                    if (!isDriverEnabled) {
                        digitalWrite(PIN_ENABLE, LOW);
                        isDriverEnabled = true;
                    }
                    Serial.println(F(">> Marcha continua: INICIADA (Presiona 'm' o 'p' para parar)"));
                } else {
                    Serial.println(F(">> Marcha continua: DETENIDA"));
                }
                break;

            case 'p':
            case 'P':
                isRunningContinuously = false;
                Serial.println(F(">> Motor DETENIDO"));
                break;

            case 'd':
            case 'D':
                currentDirection = !currentDirection;
                digitalWrite(PIN_DIR, currentDirection);
                Serial.print(F(">> Dirección cambiada a: "));
                Serial.println(currentDirection ? F("HORARIO (CW)") : F("ANTIHORARIO (CCW)"));
                break;

            case '+':
                if (stepIntervalMicros > 500) {
                    stepIntervalMicros -= 200; // Menos microsegundos = más velocidad
                    Serial.print(F(">> Más rápido. Intervalo: "));
                    Serial.print(stepIntervalMicros);
                    Serial.println(F(" us"));
                } else {
                    Serial.println(F(">> Velocidad máxima alcanzada para esta prueba"));
                }
                break;

            case '-':
                if (stepIntervalMicros < 10000) {
                    stepIntervalMicros += 200; // Más microsegundos = menos velocidad
                    Serial.print(F(">> Más lento. Intervalo: "));
                    Serial.print(stepIntervalMicros);
                    Serial.println(F(" us"));
                }
                break;

            case 'e':
            case 'E':
                isDriverEnabled = !isDriverEnabled;
                digitalWrite(PIN_ENABLE, isDriverEnabled ? LOW : HIGH);
                isRunningContinuously = false;
                Serial.print(F(">> Driver: "));
                Serial.println(isDriverEnabled ? F("HABILITADO (eje bloqueado/listo)") : F("DESHABILITADO (eje libre/frío)"));
                break;

            case 'h':
            case 'H':
                printMenu();
                break;

            default:
                Serial.print(F("Comando desconocido: '"));
                Serial.print(cmd);
                Serial.println(F("'. Escribe 'h' para ver el menú."));
                break;
        }
    }

    // 2. Si está en marcha continua, dar pasos periódicamente
    if (isRunningContinuously && isDriverEnabled) {
        unsigned long currentMicros = micros();
        if (currentMicros - lastStepTime >= stepIntervalMicros) {
            lastStepTime = currentMicros;
            stepMotor();
        }
    }
}
