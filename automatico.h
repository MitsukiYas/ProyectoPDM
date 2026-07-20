#ifndef AUTOMATICO_H
#define AUTOMATICO_H

#include <Arduino.h>

enum EstadoAutomatico {
    AUTO_IDLE,
    AUTO_CARRO,
    AUTO_PAUSA,
    AUTO_ESTABILIZADORES,
    AUTO_RETRAER_ESTABILIZADORES,
    AUTO_ACTUADOR,
    AUTO_POLEA,
    AUTO_ACTUADOR_ON,
    AUTO_ACTUADOR_OFF,
    AUTO_FIN
};

extern EstadoAutomatico estadoAuto;

void iniciarAutomatico();
void detenerAutomatico();
void ejecutarSecuenciaCompleta();

#endif