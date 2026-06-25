#include <AccelStepper.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "variables.h"
#include "automatico.h"

String comando = "";

// Parámetros ajustables del sistema
const long pulsesPerRev = 800;       // Pulsos para una revolución de estabilizadores
int pulseDelay = 200;                // Microsegundos entre pulsos

const int stepsPerRevolution = 1600; // Pasos por revolución del enganche

// PINES Y VARIABLES DE CONTROL
const int carroStepPin = 3, carroDirPin = 2;
AccelStepper carroStepper(AccelStepper::DRIVER, carroStepPin, carroDirPin);
int carroVelocidad = 0;

// Estabilizadores (Empotramiento)
const int empotDirPin1 = 4, empotPulsePin1 = 5, empotDirPin2 = 6, empotPulsePin2 = 7;
bool empotActivo = false, empotDireccion = true;
long empotPasosRestantes = 0;

// Enganche Inicial
const int engancheDirPin = 8, engancheStepPin = 9;
bool engancheActivo = false, engancheDireccion = true, engancheModoPasos = false;
long enganchePasosRestantes = 0;

// Actuador Lineal
const int pinActuador = A0, pinLectura = A1;

//Vagones y autoclaves
int vagonesTrasladados = 0;
const int vagonesTotales = 3;
int autoclavesCompletos =0;
const int autoclavesTotales =2;


void setup() {
  Serial.begin(9600);

  // Configuración del Carro
  carroStepper.setMaxSpeed(3000);
  carroStepper.setMinPulseWidth(20);
  pinMode(carroSensorCap, INPUT);

  // Configuración de Estabilizadores
  pinMode(empotDirPin1, OUTPUT); pinMode(empotPulsePin1, OUTPUT);
  pinMode(empotDirPin2, OUTPUT); pinMode(empotPulsePin2, OUTPUT);
  pinMode(sensorFinEmpot1, INPUT); pinMode(sensorFinEmpot2, INPUT);

  // Configuración de Enganche y Actuador
  pinMode(engancheDirPin, OUTPUT); pinMode(engancheStepPin, OUTPUT);
  pinMode(sensorFinEnganche, INPUT);
  pinMode(pinActuador, OUTPUT); pinMode(pinLectura, INPUT);
}

void loop() {
  leerSerial();
  ejecutarCarro();
  ejecutarEmpot();
  ejecutarEnganche();
  enviarEstadoSensores();
  ejecutarSecuenciaCompleta(); // Definido en automatico.h
}

void leerSerial() {
  if (Serial.available()) {
    comando = Serial.readStringUntil('\n');
    comando.trim();

    // Control del Carro
    if (comando == "carro_forward") carroVelocidad = 2000;
    else if (comando == "carro_backward") carroVelocidad = -2000;
    else if (comando == "carro_stop") carroVelocidad = 0;
    
    // Control de Empotramiento
    else if (comando == "empot_forward") { empotActivo = true; empotDireccion = true; }
    else if (comando == "empot_backward") { empotActivo = true; empotDireccion = false; }
    else if (comando == "empot_stop") empotActivo = false;
    
    // Control de Enganches
    else if (comando == "enganche_enrollar") { engancheActivo = true; engancheDireccion = true; }
    else if (comando == "enganche_desenrollar") { engancheActivo = true; engancheDireccion = false; }
    else if (comando == "enganche_stop") engancheActivo = false;
    
    // Control del Actuador
    else if (comando == "act_extend") digitalWrite(pinActuador, HIGH);
    else if (comando == "act_retract") digitalWrite(pinActuador, LOW);
    else if (comando == "act_stop") {
      int posicionActual = analogRead(pinLectura);
      digitalWrite(pinActuador, LOW);
    }
    
    // Comandos Globales
    else if (comando == "start_all") iniciarAutomatico();
    else if (comando == "stop_all") detenerTodo();
  }
}

void ejecutarCarro() {
  carroStepper.setSpeed(carroVelocidad);
  carroStepper.runSpeed();
}

void ejecutarEmpot() {
  if (!empotActivo) return;

  digitalWrite(empotDirPin1, empotDireccion ? LOW : HIGH);
  digitalWrite(empotDirPin2, empotDireccion ? LOW : HIGH);

  static unsigned long ultimoPulso = 0;
  if (micros() - ultimoPulso >= empotPulseDelay) {
    ultimoPulso = micros();
    static bool estadoPulso = false;
    estadoPulso = !estadoPulso;
    digitalWrite(empotPulsePin1, estadoPulso ? HIGH : LOW);
    digitalWrite(empotPulsePin2, estadoPulso ? HIGH : LOW);
    
    // conteo de pasos
    if (estadoPulso) {
      empotPasosRestantes--;
      if (empotPasosRestantes <= 0) {
        empotActivo = false; // Se apaga automáticamente al terminar las vueltas
      }
    }
  }
}

void ejecutarEnganche() {
  if (!engancheActivo) return;

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
      }
    }
  }
}

void detenerTodo() {
  carroVelocidad = 0;
  empotActivo = false;
  engancheActivo = false;
  analogWrite(pinActuador, 0);
}

void enviarEstadoSensores() {
  static unsigned long ultimoEnvio = 0;
  if (millis() - ultimoEnvio < 1000) return;
  ultimoEnvio = millis();
  
  double posicion = constrain(map(analogRead(pinLectura), 0, 665, 100, 0), 0, 100);

  JsonDocument doc;
  doc["carro_cap"] = digitalRead(carroSensorCap);
  doc["empot1"] = digitalRead(sensorFinEmpot1);
  doc["empot2"] = digitalRead(sensorFinEmpot2);
  doc["enganche"] = digitalRead(sensorFinEnganche);
  doc["actuador"] = posicion;

  serializeJson(doc, Serial);
  Serial.println();
}

void moverEnganchePasos(long vueltas, bool direccion) {
  engancheDireccion = direccion;
  enganchePasosRestantes = vueltas*stepsPerRevolution;
  engancheModoPasos = true;
  engancheActivo = true;
}

void moverEmpotPasos(long vueltas, bool direccion) {
  empotDireccion = direccion;
  empotPasosRestantes = vueltas * pulsesPerRev;
  empotActivo = true;
}