#include <AccelStepper.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "variables.h"
#include "automatico.h"

String comando = "";

int empotPulseDelay = 200;
const int stepsPerRevolution = 1600;

// =====================================================
// PINES Y VARIABLES DE CONTROL
// =====================================================

// Carro
const int carroDirPin  = 2;
const int carroStepPin = 3;
AccelStepper carroStepper(AccelStepper::DRIVER, carroStepPin, carroDirPin);
int carroVelocidad = 0;

// Estabilizadores (Empotramiento)
const int empotDirPin1   = 8;
const int empotPulsePin1 = 9;
const int empotDirPin2   = 6;
const int empotPulsePin2 = 7;
bool empotActivo = false, empotDireccion = true;
long empotPasosRestantes = 0;

// Enganche Inicial
const int engancheDirPin = 4;
const int engancheStepPin = 5;
bool engancheActivo = false, engancheDireccion = true, engancheModoPasos = false;
long enganchePasosRestantes = 0;

// Actuador Lineal
const int pinActuador = A0;
const int pinLectura = A1;
int posicionActual = 0;

// LEDs
const int pinLedActuador = A2;
const int pinLedPolea = A3;
const int pinLedEmpotramiento = A4;
const int pinLedCarro = A5;

//Sensores
const int carroSensorCap = 10;
const int sensorFinEmpot1 = 13;
const int sensorFinEmpot2 = 12;
const int sensorFinEnganche = 11;

// Vagones y autoclaves
int vagonesTrasladados = 0;
const int vagonesTotales = 3;
int autoclavesCompletos = 0;
const int autoclavesTotales = 2;

bool tareaCompletada = false;

void setup() {
  Serial.begin(115200);

  // Configuración del Carro
  carroStepper.setMaxSpeed(3000);
  carroStepper.setMinPulseWidth(20);
  pinMode(carroSensorCap, INPUT);

  // Configuración de Estabilizadores
  pinMode(empotDirPin1, OUTPUT);
  pinMode(empotPulsePin1, OUTPUT);
  pinMode(empotDirPin2, OUTPUT);
  pinMode(empotPulsePin2, OUTPUT);
  pinMode(sensorFinEmpot1, INPUT);
  pinMode(sensorFinEmpot2, INPUT);

  // Configuración de Enganche y Actuador
  pinMode(engancheDirPin, OUTPUT);
  pinMode(engancheStepPin, OUTPUT);
  pinMode(sensorFinEnganche, INPUT);
  pinMode(pinActuador, OUTPUT);
  pinMode(pinLectura, INPUT);

  // LEDs
  pinMode(pinLedActuador, OUTPUT);
  pinMode(pinLedPolea, OUTPUT);
  pinMode(pinLedEmpotramiento, OUTPUT);
  pinMode(pinLedCarro, OUTPUT);
}

void loop() {
  leerSerial();
  ejecutarCarro();
  ejecutarEmpot();
  ejecutarEnganche();
  ejecutarActuador(); // Monitoreo constante del actuador
  enviarEstadoSensores();
  ejecutarSecuenciaCompleta(); // Definido en automatico.h
}

void leerSerial() {
  if (Serial.available()) {
    comando = Serial.readStringUntil('\n');
    comando.trim();

    // Control del Carro (Solo establece intención)
    if (comando == "carro_forward") {
        carroVelocidad = 2000;
        digitalWrite(pinLedCarro, HIGH);
    }
    else if (comando == "carro_backward") {
        carroVelocidad = -2000;
        digitalWrite(pinLedCarro, HIGH);
    }
    else if (comando == "carro_stop") {
        carroVelocidad = 0;
        digitalWrite(pinLedCarro, LOW);
        tareaCompletada = true;
    }
    
    // Control de Empotramiento (Solo establece intención)
    else if (comando == "empot_forward") {
        empotActivo = true;
        empotDireccion = true;
        digitalWrite(pinLedEmpotramiento, HIGH);
    }
    else if (comando == "empot_backward") {
        empotActivo = true;
        empotDireccion = false;
        digitalWrite(pinLedEmpotramiento, HIGH);
    }
    else if (comando == "empot_stop") {
        empotActivo = false;
        digitalWrite(pinLedEmpotramiento, LOW);
        tareaCompletada = true;
    }
    
    // Control de Enganches (Solo establece intención)
    else if (comando == "enganche_enrollar") {
        engancheDireccion = true;
        engancheActivo = true;
        engancheModoPasos = false;
        digitalWrite(pinLedPolea, HIGH);
    }
    else if (comando == "enganche_desenrollar") {
        moverEnganchePasos(15, false);
        digitalWrite(pinLedPolea, HIGH);
    }
    else if (comando == "enganche_stop") {
        engancheActivo = false;
        engancheModoPasos = false;
        digitalWrite(pinLedPolea, LOW);
        tareaCompletada = true;
    }
    
    // Control del Actuador
    else if (comando == "act_extend") {
        digitalWrite(pinActuador, HIGH);
        digitalWrite(pinLedActuador, HIGH);
    }
    else if (comando == "act_retract") {
        digitalWrite(pinActuador, LOW);
        digitalWrite(pinLedActuador, HIGH);
    }
    
    // Comandos Globales
    else if (comando == "start_all") iniciarAutomatico();
    else if (comando == "stop_all") detenerTodo();
  }
}

