#include <AccelStepper.h>
#include <ArduinoJson.h>
#include "variables.h"

// =====================================================
// PINES Y VARIABLES DE CONTROL
// =====================================================

// --- Carro (Traslación) ---
const int carroDirPin  = 2;
const int carroStepPin = 3;
const int pinLedCarro = A5;
const int pinLedEmpotramiento = A4;
AccelStepper carroStepper(AccelStepper::DRIVER, carroStepPin, carroDirPin);

// --- Empotramiento (Estabilización) ---
const int empotDirPin1   = 8;
const int empotPulsePin1 = 9;
const int empotDirPin2   = 6;
const int empotPulsePin2 = 7;
const int empotPulseDelay = 500; // Ajusta este valor para cambiar la velocidad de empotramiento (menor = más rápido)

// --- Polea (Enganche) ---
const int poleaDirPin  = 4;
const int poleaStepPin = 5;
const int pinLedPolea = A3;
const int poleaPulseDelay = 1000; // Ajusta este valor para cambiar la velocidad de la polea

// --- Actuador Lineal ---
const int pinActuador = A0;
const int pinLedActuador = A2;

// --- Sensores ---
const int pinSensorInductivo = 10;
const int pinFC_Polea = 11;
const int pinFC_Empot1 = 12;
const int pinFC_Empot2 = 13;

// =====================================================
// ESTADOS DE LA SECUENCIA
// =====================================================
enum EstadoSecuencia {
  ESTADO_TRASLACION,
  ESTADO_EMPOT_EXTENDIENDO,
  ESTADO_EMPOT_ESPERA,
  ESTADO_POLEA_EXTENDIENDO,
  ESTADO_POLEA_ESPERA_1,
  ESTADO_POLEA_RETRAYENDO_HASTA_FC,
  ESTADO_POLEA_ESPERA_2,
  ESTADO_POLEA_RETRAYENDO_EXTRA,
  ESTADO_ACTUADOR_EXTENDIENDO,
  ESTADO_ACTUADOR_RETRAYENDO,
  ESTADO_EMPOT_RETRAYENDO,
  ESTADO_FIN,
  ESTADO_MANUAL_ACTUADOR,
  ESTADO_MANUAL_EMPOT,
  ESTADO_MANUAL_LIBRE
};

EstadoSecuencia estadoActual = ESTADO_FIN; // Inicia en reposo

// Variables del Modo Manual
enum ComandoManual { MANUAL_PARADO, MANUAL_AVANZAR, MANUAL_RETROCEDER };
ComandoManual cmdTraslacion = MANUAL_PARADO;
ComandoManual cmdEmpotramiento = MANUAL_PARADO;
ComandoManual cmdPolea = MANUAL_PARADO;
ComandoManual cmdActuador = MANUAL_PARADO;

// Variables de control de los motores de empotramiento
bool empot1Listo = false;
bool empot2Listo = false;
unsigned long tiempoInicioRetraccion = 0;
unsigned long ultimoEnvioTelemetria = 0;
int ciclosActualesArrastre = 0;

