#include "automatico.h"
#include "variables.h"
// Estado actual
EstadoAutomatico estadoAuto = AUTO_IDLE;
EstadoArrastreInicial estadoAI = AI_IDLE;
EstadoArrastreVagon estadoAV =AV_IDLE;

unsigned long tiempoMovimientoCarro = 0; 
bool empotRetractIniciado = false;
static int pulsosSensor = 0;

void iniciarAutomatico() {
    estadoAuto = AUTO_MOVIMIENTO;
    carroVelocidad = 0;
    empotActivo = false;
    engancheActivo = false;
}


void detenerAutomatico() {
    estadoAuto = AUTO_IDLE;
}

void ejecutarSecuenciaCompleta() {

    switch (estadoAuto) {
        case AUTO_IDLE:
          break;
        case AUTO_MOVIMIENTO:
            carroVelocidad = 2000;
            tiempoMovimientoCarro =millis();
            if (digitalRead(carroSensorCap)){
                if (millis()-tiempoMovimientoCarro>1000){//verificamos que haya avanzado algo al menos
                    carroVelocidad = 0;
                    estadoAuto = AUTO_DESPLEGAR_ESTABILIZADORES;
                }
            }

          break;
        case AUTO_DESPLEGAR_ESTABILIZADORES:
            if (!empotActivo && empotPasosRestantes <= 0) {
                empotDireccion = true; 
                empotPasosRestantes = 999999; //Alto para que sea un proceso pseudo infinito
                empotActivo = true;
            } 
            if (
                digitalRead(sensorFinEmpot1) &&
                digitalRead(sensorFinEmpot2)
            )
            {
                empotActivo = false;
                empotPasosRestantes = 0;
                estadoAI = AI_DESENROLLAR_INICIAL;
                estadoAuto = AUTO_ARRASTRE_INICIAL;
            }
            break;

        case AUTO_ARRASTRE_INICIAL:
            ejecutarArrastreInicial();         
            break;

        case AUTO_ARRASTRE_VAGON:
            // Inicializamos
            if (estadoAV == AV_IDLE) {
                vagonesTrasladados = 0;
                estadoAV = AV_DESPLEGAR;
            }
            ejecutarArrastreVagon();

            // Al terminar todos los vagones, avanzamos
            if (estadoAV == AV_FIN) {
                estadoAV = AV_IDLE;
                estadoAuto = AUTO_RETRAER_ESTABILIZADORES;
            }
            break;

        case AUTO_RETRAER_ESTABILIZADORES:
            //Gira 4 vueltas
            if (!empotRetractIniciado) {
                moverEmpotPasos(4, false); 
                empotRetractIniciado = true;
            }
            //verificamos que los fines de carrera ya no se presionen y hayan dado X pasos
            if (!empotActivo && digitalRead(sensorFinEmpot1) == LOW && digitalRead(sensorFinEmpot2) == LOW) {
                empotRetractIniciado = false;
                autoclavesCompletos++;
                estadoAuto = AUTO_SIGUIENTE_AUTOCLAVE;
            }
            break;

        case AUTO_SIGUIENTE_AUTOCLAVE:
            if (autoclavesCompletos>=autoclavesTotales){
                estadoAuto=AUTO_FIN; //Añadimos algo al final? maybe
                autoclavesCompletos =0;
            } else {
                estadoAuto=AUTO_MOVIMIENTO;
            }
            break;

        case AUTO_FIN:
            estadoAuto = AUTO_IDLE;
            break;
    }
}

