/*
 * ============================================================
 * POSICIONADOR MOTORIZADO CON MOTOR NEMA17 - VERSION CON LCD
 * ============================================================
 * Version mejorada con pantalla LCD 16x2 para mostrar:
 * - RPM actuales
 * - Direccion de giro
 * - Estado del motor (Marcha/Parado)
 * 
 * NOTA: Esta es una version OPCIONAL mejorada.
 * Si no tienes LCD, usa el archivo posicionador_nema17.ino
 * ============================================================
 */

#include <LiquidCrystal.h>

// ===================== PINES DE CONEXION =====================
// Pines del driver A4988/DRV8825
const int PIN_STEP = 3;
const int PIN_DIR  = 4;
const int PIN_ENABLE = 5;

// Pines de control
const int PIN_POTENCIOMETRO = A0;
const int PIN_SELECTOR_DIR  = 7;
const int PIN_BOTON_MARCHA  = 8;

// Pines LCD (RS, EN, D4, D5, D6, D7)
LiquidCrystal lcd(A1, A2, A3, A4, A5, 2);

// ===================== CONFIGURACION =====================
const int PASOS_POR_VUELTA = 200;
const int MICROSTEPPING = 1;
const long VELOCIDAD_MIN = 5000;
const long VELOCIDAD_MAX = 500;

// ===================== VARIABLES =====================
long delayEntrepasos = VELOCIDAD_MIN;
bool direccion = true;
bool motorActivo = true;
int valorPotenciometro = 0;
unsigned long tiempoAnterior = 0;
bool estadoStep = LOW;
unsigned long ultimoUpdateLCD = 0;

// Caracteres personalizados para LCD
byte flechaDerecha[8] = {
  0b00000,
  0b00100,
  0b00110,
  0b11111,
  0b11111,
  0b00110,
  0b00100,
  0b00000
};

byte flechaIzquierda[8] = {
  0b00000,
  0b00100,
  0b01100,
  0b11111,
  0b11111,
  0b01100,
  0b00100,
  0b00000
};

// ===================== SETUP =====================
void setup() {
  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);
  pinMode(PIN_SELECTOR_DIR, INPUT_PULLUP);
  pinMode(PIN_BOTON_MARCHA, INPUT_PULLUP);
  
  digitalWrite(PIN_ENABLE, LOW);
  digitalWrite(PIN_DIR, HIGH);
  digitalWrite(PIN_STEP, LOW);
  
  // Inicializar LCD
  lcd.begin(16, 2);
  lcd.createChar(0, flechaDerecha);
  lcd.createChar(1, flechaIzquierda);
  
  // Pantalla de bienvenida
  lcd.setCursor(0, 0);
  lcd.print("POSICIONADOR");
  lcd.setCursor(0, 1);
  lcd.print("NEMA17  v1.0");
  delay(2000);
  lcd.clear();
  
  Serial.begin(9600);
  Serial.println("Sistema con LCD iniciado.");
}

// ===================== LOOP =====================
void loop() {
  leerVelocidad();
  leerDireccion();
  leerBotonMarcha();
  
  if (motorActivo) {
    generarPaso();
  }
  
  actualizarLCD();
}

// ===================== FUNCIONES =====================

void leerVelocidad() {
  valorPotenciometro = analogRead(PIN_POTENCIOMETRO);
  delayEntrepasos = map(valorPotenciometro, 0, 1023, VELOCIDAD_MIN, VELOCIDAD_MAX);
  
  if (valorPotenciometro < 10) {
    motorActivo = false;
    digitalWrite(PIN_ENABLE, HIGH);
  } else if (valorPotenciometro >= 10 && motorActivo == false) {
    // Reactivar solo si no fue parado por boton
  }
}

void leerDireccion() {
  bool nuevoEstado = digitalRead(PIN_SELECTOR_DIR);
  if (nuevoEstado != direccion) {
    direccion = nuevoEstado;
    digitalWrite(PIN_DIR, direccion ? HIGH : LOW);
    delay(5);
  }
}

void leerBotonMarcha() {
  static bool ultimoEstadoBoton = HIGH;
  static unsigned long tiempoDebounce = 0;
  
  bool estadoBoton = digitalRead(PIN_BOTON_MARCHA);
  
  if (estadoBoton != ultimoEstadoBoton) {
    tiempoDebounce = millis();
  }
  
  if ((millis() - tiempoDebounce) > 50) {
    static bool estadoFiltrado = HIGH;
    if (estadoBoton != estadoFiltrado) {
      estadoFiltrado = estadoBoton;
      if (estadoFiltrado == LOW) {
        motorActivo = !motorActivo;
        digitalWrite(PIN_ENABLE, motorActivo ? LOW : HIGH);
      }
    }
  }
  
  ultimoEstadoBoton = estadoBoton;
}

void generarPaso() {
  unsigned long tiempoActual = micros();
  if (tiempoActual - tiempoAnterior >= delayEntrepasos) {
    tiempoAnterior = tiempoActual;
    estadoStep = !estadoStep;
    digitalWrite(PIN_STEP, estadoStep);
  }
}

float calcularRPM() {
  if (delayEntrepasos <= 0) return 0;
  float tiempoPorPasoSeg = (delayEntrepasos * 2.0) / 1000000.0;
  float pasosEfectivos = PASOS_POR_VUELTA * MICROSTEPPING;
  float tiempoPorVuelta = tiempoPorPasoSeg * pasosEfectivos;
  return 60.0 / tiempoPorVuelta;
}

/*
 * actualizarLCD()
 * Actualiza la pantalla LCD cada 500ms con:
 * Linea 1: "RPM: XX.X  [flecha]"
 * Linea 2: "MARCHA" o "PARADO" + barra de velocidad
 */
void actualizarLCD() {
  if (millis() - ultimoUpdateLCD < 500) return;
  ultimoUpdateLCD = millis();
  
  float rpm = calcularRPM();
  
  // Linea 1: RPM y direccion
  lcd.setCursor(0, 0);
  lcd.print("RPM:");
  
  if (motorActivo) {
    if (rpm < 10) lcd.print(" ");
    lcd.print(rpm, 1);
  } else {
    lcd.print(" 0.0");
  }
  
  lcd.print("  ");
  
  // Flecha de direccion
  lcd.setCursor(13, 0);
  if (direccion) {
    lcd.print("CW ");
  } else {
    lcd.print("CCW");
  }
  
  // Linea 2: Estado + barra de velocidad
  lcd.setCursor(0, 1);
  if (motorActivo) {
    lcd.print("RUN ");
    // Barra de velocidad (0-10 bloques)
    int barras = map(valorPotenciometro, 0, 1023, 0, 10);
    for (int i = 0; i < 10; i++) {
      if (i < barras) {
        lcd.write(255);  // Bloque lleno
      } else {
        lcd.print(" ");
      }
    }
    lcd.print("  ");
  } else {
    lcd.print("STOP            ");
  }
}