// Variables de Modo Automático Infinito
bool modoAutomaticoActivo = false;
bool direccionTraslacionAdelante = true;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(10); // Evitar bloqueos al leer Serial

  // Configurar sensores (Entradas)
  pinMode(pinSensorInductivo, INPUT_PULLUP);
  pinMode(pinFC_Polea, INPUT_PULLUP);
  pinMode(pinFC_Empot1, INPUT_PULLUP);
  pinMode(pinFC_Empot2, INPUT_PULLUP);

  // Configurar pines de empotramiento (Salidas)
  pinMode(empotDirPin1, OUTPUT);
  pinMode(empotPulsePin1, OUTPUT);
  pinMode(empotDirPin2, OUTPUT);
  pinMode(empotPulsePin2, OUTPUT);

  // Configurar pines de polea (Salidas)
  pinMode(poleaDirPin, OUTPUT);
  pinMode(poleaStepPin, OUTPUT);

  // Configurar actuador (Salida)
  pinMode(pinActuador, OUTPUT);

  // Configurar LED (Salida) y asegurar que inician apagados
  pinMode(pinLedCarro, OUTPUT);
  digitalWrite(pinLedCarro, LOW);
  
  pinMode(pinLedEmpotramiento, OUTPUT);
  digitalWrite(pinLedEmpotramiento, LOW);
  
  pinMode(pinLedPolea, OUTPUT);
  digitalWrite(pinLedPolea, LOW);
  
  pinMode(pinLedActuador, OUTPUT);
  digitalWrite(pinLedActuador, LOW);

  // Configuración del motor del carro
  carroStepper.setMaxSpeed(3000);   
  carroStepper.setSpeed(1000);      

  delay(3000);
  digitalWrite(pinLedCarro, HIGH);
}

