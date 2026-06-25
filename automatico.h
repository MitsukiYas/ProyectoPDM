#ifndef AUTOMATICO_H
#define AUTOMATICO_H

#include <Arduino.h>

// Estados del automático
enum EstadoAutomatico {
    AUTO_IDLE,
    AUTO_MOVIMIENTO,
    AUTO_DESPLEGAR_ESTABILIZADORES,
    AUTO_ARRASTRE_INICIAL,
    AUTO_ARRASTRE_VAGON,
    AUTO_RETRAER_ESTABILIZADORES,
    AUTO_SIGUIENTE_AUTOCLAVE,
    AUTO_FIN
};

enum EstadoArrastreInicial {
    AI_IDLE,
    AI_DESENROLLAR_INICIAL,
    AI_ESPERAR_OPERADOR,
    AI_ENROLLAR,
    AI_ESPERA_5S,
    AI_DESENROLLAR_FINAL,
    AI_ESPERAR_DESACOPLE,
    AI_FIN
};
enum EstadoArrastreVagon {
    AV_IDLE,
    AV_DESPLEGAR,
    AV_RETRAER,
    AV_VERIFICAR,
    AV_ESPERA_5S,
    AV_FIN
};


// Variables globales de maquina de estados
extern EstadoAutomatico estadoAuto;
extern EstadoArrastreInicial estadoAI;
extern EstadoArrastreVagon estadoAV;


// Funciones públicas
void iniciarAutomatico();
void detenerAutomatico();
void ejecutarSecuenciaCompleta();
void ejecutarArrastreInicial();
void ejecutarArrastreVagon();

#endif