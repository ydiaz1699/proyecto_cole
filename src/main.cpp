/**
 * =============================================================================
 * main.cpp — Entry Point del Posicionador Motorizado NEMA17
 * =============================================================================
 * Versión: 2.0 (Nivel 2 — Robusto)
 * Proyecto de Grado - Colegio Nacional San Luis, Tarija, Bolivia
 *
 * Estudiantes: Jhonny Elías Díaz Moyata, Rodrigo Ángel Nava Flores
 * Profesor: Marco Antonio Vargas
 *
 * Metodología:
 *   - guia-crecimiento-de-codigo.md (Nivel 1 → Nivel 2)
 *   - skill-analisis-profundo.md (3 capas de análisis)
 *   - vsCode-AI Context Engineering v3 (documentación .ai/)
 *
 * Arquitectura:
 *   main.cpp ─── setup() + loop()
 *       ├── InputManager   — Filtrado ADC, debounce, selector
 *       ├── MotorController — FSM (IDLE/ACCEL/RUN/DECEL/ERROR)
 *       └── DisplayManager  — LCD + Serial con rate limiting
 *
 * Loop cycle time: ~50-100µs (sin LCD), ~200µs (con LCD)
 * Esto garantiza que los pasos STEP se generan a tiempo incluso a 300 RPM
 * (delay mínimo entre pasos = 500µs >> loop cycle time).
 * =============================================================================
 */

#include "config.h"
#include "input_manager.h"
#include "motor_controller.h"
#include "display_manager.h"

// === Instancias globales de los módulos ===
InputManager    inputManager;
MotorController motorController;
DisplayManager  displayManager;

// =============================================================================
// SETUP — Inicialización (se ejecuta una vez)
// =============================================================================
void setup() {
    // 1. Inicializar módulos (orden importa: inputs primero, luego motor)
    inputManager.init();
    motorController.init();
    displayManager.init();

    // 2. Pantalla de bienvenida
    displayManager.showWelcome();

    // 3. Lectura inicial para establecer estado base
    inputManager.update();
}

// =============================================================================
// LOOP — Ciclo principal (se ejecuta continuamente, ~10-50 kHz)
// =============================================================================
void loop() {
    // 1. LEER entradas (filtrado + debounce)
    inputManager.update();
    const InputState& inputs = inputManager.getState();

    // 2. ACTUALIZAR la FSM del motor (procesa entradas, genera pasos)
    motorController.update(inputs);
    const MotorStatus& motorStatus = motorController.getStatus();

    // 3. ACTUALIZAR display (con rate limiting, no bloquea)
    displayManager.update(motorStatus, inputs);
}
