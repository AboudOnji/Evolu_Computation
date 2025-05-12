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
#include <sys/types.h> // Puede no ser estrictamente necesario en todos los sistemas modernos para este código
#include <iostream> // Para std::cout y std::endl en la función main

#define NRAND_SAMPLES 5 // Definido pero no usado en el código proporcionado
#define Uniform randreal // Macro para llamar a randreal

// Variables estáticas globales
static int g_Seed = 0;    // Renombrado para evitar confusión con parámetros de función.
                          // Si es 0, se usará time(NULL) para la semilla.
static int g_isinit = 0;  // Bandera para saber si srand() ya fue llamado.

// Prototipos de funciones (buena práctica tenerlos, aunque C++ puede inferirlos si están antes del uso)
double randreal(void);
void initrandom(int seed_val); // Cambiado nombre de parámetro
double Gauss(double sigma);
double N(double m, double sigma);
int randint(int lo, int hi);

// Implementación de la función Gauss
double Gauss(double sigma)
{
    double ret_val;
    // static double d, u, x, y, u0, u1, u2; // 'd' no se usa.
    static double u, x, y, u0, u1, u2;


/* SIGMA   --> standard deviation */

/* L1: */
    u = Uniform();
    u0 = Uniform();
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
    ret_val = sigma * x;
    return ret_val;
}

// Genera un número aleatorio con distribución normal N(m, sigma)
double N(double m, double sigma)
{
    return m + Gauss(sigma);
}

// Inicializa el generador de números aleatorios.
// Si seed_val es 0, se usará el tiempo actual.
// Si se llama varias veces, permite re-sembrar.
void initrandom(int seed_val)
{
    g_Seed = seed_val;
    g_isinit = 0; // Permite que randreal vuelva a llamar a srand() con la nueva semilla
}

// Genera un número aleatorio real entre [0, 1]
double randreal(void)
{
    double result;

    if (!g_isinit) {
        if (g_Seed == 0) { // Si no se especificó una semilla o se pasó 0
            srand((unsigned int)time(NULL)); // Usar el tiempo actual como semilla
        } else {
            srand((unsigned int)g_Seed); // Usar la semilla especificada
        }
        g_isinit = 1;
    }
    result = ((double)rand());
    result /= ((double)RAND_MAX); // Asegurar división de punto flotante

    return (result);
}

// Genera un número aleatorio entero entre [lo, hi] (inclusive)
int randint(int lo, int hi)
{
    // El +1 es para incluir 'hi' en el rango posible.
    // (int) truncará el resultado.
    return (lo + (int)(randreal() * (hi - lo + 1.0)));
}

// ----- FUNCIÓN MAIN -----
// Punto de entrada del programa
int main() {
    // Inicializar el generador de números aleatorios.
    // Puedes usar una semilla específica para obtener resultados reproducibles:
    // initrandom(12345);
    // O no llamar a initrandom (o llamar initrandom(0)) para usar el tiempo actual:
    initrandom(0); // O comenta esta línea para el mismo efecto si g_Seed es 0 por defecto.

    std::cout << "Generando numeros aleatorios Gaussianos:" << std::endl;

    double media = 5.0;
    double desviacion_estandar = 2.0;

    std::cout << "Distribucion N(media=" << media << ", sigma=" << desviacion_estandar << "):" << std::endl;
    for (int i = 0; i < 10; ++i) {
        std::cout << N(media, desviacion_estandar) << std::endl;
    }

    std::cout << "\nDistribucion Gauss (media=0, sigma=" << desviacion_estandar << "):" << std::endl;
    for (int i = 0; i < 10; ++i) {
        std::cout << Gauss(desviacion_estandar) << std::endl;
    }

    std::cout << "\nNumero entero aleatorio entre 10 y 20:" << std::endl;
    for (int i = 0; i < 5; ++i) {
        std::cout << randint(10, 20) << std::endl;
    }
    
    return 0; // Indicar que el programa terminó correctamente
}