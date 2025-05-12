// gauss1.h
#ifndef GAUSS1_H
#define GAUSS1_H

// Prototipos de las funciones definidas en Gauss1.cpp que serán usadas por otros archivos

// Inicializa la semilla del generador aleatorio usado por Gauss() y randreal()
void initrandom(int seed_val);

// Genera un número aleatorio uniforme en [0,1]
// Necesario si ee_completo.cpp lo usa para inicializar parent_x
double randreal(void); 

// Genera un número aleatorio N(0, sigma_param)
// ee_completo.cpp llamará a Gauss(1.0) para obtener N(0,1)
double Gauss(double sigma_param);

// Opcional: si ee_completo.cpp necesita estas funciones también
// double N(double m, double sigma_param);
// int randint(int lo, int hi);

#endif // GAUSS1_H