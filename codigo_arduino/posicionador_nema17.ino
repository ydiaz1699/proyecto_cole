/*
 * ============================================================
 * POSICIONADOR MOTORIZADO CON MOTOR PASO A PASO NEMA17
 * ============================================================
 * Proyecto de Grado - Colegio Nacional San Luis
 * Seccion Industrial - Mecanica Industrial
 * Tarija, Bolivia
 * 
 * Estudiante: Jhonny Elias Diaz Moyata
 * Estudiante 2: Rodrigo Angel Nava Flores
 * 
 * Motor: NEMA17 JK42HS40-1684 (1.68A, 3.6 kg.cm)
 * Driver: A4988 o DRV8825
 * Control: Potenciometro de velocidad + Selector de direccion
 * 
 * DESCRIPCION:
 * Este programa controla un motor paso a paso NEMA17 mediante
 * un driver A4988/DRV8825 para un posicionador de soldadura.
 * Permite regular la velocidad con un potenciometro y cambiar
 * el sentido de giro con un switch selector.
 * ============================================================
 */

// ===================== PINES DE CONEXION =====================
// Pines del driver A4988/DRV8825
const int PIN_STEP = 3;       // Pin STEP del driver (pulsos de paso)
const int PIN_DIR  = 4;       // Pin DIR del driver (direccion de giro)
const int PIN_ENABLE = 5;     // Pin ENABLE del driver (activar/desactivar motor)

// Pines de control del usuario
const int PIN_POTENCIOMETRO = A0;  // Potenciometro 10kOhm para velocidad
const int PIN_SELECTOR_DIR  = 7;   // Switch selector de direccion (CW/CCW)
const int PIN_BOTON_MARCHA  = 8;   // Boton de marcha/parada (opcional)

// Pin de microstepping (opcional - para A4988/DRV8825)
const int PIN_MS1 = 9;       // Microstepping bit 1
const int PIN_MS2 = 10;      // Microstepping bit 2
const int PIN_MS3 = 11;      // Microstepping bit 3

// ===================== CONFIGURACION =====================
// Parametros del motor NEMA17
const int PASOS_POR_VUELTA = 200;     // 200 pasos = 360 grados (1.8 por paso)
const int MICROSTEPPING = 1;           // 1=paso completo, 2=medio paso, 4=1/4, 8=1/8, 16=1/16

// Limites de velocidad (en microsegundos entre pasos)
const long VELOCIDAD_MIN = 5000;   // Velocidad minima (delay largo = giro lento)
const long VELOCIDAD_MAX = 500;    // Velocidad maxima (delay corto = giro rapido)

// ===================== VARIABLES GLOBALES =====================
long delayEntrepasos = VELOCIDAD_MIN;  // Delay actual entre pasos
bool direccion = true;                  // true = horario (CW), false = antihorario (CCW)
bool motorActivo = true;                // Estado del motor (marcha/parada)
int valorPotenciometro = 0;            // Lectura del potenciometro (0-1023)

// Variables para control sin delay (non-blocking)
unsigned long tiempoAnterior = 0;
bool estadoStep = LOW;

// ===================== SETUP =====================
void setup() {
  // Configurar pines de salida (driver)
  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);
  
  // Configurar pines de microstepping
  pinMode(PIN_MS1, OUTPUT);
  pinMode(PIN_MS2, OUTPUT);
  pinMode(PIN_MS3, OUTPUT);
  
  // Configurar pines de entrada (controles)
  pinMode(PIN_SELECTOR_DIR, INPUT_PULLUP);  // Pull-up interno
  pinMode(PIN_BOTON_MARCHA, INPUT_PULLUP);  // Pull-up interno
  
  // Estado inicial
  digitalWrite(PIN_ENABLE, LOW);   // Motor habilitado (LOW = activo en A4988)
  digitalWrite(PIN_DIR, HIGH);     // Direccion inicial: horario
  digitalWrite(PIN_STEP, LOW);     // Step en bajo
  
  // Configurar microstepping (paso completo por defecto)
  configurarMicrostepping(MICROSTEPPING);
  
  // Iniciar comunicacion serial (para debug/monitoreo)
  Serial.begin(9600);
  Serial.println("=================================");
  Serial.println("POSICIONADOR MOTORIZADO NEMA17");
  Serial.println("Colegio Nacional San Luis");
  Serial.println("=================================");
  Serial.println("Sistema listo. Ajuste velocidad con potenciometro.");
  Serial.println("");
}