void loop() {
  if (millis() - ultimoEnvioTelemetria >= 100) {
    ultimoEnvioTelemetria = millis();
    enviarTelemetriaESP32();
  }

  // Comprobar si hay comandos entrantes
  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    
    // Imprimir para depuración (la ESP32 ignorará este texto porque no empieza con '{')
    Serial.print("Arduino recibio: ");
    Serial.println(comando);
    
    // Crear documento JSON (tamaño de 200 bytes suele ser suficiente para tramas pequeñas)
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, comando);

    if (error) {
      Serial.print("Error de parseo JSON: ");
      Serial.println(error.c_str());
    } else {
      const char* tipo = doc["tipo"];
      
      if (tipo != nullptr) {
        if (strcmp(tipo, "sistema") == 0) {
          const char* accion = doc["accion"];
          if (accion != nullptr && strcmp(accion, "estado_inicial") == 0) {
            modoAutomaticoActivo = false; // Apagar ciclo automático
            
            // Apagar todos los LEDs de secuencia
            digitalWrite(pinLedCarro, LOW);
            digitalWrite(pinLedEmpotramiento, LOW);
            digitalWrite(pinLedPolea, LOW);
            digitalWrite(pinLedActuador, LOW);
  
            // Iniciar la retracción del actuador lineal
            digitalWrite(pinActuador, LOW); 
            
            // Pasar al estado manual
            estadoActual = ESTADO_MANUAL_ACTUADOR;
          }
        } 
        else if (strcmp(tipo, "manual") == 0) {
          const char* motor = doc["motor"];
          const char* accion = doc["accion"];
          
          if (motor != nullptr && accion != nullptr) {
            
            // Apagamos modo automático e ingresamos al estado libre
            modoAutomaticoActivo = false;
            estadoActual = ESTADO_MANUAL_LIBRE;
            
            if (strcmp(motor, "traslacion") == 0) {
              if (strcmp(accion, "avanzar") == 0) cmdTraslacion = MANUAL_AVANZAR;
              else if (strcmp(accion, "retroceder") == 0) cmdTraslacion = MANUAL_RETROCEDER;
              else cmdTraslacion = MANUAL_PARADO;
            } 
            else if (strcmp(motor, "empotramiento") == 0) {
              if (strcmp(accion, "extender") == 0) cmdEmpotramiento = MANUAL_AVANZAR;
              else if (strcmp(accion, "retraer") == 0) cmdEmpotramiento = MANUAL_RETROCEDER;
              else cmdEmpotramiento = MANUAL_PARADO;
            } 
            else if (strcmp(motor, "polea") == 0) {
              if (strcmp(accion, "extender") == 0) cmdPolea = MANUAL_AVANZAR;
              else if (strcmp(accion, "retraer") == 0) cmdPolea = MANUAL_RETROCEDER;
              else cmdPolea = MANUAL_PARADO;
            } 
            else if (strcmp(motor, "actuador") == 0) {
              if (strcmp(accion, "extender") == 0) cmdActuador = MANUAL_AVANZAR;
              else if (strcmp(accion, "retraer") == 0) cmdActuador = MANUAL_RETROCEDER;
              // El actuador no tiene botón de Parar
            }
          }
        } 
        else if (strcmp(tipo, "automatico") == 0) {
          const char* accion = doc["accion"];
          if (accion != nullptr && strcmp(accion, "iniciar") == 0) {
            modoAutomaticoActivo = true;
            direccionTraslacionAdelante = true; // Reiniciar siempre hacia adelante al activar
            
            digitalWrite(pinLedEmpotramiento, LOW);
            digitalWrite(pinLedPolea, LOW);
            digitalWrite(pinLedActuador, LOW);
            
            estadoActual = ESTADO_TRASLACION;
          }
        }
      }
    }
  }

  switch (estadoActual) {

    // ----------------------------------------------------
    // ESTADO 1: TRASLACIÓN
    // ----------------------------------------------------
    case ESTADO_TRASLACION: {
      int estadoInductivo = digitalRead(pinSensorInductivo);
      
      if (estadoInductivo == LOW) { // NO ha detectado metal
        if (direccionTraslacionAdelante) {
          carroStepper.setSpeed(1000);
        } else {
          carroStepper.setSpeed(-1000); // Gira en sentido contrario
        }
        carroStepper.runSpeed(); 
      } else { // HIGH = Metal detectado
        digitalWrite(pinLedCarro, LOW);
        
        // Configuramos la dirección para EXTENDER (Ajusta LOW/HIGH según tu cableado)
        digitalWrite(empotDirPin1, LOW); 
        digitalWrite(empotDirPin2, LOW);
        
        digitalWrite(pinLedEmpotramiento, HIGH); // Enciende LED piloto de estabilizadores
        
        empot1Listo = false;
        empot2Listo = false;
        estadoActual = ESTADO_EMPOT_EXTENDIENDO;
      }
      break;
    }

    // ----------------------------------------------------
    // ESTADO 2: EXTENDER ESTABILIZADORES
    // ----------------------------------------------------
    case ESTADO_EMPOT_EXTENDIENDO: {
      // Leemos si los fines de carrera fueron presionados (asumimos que LOW es presionado)
      if (digitalRead(pinFC_Empot1) == LOW && !empot1Listo) {
        empot1Listo = true;
      }
      if (digitalRead(pinFC_Empot2) == LOW && !empot2Listo) {
        empot2Listo = true;
      }

      // Generación de pulsos manual para los estabilizadores
      static unsigned long ultimoPulso = 0;
      if (micros() - ultimoPulso >= empotPulseDelay) {
        ultimoPulso = micros();
        static bool estadoPulso = false;
        estadoPulso = !estadoPulso;

        // Sólo mandamos pulsos al motor que NO haya llegado a su fin de carrera
        // (Esto asegura que se detengan independientemente)
        if (!empot1Listo) digitalWrite(empotPulsePin1, estadoPulso);
        if (!empot2Listo) digitalWrite(empotPulsePin2, estadoPulso);
      }

      // Si ambos ya tocaron su fin de carrera, pasamos al siguiente estado
      if (empot1Listo && empot2Listo) {
        
        digitalWrite(pinLedEmpotramiento, LOW); // Apagamos el LED porque ya no están en movimiento
        
        // Guardamos el tiempo actual para contar los 3 segundos de espera
        tiempoInicioRetraccion = millis(); 
        estadoActual = ESTADO_EMPOT_ESPERA;
      }
      break;
    }

    // ----------------------------------------------------
    // ESTADO 2.5: ESPERA ANTES DE LA POLEA
    // ----------------------------------------------------
    case ESTADO_EMPOT_ESPERA: {
      if (millis() - tiempoInicioRetraccion >= TIEMPO_ESPERA_EMPOT) {
        
        digitalWrite(pinLedPolea, HIGH); 
        digitalWrite(poleaDirPin, LOW); // Corrección: LOW es extensión
        
        tiempoInicioRetraccion = millis();
        estadoActual = ESTADO_POLEA_EXTENDIENDO;
      }
      break;
    }

    // ----------------------------------------------------
    // ESTADO 3: POLEA EXTENDIENDO
    // ----------------------------------------------------
    case ESTADO_POLEA_EXTENDIENDO: {
      if (millis() - tiempoInicioRetraccion <= TIEMPO_POLEA_EXTENDIENDO) {
        static unsigned long ultimoPulsoP = 0;
        if (micros() - ultimoPulsoP >= poleaPulseDelay) {
          ultimoPulsoP = micros();
          static bool estadoPulsoP = false;
          estadoPulsoP = !estadoPulsoP;
          digitalWrite(poleaStepPin, estadoPulsoP);
        }
      } else {
        digitalWrite(pinLedPolea, LOW); 
        
        tiempoInicioRetraccion = millis();
        estadoActual = ESTADO_POLEA_ESPERA_1;
      }
      break;
    }

    // ----------------------------------------------------
    // ESTADO 3.1: POLEA ESPERA 1
    // ----------------------------------------------------
    case ESTADO_POLEA_ESPERA_1: {
      if (millis() - tiempoInicioRetraccion >= TIEMPO_POLEA_ESPERA_1) {
        
        digitalWrite(pinLedPolea, HIGH);
        digitalWrite(poleaDirPin, HIGH); // Corrección: HIGH es retracción
        
        estadoActual = ESTADO_POLEA_RETRAYENDO_HASTA_FC;
      }
      break;
    }

    // ----------------------------------------------------
    // ESTADO 3.2: POLEA RETRAYENDO HASTA FC
    // ----------------------------------------------------
    case ESTADO_POLEA_RETRAYENDO_HASTA_FC: {
      // Asumimos que LOW es presionado (con INPUT_PULLUP)
      if (digitalRead(pinFC_Polea) == HIGH) { // Si está libre, sigue moviendo
        static unsigned long ultimoPulsoP2 = 0;
        if (micros() - ultimoPulsoP2 >= poleaPulseDelay) {
          ultimoPulsoP2 = micros();
          static bool estadoPulsoP2 = false;
          estadoPulsoP2 = !estadoPulsoP2;
          digitalWrite(poleaStepPin, estadoPulsoP2);
        }
      } else { // Fue presionado (LOW)
        digitalWrite(pinLedPolea, LOW);
        
        tiempoInicioRetraccion = millis();
        estadoActual = ESTADO_POLEA_ESPERA_2;
      }
      break;
    }

    // ----------------------------------------------------
    // ESTADO 3.3: POLEA ESPERA 2
    // ----------------------------------------------------
    case ESTADO_POLEA_ESPERA_2: {
      if (millis() - tiempoInicioRetraccion >= TIEMPO_POLEA_ESPERA_2) {
        
        digitalWrite(pinLedPolea, HIGH);
        digitalWrite(poleaDirPin, HIGH); // Mantiene retracción (HIGH)
        
        tiempoInicioRetraccion = millis();
        estadoActual = ESTADO_POLEA_RETRAYENDO_EXTRA;
      }
      break;
    }

    // ----------------------------------------------------
    // ESTADO 3.4: POLEA RETRAYENDO EXTRA
    // ----------------------------------------------------
    case ESTADO_POLEA_RETRAYENDO_EXTRA: {
      if (millis() - tiempoInicioRetraccion <= TIEMPO_POLEA_RETRAYENDO_EXTRA) {
        static unsigned long ultimoPulsoP3 = 0;
        if (micros() - ultimoPulsoP3 >= poleaPulseDelay) {
          ultimoPulsoP3 = micros();
          static bool estadoPulsoP3 = false;
          estadoPulsoP3 = !estadoPulsoP3;
          digitalWrite(poleaStepPin, estadoPulsoP3);
        }
      } else {
        digitalWrite(pinLedPolea, LOW);
        
        digitalWrite(pinActuador, HIGH); // Activa actuador al máximo
        digitalWrite(pinLedActuador, HIGH); // Enciende LED del actuador
        
        tiempoInicioRetraccion = millis();
        ciclosActualesArrastre = 0; // Inicializar contador de ciclos
        estadoActual = ESTADO_ACTUADOR_EXTENDIENDO;
      }
      break;
    }

    // ----------------------------------------------------
    // ESTADO 5: ACTUADOR EXTENDIENDO
    // ----------------------------------------------------
    case ESTADO_ACTUADOR_EXTENDIENDO: {
      if (analogRead(A1) <= UMBRAL_ACTUADOR_EXTENDIDO) { // Cuando alcance su extensión máxima (valor bajo)
        
        digitalWrite(pinActuador, LOW); // Retorna a su base
        // El LED sigue encendido porque sigue en movimiento
        
        tiempoInicioRetraccion = millis();
        estadoActual = ESTADO_ACTUADOR_RETRAYENDO;
      }
      break;
    }

    // ----------------------------------------------------
    // ESTADO 6: ACTUADOR RETRAYENDO
    // ----------------------------------------------------
    case ESTADO_ACTUADOR_RETRAYENDO: {
      if (analogRead(A1) >= UMBRAL_ACTUADOR_RETRAIDO) { // Cuando alcance su retracción máxima (valor alto)
        
        ciclosActualesArrastre++; // Incrementamos el contador de ciclos completados
        
        if (ciclosActualesArrastre < CICLOS_ARRASTRE) {
          // Aún no terminamos, repetimos extensión
          digitalWrite(pinActuador, HIGH); // Vuelve a extender
          estadoActual = ESTADO_ACTUADOR_EXTENDIENDO;
        } else {
          // Ya se completaron los ciclos
          digitalWrite(pinLedActuador, LOW); // Apaga LED del actuador
          
          // Encendemos el LED de estabilización y configuramos la dirección para retraer
          digitalWrite(pinLedEmpotramiento, HIGH);
          digitalWrite(empotDirPin1, HIGH);
          digitalWrite(empotDirPin2, HIGH);
          
          tiempoInicioRetraccion = millis();
          estadoActual = ESTADO_EMPOT_RETRAYENDO;
        }
      }
      break;
    }

    // ----------------------------------------------------
    // ESTADO 7: RETRAER ESTABILIZADORES
    // ----------------------------------------------------
    case ESTADO_EMPOT_RETRAYENDO: {
      // Comprobamos si ya pasaron los milisegundos definidos
      if (millis() - tiempoInicioRetraccion <= TIEMPO_EMPOT_RETRAYENDO) {
        // Continuamos mandando pulsos a ambos motores para que retrocedan
        static unsigned long ultimoPulsoRetraccion = 0;
        if (micros() - ultimoPulsoRetraccion >= empotPulseDelay) {
          ultimoPulsoRetraccion = micros();
          static bool estadoPulsoR = false;
          estadoPulsoR = !estadoPulsoR;
          
          digitalWrite(empotPulsePin1, estadoPulsoR);
          digitalWrite(empotPulsePin2, estadoPulsoR);
        }
      } else {
        
        digitalWrite(pinLedEmpotramiento, LOW); // Apaga LED piloto de estabilizadores
        
        if (modoAutomaticoActivo) {
          // Si el modo infinito está activo, alternar dirección y reiniciar ciclo
          direccionTraslacionAdelante = !direccionTraslacionAdelante;
          estadoActual = ESTADO_TRASLACION;
        } else {
          estadoActual = ESTADO_FIN;
        }
      }
      break;
    }

    // ----------------------------------------------------
    // ESTADO 8: FIN
    // ----------------------------------------------------
    case ESTADO_FIN: {
      // Secuencia terminada, no hacemos nada más
      break;
    }

    // ----------------------------------------------------
    // ESTADO MODO MANUAL: ACTUADOR RETRAYENDO
    // ----------------------------------------------------
    case ESTADO_MANUAL_ACTUADOR: {
      if (analogRead(A1) >= UMBRAL_ACTUADOR_RETRAIDO) {
        // Terminó de retraerse el actuador lineal. 
        // Encendemos LED de empotramiento como piloto y comenzamos a retraer estabilizadores
        digitalWrite(pinLedEmpotramiento, HIGH);
        digitalWrite(empotDirPin1, HIGH);
        digitalWrite(empotDirPin2, HIGH);
        
        tiempoInicioRetraccion = millis();
        estadoActual = ESTADO_MANUAL_EMPOT;
      }
      break;
    }

    // ----------------------------------------------------
    // ESTADO MODO MANUAL: EMPOTRAMIENTO RETRAYENDO
    // ----------------------------------------------------
    case ESTADO_MANUAL_EMPOT: {
      if (millis() - tiempoInicioRetraccion <= TIEMPO_EMPOT_RETRAYENDO) {
        // Mandamos pulsos a ambos motores para que retrocedan
        static unsigned long ultimoPulsoManual = 0;
        if (micros() - ultimoPulsoManual >= empotPulseDelay) {
          ultimoPulsoManual = micros();
          static bool estadoPulsoM = false;
          estadoPulsoM = !estadoPulsoM;
          
          digitalWrite(empotPulsePin1, estadoPulsoM);
          digitalWrite(empotPulsePin2, estadoPulsoM);
        }
      } else {
        digitalWrite(pinLedEmpotramiento, LOW); // Apaga LED piloto
        estadoActual = ESTADO_FIN; // Queda en reposo
      }
      break;
    }

    // ----------------------------------------------------
    // ESTADO MODO MANUAL LIBRE
    // ----------------------------------------------------
    case ESTADO_MANUAL_LIBRE: {
      // --- Traslación ---
      // Si quiere avanzar pero detecta metal (HIGH), paramos. Si no, avanza libre. 
      // Retroceder siempre es libre (asumiendo que se aleja del metal).
      if (cmdTraslacion == MANUAL_AVANZAR) {
        digitalWrite(pinLedCarro, HIGH); // Enciende LED
        if (digitalRead(pinSensorInductivo) == HIGH) { // Corrección: HIGH es metal detectado
          cmdTraslacion = MANUAL_PARADO; // Tope alcanzado
          carroStepper.setSpeed(0);
        } else {
          carroStepper.setSpeed(1000);
        }
        carroStepper.runSpeed();
      } else if (cmdTraslacion == MANUAL_RETROCEDER) {
        digitalWrite(pinLedCarro, HIGH); // Enciende LED
        carroStepper.setSpeed(-1000);
        carroStepper.runSpeed();
      } else {
        digitalWrite(pinLedCarro, LOW); // Apaga LED
        carroStepper.setSpeed(0);
        carroStepper.runSpeed();
      }

      // --- Estabilizadores (Empotramiento) ---
      if (cmdEmpotramiento != MANUAL_PARADO) {
        digitalWrite(pinLedEmpotramiento, HIGH); // Enciende LED
        bool puedeMoverse = true;
        
        if (cmdEmpotramiento == MANUAL_AVANZAR) {
          // Asumiendo HIGH = sensor NO presionado. Si alguno se presiona, paramos el avance de los estabilizadores.
          if (digitalRead(pinFC_Empot1) == LOW || digitalRead(pinFC_Empot2) == LOW) {
            cmdEmpotramiento = MANUAL_PARADO;
            puedeMoverse = false;
          }
        }
        
        if (puedeMoverse) {
          digitalWrite(empotDirPin1, cmdEmpotramiento == MANUAL_AVANZAR ? LOW : HIGH);
          digitalWrite(empotDirPin2, cmdEmpotramiento == MANUAL_AVANZAR ? LOW : HIGH);
          
          static unsigned long ultimoPulsoE = 0;
          if (micros() - ultimoPulsoE >= empotPulseDelay) {
            ultimoPulsoE = micros();
            static bool estadoPulsoE = false;
            estadoPulsoE = !estadoPulsoE;
            digitalWrite(empotPulsePin1, estadoPulsoE);
            digitalWrite(empotPulsePin2, estadoPulsoE);
          }
        }
      } else {
        digitalWrite(pinLedEmpotramiento, LOW); // Apaga LED si está parado
      }

      // --- Polea ---
      if (cmdPolea != MANUAL_PARADO) {
        digitalWrite(pinLedPolea, HIGH); // Enciende LED
        bool puedeMoverseP = true;
        
        if (cmdPolea == MANUAL_RETROCEDER) { // Retraer hacia el fin de carrera
          if (digitalRead(pinFC_Polea) == LOW) {
            cmdPolea = MANUAL_PARADO;
            puedeMoverseP = false;
          }
        }
        
        if (puedeMoverseP) {
          digitalWrite(poleaDirPin, cmdPolea == MANUAL_AVANZAR ? LOW : HIGH); // Invertido
          
          static unsigned long ultimoPulsoP = 0;
          if (micros() - ultimoPulsoP >= poleaPulseDelay) {
            ultimoPulsoP = micros();
            static bool estadoPulsoP = false;
            estadoPulsoP = !estadoPulsoP;
            digitalWrite(poleaStepPin, estadoPulsoP);
          }
        }
      } else {
        digitalWrite(pinLedPolea, LOW); // Apaga LED si está parado
      }

      // --- Actuador Lineal ---
      if (cmdActuador == MANUAL_AVANZAR) {
        digitalWrite(pinLedActuador, HIGH); // Enciende LED
        if (analogRead(A1) <= UMBRAL_ACTUADOR_EXTENDIDO) {
          cmdActuador = MANUAL_PARADO; // Ya está extendido
        } else {
          digitalWrite(pinActuador, HIGH);
        }
      } else if (cmdActuador == MANUAL_RETROCEDER) {
        digitalWrite(pinLedActuador, HIGH); // Enciende LED
        if (analogRead(A1) >= UMBRAL_ACTUADOR_RETRAIDO) {
          cmdActuador = MANUAL_PARADO; // Ya está retraído
        } else {
          digitalWrite(pinActuador, LOW);
        }
      } else {
        digitalWrite(pinLedActuador, LOW); // Apaga LED si está parado
      }
      
      break;
    }
  }
}

