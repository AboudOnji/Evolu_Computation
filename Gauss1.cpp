// Gauss1.cpp
// Modificado para ser usado como una librería por ee_completo.cpp

/* Rutina para generacion de numeros aleatorios gaussianos con
   media cero y desviacion estandar sigma.

   Codigo original de Joachim Sprave, University of Dortmund,
   Alemania, Dept. of Computer Science and Systems Analysis
*/
/* Este código fue adaptado por BARSEKH-ONJI Aboud (D.Sc.) a partir del curso de Coello Coello C. (PhD)*/
/* El código original fue escrito en C y se ha adaptado a C++.
   Se han realizado cambios para mejorar la legibilidad y la
   portabilidad, así como para seguir las mejores prácticas de C++.
   Se han añadido comentarios para explicar el funcionamiento del código.
*/
#include <stdlib.h> // Para rand(), srand(), RAND_MAX
#include <time.h>   // Para time()
#include <math.h>   // Para sqrt(), log(), exp()
#include <sys/types.h> 
// #include <iostream> // Ya no se necesita aquí si main() se elimina

// Si incluyes tu propio cabecero, puedes hacerlo, pero no es estrictamente necesario
// #include "gauss1.h" 

#define Uniform randreal // Macro para llamar a randreal

// Variables estáticas globales (específicas de este archivo gracias a static)
static int g_Seed_gauss = 0;    // Renombrado para evitar conflicto si hay otros g_Seed globales
static int g_isinit_gauss = 0;  // Renombrado para evitar conflicto

// Prototipos internos si son necesarios antes de su uso en este mismo archivo
// (ya están antes de su uso en el código que proporcionaste)

// Genera un número aleatorio real entre [0, 1]
// Esta función es usada por Gauss()
double randreal(void) // No es necesario que sea static si se declara en gauss1.h
{
    double result;

    if (!g_isinit_gauss) {
        if (g_Seed_gauss == 0) { 
            srand((unsigned int)time(NULL)); 
        } else {
            srand((unsigned int)g_Seed_gauss); 
        }
        g_isinit_gauss = 1;
    }
    result = ((double)rand());
    result /= ((double)RAND_MAX); 

    return (result);
}

// Implementación de la función Gauss
// Genera N(0, sigma)
double Gauss(double sigma) // No es necesario que sea static si se declara en gauss1.h
{
    double ret_val;
    static double u, x, y, u0, u1, u2; // Estas variables 'static' mantienen su valor entre llamadas a Gauss
                                       // Esto es parte del algoritmo original de Sprave para generar el N(0,1) base.
                                       // La parte de `sigma * x` al final lo escala.

    u = Uniform(); // Llama a randreal()
    u0 = Uniform(); // Llama a randreal()
    if (u >= .919544406) {
    goto L2;
    }
    x = (u0 + u * .825339283) * 2.40375766 - 2.11402808;
    goto L10;
  L2:
    if (u < .965487131) {
    goto L4;
    }
  L3:
    u1 = Uniform();
    y = sqrt(4.46911474 - log(u1) * 2.);
    u2 = Uniform();
    if (y * u2 > 2.11402808) {
    goto L3;
    }
    goto L9;
  L4:
    if (u < .949990709) {
    goto L6;
    }
  L5:
    u1 = Uniform();
    y = u1 * .273629336 + 1.84039875;
    u2 = Uniform();
    if (exp(y * -.5 * y) * .39894228 - .443299126 + y * .209694057 < u2 *
    .0427025816) {
    goto L5;
    }
    goto L9;
  L6:
    if (u < .925852334) {
    goto L8;
    }
  L7:
    u1 = Uniform();
    y = u1 * 1.55066917 + .289729574;
    u2 = Uniform();
    if (exp(y * -.5 * y) * .39894228 - .443299126 + y * .209694057 < u2 *
    .0159745227) {
    goto L7;
    }
    goto L9;
  L8:
    u1 = Uniform();
    y = u1 * .289729574;
    u2 = Uniform();
    if (exp(y * -.5 * y) * .39894228 - .382544556 < u2 * .0163977244) {
    goto L8;
    }
  L9:
    x = y;
    if (u0 >= .5) {
    x = -y;
    }
  L10:
    ret_val = sigma * x; // 'x' aquí es el N(0,1) base, escalado por 'sigma'
    return ret_val;
}

// Inicializa el generador de números aleatorios.
void initrandom(int seed_val) // No es necesario que sea static si se declara en gauss1.h
{
    g_Seed_gauss = seed_val;
    g_isinit_gauss = 0; // Permite que randreal vuelva a llamar a srand() con la nueva semilla
}


// Las funciones N() y randint() pueden quedarse si tu EE las necesita,
// o eliminarse si no son usadas por ee_completo.cpp.
// Por ahora las dejo, declarándolas también en el .h si ee_completo.cpp las va a usar.

// Genera un número aleatorio con distribución normal N(m, sigma)
double N(double m, double sigma) // No es necesario que sea static si se declara en gauss1.h
{
    return m + Gauss(sigma);
}

// Genera un número aleatorio entero entre [lo, hi] (inclusive)
int randint(int lo, int hi) // No es necesario que sea static si se declara en gauss1.h
{
    return (lo + (int)(randreal() * (hi - lo + 1.0)));
}

/*
// ----- FUNCIÓN MAIN ----- ELIMINADA O COMENTADA -----
// Punto de entrada del programa
int main() {
    // ... código de la main original ...
    return 0; 
}
*/