// ===================== LOOP PRINCIPAL =====================
void loop() {
  // 1. Leer el potenciometro de velocidad
  leerVelocidad();
  
  // 2. Leer el selector de direccion
  leerDireccion();
  
  // 3. Leer boton de marcha/parada
  leerBotonMarcha();
  
  // 4. Generar pulsos de paso si el motor esta activo
  if (motorActivo) {
    generarPaso();
  }
}

// ===================== FUNCIONES =====================

/*
 * leerVelocidad()
 * Lee el potenciometro y mapea su valor al rango de velocidad.
 * Potenciometro en 0 = velocidad minima (giro lento)
 * Potenciometro en 1023 = velocidad maxima (giro rapido)
 */
void leerVelocidad() {
  valorPotenciometro = analogRead(PIN_POTENCIOMETRO);
  
  // Mapear: potenciometro 0-1023 -> delay entre pasos
  // Valor alto de delay = lento, valor bajo = rapido
  delayEntrepasos = map(valorPotenciometro, 0, 1023, VELOCIDAD_MIN, VELOCIDAD_MAX);
  
  // Zona muerta: si potenciometro esta casi en 0, detener motor
  if (valorPotenciometro < 10) {
    motorActivo = false;
    digitalWrite(PIN_ENABLE, HIGH);  // Desactivar motor (ahorro energia)
  } else {
    if (!motorActivo && digitalRead(PIN_BOTON_MARCHA) == LOW) {
      // Solo reactivar si el boton de marcha no lo impide
    } else if (valorPotenciometro >= 10) {
      motorActivo = true;
      digitalWrite(PIN_ENABLE, LOW);   // Activar motor
    }
  }
}

/*
 * leerDireccion()
 * Lee el switch selector y establece la direccion de giro.
 * Switch en HIGH (abierto con pull-up) = Horario (CW)
 * Switch en LOW (cerrado a GND) = Antihorario (CCW)
 */
void leerDireccion() {
  bool nuevoEstado = digitalRead(PIN_SELECTOR_DIR);
  
  if (nuevoEstado != direccion) {
    direccion = nuevoEstado;
    digitalWrite(PIN_DIR, direccion ? HIGH : LOW);
    
    // Pequena pausa al cambiar direccion (proteccion mecanica)
    delay(5);
    
    // Debug por serial
    Serial.print("Direccion: ");
    Serial.println(direccion ? "HORARIO (CW)" : "ANTIHORARIO (CCW)");
  }
}

/*
 * leerBotonMarcha()
 * Lee el boton de marcha/parada con antirrebote (debounce).
 * Presionar una vez = parar, presionar de nuevo = arrancar.
 */
void leerBotonMarcha() {
  static bool ultimoEstadoBoton = HIGH;
  static unsigned long tiempoDebounce = 0;
  const unsigned long DEBOUNCE_DELAY = 50;  // 50ms anti-rebote
  
  bool estadoBoton = digitalRead(PIN_BOTON_MARCHA);
  
  if (estadoBoton != ultimoEstadoBoton) {
    tiempoDebounce = millis();
  }
  
  if ((millis() - tiempoDebounce) > DEBOUNCE_DELAY) {
    static bool estadoFiltrado = HIGH;
    if (estadoBoton != estadoFiltrado) {
      estadoFiltrado = estadoBoton;
      
      // Detectar flanco de bajada (boton presionado)
      if (estadoFiltrado == LOW) {
        motorActivo = !motorActivo;  // Toggle marcha/parada
        
        if (motorActivo) {
          digitalWrite(PIN_ENABLE, LOW);  // Activar motor
          Serial.println("Motor: MARCHA");
        } else {
          digitalWrite(PIN_ENABLE, HIGH); // Desactivar motor
          Serial.println("Motor: PARADO");
        }
      }
    }
  }
  
  ultimoEstadoBoton = estadoBoton;
}

/*
 * generarPaso()
 * Genera un pulso en el pin STEP para avanzar un paso.
 * Usa control de tiempo no bloqueante (sin delay()) para
 * que el programa siga respondiendo a los controles.
 */
void generarPaso() {
  unsigned long tiempoActual = micros();
  
  if (tiempoActual - tiempoAnterior >= delayEntrepasos) {
    tiempoAnterior = tiempoActual;
    
    // Generar pulso: HIGH-LOW
    estadoStep = !estadoStep;
    digitalWrite(PIN_STEP, estadoStep);
  }
}

