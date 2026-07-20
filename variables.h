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

#endif