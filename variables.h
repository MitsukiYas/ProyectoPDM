#ifndef VARIABLES_H
#define VARIABLES_H

extern int carroVelocidad;
extern int vagonesTrasladados;
extern const int vagonesTotales;
extern int autoclavesCompletos;
extern const int autoclavesTotales;

extern bool empotActivo;
extern bool empotDireccion;

extern bool engancheActivo;
extern bool engancheDireccion;
extern long enganchePasosRestantes;
extern bool engancheModoPasos;

const int carroSensorCap = 13;
const int sensorFinEmpot1 = 10;
const int sensorFinEmpot2 = 11;
const int sensorFinEnganche = 12;


void moverEnganchePasos(long vueltas, bool direccion);
void moverEmpotPasos(long vueltas, bool direccion);

#endif