/*
 * configurarMicrostepping()
 * Configura los pines MS1, MS2, MS3 del driver A4988/DRV8825
 * para seleccionar el modo de microstepping.
 * 
 * A4988:
 *   MS1=L MS2=L MS3=L -> Paso completo
 *   MS1=H MS2=L MS3=L -> 1/2 paso
 *   MS1=L MS2=H MS3=L -> 1/4 paso
 *   MS1=H MS2=H MS3=L -> 1/8 paso
 *   MS1=H MS2=H MS3=H -> 1/16 paso
 * 
 * DRV8825:
 *   MS1=L MS2=L MS3=L -> Paso completo
 *   MS1=H MS2=L MS3=L -> 1/2 paso
 *   MS1=L MS2=H MS3=L -> 1/4 paso
 *   MS1=H MS2=H MS3=L -> 1/8 paso
 *   MS1=L MS2=L MS3=H -> 1/16 paso
 *   MS1=H MS2=L MS3=H -> 1/32 paso (solo DRV8825)
 */
void configurarMicrostepping(int modo) {
  switch (modo) {
    case 1:  // Paso completo
      digitalWrite(PIN_MS1, LOW);
      digitalWrite(PIN_MS2, LOW);
      digitalWrite(PIN_MS3, LOW);
      Serial.println("Microstepping: PASO COMPLETO (1/1)");
      break;
    case 2:  // Medio paso
      digitalWrite(PIN_MS1, HIGH);
      digitalWrite(PIN_MS2, LOW);
      digitalWrite(PIN_MS3, LOW);
      Serial.println("Microstepping: MEDIO PASO (1/2)");
      break;
    case 4:  // Cuarto de paso
      digitalWrite(PIN_MS1, LOW);
      digitalWrite(PIN_MS2, HIGH);
      digitalWrite(PIN_MS3, LOW);
      Serial.println("Microstepping: CUARTO PASO (1/4)");
      break;
    case 8:  // Octavo de paso
      digitalWrite(PIN_MS1, HIGH);
      digitalWrite(PIN_MS2, HIGH);
      digitalWrite(PIN_MS3, LOW);
      Serial.println("Microstepping: OCTAVO PASO (1/8)");
      break;
    case 16: // Dieciseisavo de paso
      digitalWrite(PIN_MS1, HIGH);
      digitalWrite(PIN_MS2, HIGH);
      digitalWrite(PIN_MS3, HIGH);
      Serial.println("Microstepping: 1/16 PASO");
      break;
    default: // Paso completo por defecto
      digitalWrite(PIN_MS1, LOW);
      digitalWrite(PIN_MS2, LOW);
      digitalWrite(PIN_MS3, LOW);
      Serial.println("Microstepping: PASO COMPLETO (default)");
      break;
  }
}

/*
 * calcularRPM()
 * Funcion auxiliar para mostrar las RPM actuales por serial.
 * RPM = 60 / (pasos_por_vuelta * microstepping * delay_us * 2 / 1000000)
 * El *2 es porque cada paso completo necesita un flanco HIGH y uno LOW.
 */
float calcularRPM() {
  if (delayEntrepasos <= 0) return 0;
  
  float tiempoPorPasoSeg = (delayEntrepasos * 2.0) / 1000000.0;  // *2 por toggle
  float pasosEfectivos = PASOS_POR_VUELTA * MICROSTEPPING;
  float tiempoPorVuelta = tiempoPorPasoSeg * pasosEfectivos;
  float rpm = 60.0 / tiempoPorVuelta;
  
  return rpm;
}

/*
 * mostrarEstado()
 * Imprime por serial el estado actual del sistema.
 * Llamar periodicamente para monitoreo (cada 1-2 segundos).
 */
void mostrarEstado() {
  static unsigned long ultimoReporte = 0;
  
  if (millis() - ultimoReporte >= 2000) {  // Cada 2 segundos
    ultimoReporte = millis();
    
    Serial.println("--- ESTADO DEL SISTEMA ---");
    Serial.print("Motor: ");
    Serial.println(motorActivo ? "ACTIVO" : "PARADO");
    Serial.print("Direccion: ");
    Serial.println(direccion ? "HORARIO" : "ANTIHORARIO");
    Serial.print("Potenciometro: ");
    Serial.println(valorPotenciometro);
    Serial.print("Delay entre pasos: ");
    Serial.print(delayEntrepasos);
    Serial.println(" us");
    Serial.print("RPM estimadas: ");
    Serial.println(calcularRPM(), 1);
    Serial.println("--------------------------");
    Serial.println("");
  }
}