void ejecutarCarro() {
  // SEGURIDAD: Si está avanzando y el sensor detecta, fuerza la parada
  if (carroVelocidad > 0 && digitalRead(carroSensorCap)) {
      carroVelocidad = 0;
      digitalWrite(pinLedCarro, LOW);
      if (!tareaCompletada) tareaCompletada = true;
  }
  
  carroStepper.setSpeed(carroVelocidad);
  carroStepper.runSpeed();
}

void ejecutarEmpot() {
  if (!empotActivo) return;

  // SEGURIDAD: Si está bajando (true) y los topes se presionan, se detiene
  if (empotDireccion == true && digitalRead(sensorFinEmpot1) && digitalRead(sensorFinEmpot2)) {
      empotActivo = false;
      digitalWrite(pinLedEmpotramiento, LOW);
      if (!tareaCompletada) tareaCompletada = true;
      return;
  }

  digitalWrite(empotDirPin1, empotDireccion ? LOW : HIGH);
  digitalWrite(empotDirPin2, empotDireccion ? LOW : HIGH);

  static unsigned long ultimoPulso = 0;
  if (micros() - ultimoPulso >= empotPulseDelay) {
    ultimoPulso = micros();
    static bool estadoPulso = false;
    estadoPulso = !estadoPulso;
    digitalWrite(empotPulsePin1, estadoPulso ? HIGH : LOW);
    digitalWrite(empotPulsePin2, estadoPulso ? HIGH : LOW);
  }
}

void ejecutarEnganche() {
  if (!engancheActivo) return;

  // SEGURIDAD: Si está enrollando (true) y el sensor detecta, se detiene
  if (engancheDireccion == true && digitalRead(sensorFinEnganche)) {
      engancheActivo = false;
      digitalWrite(pinLedPolea, LOW);
      if (!tareaCompletada) tareaCompletada = true;
      return;
  }

  digitalWrite(engancheDirPin, engancheDireccion ? HIGH : LOW);
  static unsigned long ultimoPulsoEng = 0;
  if (micros() - ultimoPulsoEng >= 1000) {
    ultimoPulsoEng = micros();
    static bool estadoPulsoEng = false;
    estadoPulsoEng = !estadoPulsoEng;
    digitalWrite(engancheStepPin, estadoPulsoEng);
    
    // Control por conteo de pasos
    if (estadoPulsoEng && engancheModoPasos) {
      enganchePasosRestantes--;
      if (enganchePasosRestantes <= 0) {
        engancheActivo = false;
        engancheModoPasos = false;
        digitalWrite(pinLedPolea, LOW);
        tareaCompletada = true;
      }
    }
  }
}

void ejecutarActuador() {
  // Lógica para evaluar constantemente la posición y apagar el LED
  if (digitalRead(pinActuador) == HIGH) {
      posicionActual = map(analogRead(pinLectura), 0, 1023, 0, 255);
      if (posicionActual > 89) {
          digitalWrite(pinLedActuador, LOW);
          static bool avisoExtend = false;
          if (!avisoExtend) {
              tareaCompletada = true;
              avisoExtend = true;
          }
      }
  } else {
      posicionActual = constrain(map(analogRead(pinLectura), 0, 665, 100, 0), 0, 100);
      if (posicionActual < 7) {
          digitalWrite(pinLedActuador, LOW);
      }
  }
}

void detenerTodo() {
  carroVelocidad = 0;
  empotActivo = false;
  engancheActivo = false;
  digitalWrite(pinActuador, LOW);
  tareaCompletada = true;
}

void enviarEstadoSensores() {
  static unsigned long ultimoEnvio = 0;
  
  // Condición corregida: se envía si pasa 1s O si se acaba de completar una tarea
  if (millis() - ultimoEnvio < 1000 && !tareaCompletada) return;
  ultimoEnvio = millis();
  
  double lectura = analogRead(pinLectura);
  double posicion = constrain(map(lectura, 0, 665, 100, 0), 0, 100);

  JsonDocument doc;
  doc["carro_cap"] = digitalRead(carroSensorCap);
  doc["empot1"] = digitalRead(sensorFinEmpot1);
  doc["empot2"] = digitalRead(sensorFinEmpot2);
  doc["enganche"] = digitalRead(sensorFinEnganche);
  doc["actuador"] = posicion;

  // Añadimos las variables de control al JSON
  doc["task_completed"] = tareaCompletada;
  doc["vagones"] = vagonesTrasladados;
  doc["autoclaves"] = autoclavesCompletos;

  serializeJson(doc, Serial);
  Serial.println();

  if (tareaCompletada) {
      tareaCompletada = false;
  }
}

void moverEnganchePasos(long vueltas, bool direccion) {
  engancheDireccion = direccion;
  enganchePasosRestantes = vueltas * stepsPerRevolution;
  engancheModoPasos = true;
  engancheActivo = true;
}