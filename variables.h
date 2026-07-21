#ifndef VARIABLES_H
#define VARIABLES_H

extern String comando;

extern int carroVelocidad;

extern bool empotActivo;
extern bool empotDireccion;

extern bool engancheActivo;
extern bool engancheDireccion;
extern bool engancheModoPasos;
extern long enganchePasosRestantes;

extern const int pinLectura;
extern const int pinActuador;

extern const int pinLedActuador;
extern const int pinLedPolea;
extern const int pinLedEmpotramiento;
extern const int pinLedCarro;

void moverEnganchePasos(long vueltas, bool direccion);

// --- Tiempos de Secuencia (milisegundos) ---
const unsigned long TIEMPO_ESPERA_EMPOT = 3000;
const unsigned long TIEMPO_POLEA_EXTENDIENDO = 10000;
const unsigned long TIEMPO_POLEA_ESPERA_1 = 3000;
const unsigned long TIEMPO_POLEA_ESPERA_2 = 3000;
const unsigned long TIEMPO_POLEA_RETRAYENDO_EXTRA = 2000;
const int UMBRAL_ACTUADOR_EXTENDIDO = 20; // Valor cercano a 10 (máx. extensión)
const int UMBRAL_ACTUADOR_RETRAIDO = 615; // Valor cercano a 620 (máx. retracción)
const int CICLOS_ARRASTRE = 3; // Número de veces que el actuador entra y sale
const unsigned long TIEMPO_EMPOT_RETRAYENDO = 3500;

#endif