void ejecutarArrastreInicial() {
    static unsigned long tiempoEspera = 0;
    static bool pasoIniciado = false;

    switch (estadoAI) {
        case AI_IDLE:
            break;

        case AI_DESENROLLAR_INICIAL: //Se desenrolla la cuerda para colocarla en el vagon
            if (!pasoIniciado){
            
                moverEnganchePasos(15,false);
                pasoIniciado=true;
            }
            if (!engancheActivo){
                pasoIniciado=false;
                estadoAI=AI_ESPERAR_OPERADOR;
            }
            
            break;

        case AI_ESPERAR_OPERADOR:
            // Espera comando de confirmación del operador por Serial de que el gancho ha sido colocado
            if (comando == "enganche_listo") {
                comando = ""; // Limpiar comando
                estadoAI = AI_ENROLLAR;
            }
            break;

        case AI_ENROLLAR:
            engancheDireccion = true;
            engancheActivo = true;
            if (digitalRead(sensorFinEnganche) || !engancheActivo) {
                tiempoEspera = millis(); // Captura tiempo para el delay de 5s
                estadoAI = AI_ESPERA_5S;
                engancheActivo = false;
            }
            break;

        case AI_ESPERA_5S:
            // Espera de 5 segundos no bloqueante
            if (millis() - tiempoEspera >= 5000) {
                estadoAI = AI_DESENROLLAR_FINAL;
            }
            break;

        case AI_DESENROLLAR_FINAL:
            if (!pasoIniciado) {
                moverEnganchePasos(4, false); 
                pasoIniciado = true;
            }
            if (!engancheActivo) {
                pasoIniciado = false;
                estadoAI = AI_ESPERAR_DESACOPLE;
            }
            break;

        case AI_ESPERAR_DESACOPLE:
            // Espera confirmación manual de fin de desacople
            if (comando == "enganche_desacoplado") {
                comando = "";
                estadoAI = AI_FIN;
            }
            break;

        case AI_FIN:
            estadoAI = AI_IDLE;
            break;
    }
}

void ejecutarArrastreVagon() {
    static unsigned long tiempoEsperaVagon = 0;
    double posicion_actuador = constrain(map(analogRead(pinLectura), 0, 665, 100, 0), 0, 100);

    switch (estadoAV) {
        case AV_IDLE:
            pulsosSensor = 0;
            break;

        case AV_DESPLEGAR:
            digitalWrite(pinActuador, HIGH); // Desplegar completo
            
            if (posicion_actuador >= 100) { //llego al final? (Alejandro del futuro: si no funciona lo bajamos a 98)
                estadoAV = AV_RETRAER;
            }
            break;

        case AV_RETRAER:
            digitalWrite(pinActuador, LOW); // Retroceder
            
            
            if (posicion_actuador <= 0) { //Regreso por completo (aqui igual xdxd)
                estadoAV = AV_VERIFICAR;
            }
            break;

        case AV_VERIFICAR:
            static bool ultimoEstadoSensor = LOW;
            bool estadoActualSensor = digitalRead(sensorFinEnganche);

            // Fin de carrera cuenta los vagonesss
            if (estadoActualSensor == HIGH && ultimoEstadoSensor == LOW) { 
                pulsosSensor++;

                if (pulsosSensor >= 2) {
                    pulsosSensor = 0;
                    vagonesTrasladados++; 
                    
                    if (vagonesTrasladados >= vagonesTotales) {
                        tiempoEsperaVagon = millis(); 
                        estadoAV = AV_ESPERA_5S;
                        ultimoEstadoSensor = estadoActualSensor; // Actualizar antes de salir
                        break; 
                    }
                }
                estadoAV = AV_DESPLEGAR;
            } 
            // Si sigue en HIGH pero ya lo contamos, o si es LOW, no hacemos nada y seguimos esperando o empujando
            else if (estadoActualSensor == LOW && ultimoEstadoSensor == HIGH) {
                // El sensor se liberó, podemos prepararnos para la siguiente lectura
                estadoAV = AV_DESPLEGAR;
            }
            
            ultimoEstadoSensor = estadoActualSensor; // Guardar estado para el próximo ciclo
            break;

        case AV_ESPERA_5S:
            if (millis() - tiempoEsperaVagon >= 5000) {
                estadoAV = AV_FIN;
            }
            break;

        case AV_FIN:
            // Mantiene el actuador bloqueado
            digitalWrite(pinActuador, LOW);
            estadoAV=AV_IDLE;
            break;
    }
}