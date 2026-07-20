#include "automatico.h"
#include "variables.h"

EstadoAutomatico estadoAuto = AUTO_IDLE;
EstadoAutomatico siguienteEstado = AUTO_IDLE; // A dónde ir después de la pausa
unsigned long tiempoInicioEstado = 0;
const unsigned long DURACION_PRUEBA = 5000; // 5 segundos para actuador
const unsigned long DURACION_PAUSA = 2000;  // 2 segundos de pausa

void iniciarAutomatico() {
    estadoAuto = AUTO_CARRO;
    tiempoInicioEstado = millis();

    // Encender carro
    carroVelocidad = 2000;
    digitalWrite(pinLedCarro, HIGH);
}

void detenerAutomatico() {
    carroVelocidad = 0;
    empotActivo = false;
    engancheActivo = false;
    digitalWrite(pinActuador, LOW);
    digitalWrite(pinLedCarro, LOW);
    digitalWrite(pinLedEmpotramiento, LOW);
    digitalWrite(pinLedActuador, LOW);
    digitalWrite(pinLedPolea, LOW);
    estadoAuto = AUTO_IDLE;
}

void ejecutarSecuenciaCompleta() {
    switch (estadoAuto) {
        case AUTO_IDLE:
            break;

        case AUTO_CARRO:
            if (millis() - tiempoInicioEstado >= 1000) {
                // Apagar carro
                carroVelocidad = 0;
                digitalWrite(pinLedCarro, LOW);

                // Ir a pausa, después estabilizadores
                siguienteEstado = AUTO_ESTABILIZADORES;
                tiempoInicioEstado = millis();
                estadoAuto = AUTO_PAUSA;
            }
            break;

        case AUTO_PAUSA:
            if (millis() - tiempoInicioEstado >= DURACION_PAUSA) {
                tiempoInicioEstado = millis();
                estadoAuto = siguienteEstado;

                // Encender lo que toque según el siguiente estado
                if (siguienteEstado == AUTO_ESTABILIZADORES) {
                    empotDireccion = true;
                    empotActivo = true;
                    digitalWrite(pinLedEmpotramiento, HIGH);
                }
                else if (siguienteEstado == AUTO_RETRAER_ESTABILIZADORES) {
                    empotDireccion = false;
                    empotActivo = true;
                    digitalWrite(pinLedEmpotramiento, HIGH);
                }
                else if (siguienteEstado == AUTO_ACTUADOR) {
                    digitalWrite(pinActuador, HIGH);
                    digitalWrite(pinLedActuador, HIGH);
                }
                else if (siguienteEstado == AUTO_POLEA) {
                    engancheDireccion = true;
                    engancheActivo = true;
                    engancheModoPasos = false;
                    digitalWrite(pinLedPolea, HIGH);
                }
            }
            break;

        case AUTO_ESTABILIZADORES:
            if (millis() - tiempoInicioEstado >= 1000) {
                // Apagar estabilizadores (antes de pausa)
                empotActivo = false;
                digitalWrite(pinLedEmpotramiento, LOW);

                // Ir a pausa, después retraer
                siguienteEstado = AUTO_RETRAER_ESTABILIZADORES;
                tiempoInicioEstado = millis();
                estadoAuto = AUTO_PAUSA;
            }
            break;

        case AUTO_RETRAER_ESTABILIZADORES:
            if (millis() - tiempoInicioEstado >= 1000) {
                // Apagar estabilizadores
                empotActivo = false;
                digitalWrite(pinLedEmpotramiento, LOW);

                // Ir a pausa, después actuador
                siguienteEstado = AUTO_ACTUADOR;
                tiempoInicioEstado = millis();
                estadoAuto = AUTO_PAUSA;
            }
            break;

        case AUTO_ACTUADOR:
            if (millis() - tiempoInicioEstado >= DURACION_PRUEBA) {
                // Apagar actuador
                digitalWrite(pinActuador, LOW);
                digitalWrite(pinLedActuador, LOW);

                // Ir a pausa, después polea
                siguienteEstado = AUTO_POLEA;
                tiempoInicioEstado = millis();
                estadoAuto = AUTO_PAUSA;
            }
            break;

        case AUTO_POLEA:
            if (millis() - tiempoInicioEstado >= 1000) {
                // Apagar polea
                engancheActivo = false;
                digitalWrite(pinLedPolea, LOW);

                // Iniciar ciclo repetitivo del actuador
                digitalWrite(pinActuador, HIGH);
                digitalWrite(pinLedActuador, HIGH);
                tiempoInicioEstado = millis();
                estadoAuto = AUTO_ACTUADOR_ON;
            }
            break;

        case AUTO_ACTUADOR_ON:
            if (millis() - tiempoInicioEstado >= 10000) {
                // Apagar actuador por 8 segundos
                digitalWrite(pinActuador, LOW);
                digitalWrite(pinLedActuador, LOW);
                tiempoInicioEstado = millis();
                estadoAuto = AUTO_ACTUADOR_OFF;
            }
            break;

        case AUTO_ACTUADOR_OFF:
            if (millis() - tiempoInicioEstado >= 10000) {
                // Encender actuador por 5 segundos
                digitalWrite(pinActuador, HIGH);
                digitalWrite(pinLedActuador, HIGH);
                tiempoInicioEstado = millis();
                estadoAuto = AUTO_ACTUADOR_ON;
            }
            break;

        case AUTO_FIN:
            estadoAuto = AUTO_IDLE;
            break;
    }
}