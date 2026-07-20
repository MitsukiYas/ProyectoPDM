#include <AccelStepper.h>

// =====================================================
// PINES Y VARIABLES DE CONTROL
// =====================================================

// Pines del sistema de traslación (Carro)
const int carroDirPin  = 2;
const int carroStepPin = 3;

// Pin del sensor inductivo
const int pinSensorInductivo = 10;

// Instancia del motor
AccelStepper carroStepper(AccelStepper::DRIVER, carroStepPin, carroDirPin);

void setup() {
  Serial.begin(115200);

  // Configurar el sensor inductivo con su resistencia pull-up
  pinMode(pinSensorInductivo, INPUT_PULLUP);

  // Configuración del motor del carro
  carroStepper.setMaxSpeed(3000);   // Velocidad máxima permitida
  carroStepper.setSpeed(1000);      // Velocidad de prueba (pasos por segundo)
  
  // Puedes cambiar la dirección de giro inicial usando un número negativo en setSpeed, 
  // o configurando directamente los pines aquí si hiciera falta.

  Serial.println("Iniciando sistema...");
  Serial.println("Esperando 3 segundos antes de mover la traslacion...");
  delay(3000);
  Serial.println("Moviendo carro...");
}

void loop() {
  // 1. Leemos el estado del sensor
  int estadoInductivo = digitalRead(pinSensorInductivo);

  // 2. Lógica de movimiento
  if (estadoInductivo == HIGH) {
    // HIGH significa que NO ha detectado metal (asumiendo que al detectar baja a GND / LOW)
    // Mantenemos la velocidad
    carroStepper.setSpeed(1000);
    // runSpeed() genera los pulsos necesarios para mantener la velocidad actual. 
    // Debe llamarse constantemente.
    carroStepper.runSpeed(); 
  } else {
    // LOW significa que detectó metal.
    // Dejamos de llamar a runSpeed() y el motor se detiene de inmediato.
    Serial.println("¡METAL DETECTADO! - Motor detenido.");
    
    // Un pequeño delay para no inundar el monitor serial con mensajes.
    // Ojo: Si usáramos delay mientras el motor se debe mover, arruinaríamos el movimiento.
    // Pero como aquí ya queremos que esté detenido, no hay problema.
    delay(500);
  }
}