#ifndef VARIABLES_H
#define VARIABLES_H

extern String comando;

extern int carroVelocidad;
extern int vagonesTrasladados;
extern const int vagonesTotales;
extern int autoclavesCompletos;
extern const int autoclavesTotales;

extern bool empotActivo;
extern bool empotDireccion;
extern long empotPasosRestantes;

extern bool engancheActivo;
extern bool engancheDireccion;
extern long enganchePasosRestantes;
extern bool engancheModoPasos;




const int carroSensorCap = 10;
const int sensorFinEmpot1 = 13;
const int sensorFinEmpot2 = 12;
const int sensorFinEnganche = 11;
extern const int pinLectura;
extern const int pinActuador;



void moverEnganchePasos(long vueltas, bool direccion);
void moverEmpotPasos(long vueltas, bool direccion);

#endif