void enviarTelemetriaESP32() {
  String json = "{";
  json += "\"est\":" + String((int)estadoActual) + ",";
  // Sensores: Asumimos INPUT_PULLUP.
  // Para el inductivo: HIGH = Detecta Metal (Encendido), LOW = No Detecta (Apagado)
  json += "\"ind\":" + String(digitalRead(pinSensorInductivo) == HIGH ? 1 : 0) + ",";
  json += "\"fc1\":" + String(digitalRead(pinFC_Empot1) == LOW ? 1 : 0) + ",";
  json += "\"fc2\":" + String(digitalRead(pinFC_Empot2) == LOW ? 1 : 0) + ",";
  json += "\"fcp\":" + String(digitalRead(pinFC_Polea) == LOW ? 1 : 0) + ",";
  // LEDs (salidas lógicas):
  json += "\"l_car\":" + String(digitalRead(pinLedCarro)) + ",";
  json += "\"l_emp\":" + String(digitalRead(pinLedEmpotramiento)) + ",";
  json += "\"l_pol\":" + String(digitalRead(pinLedPolea)) + ",";
  json += "\"l_act\":" + String(digitalRead(pinLedActuador)) + ",";
  // Feedback analógico del actuador:
  json += "\"a1\":" + String(analogRead(A1));
  json += "}";
  Serial.println(json);
}