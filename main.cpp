/* Este código corresponde a la tarea 1
Curso: Computación Evolutiva
Prof. COELLO COELLO Carlos (PhD)
Alumno. BARSEKH-ONJI Aboud (M EEng)
Fecha: may12, 2025*/

/* Notas para reompilar correctamente:
Debido a que se solicita g++
- g++ main.cpp Gauss1.cpp -o EE1_Gauss1 -std=c++11
Para la visualización de los datos (figura en 3D):
- Utilicé gnuplot, para la instalación: brew install gnuplot
- gnuplot --- splot 'plot_data_function.dat' with pm3d title "f(x1,x2)"*/

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <cstdlib> 
#include <ctime>   
#include <iomanip>
#include <limits>     
#include <numeric>   
#include <algorithm>  

#include "gauss1.h" // este archivo contiene el código compartido por el Dr. Coello Coello

// Constantes y parámetros globales
const int N_VARIABLES = 2; 
const double C_VAL = 0.90; 
const double SIGMA_INITIAL = 3.0;
const int PS_UPDATE_INTERVAL = 10 * N_VARIABLES; 

const double X1_MIN = -3.0;
const double X1_MAX = 3.0;
const double X2_MIN = -2.0;
const double X2_MAX = 2.0;

// Función objetivo 
double objectiveFunctionEval(double x1_val, double x2_val) {
    return (4.0 - 2.1 * pow(x1_val, 2) + pow(x1_val, 4) / 3.0) * pow(x1_val, 2) +
           x1_val * x2_val +
           (-4.0 + 4.0 * pow(x2_val, 2)) * pow(x2_val, 2);
}

double evaluateObjectiveFunctionEE(const std::vector<double>& x) {
    return objectiveFunctionEval(x[0], x[1]);
}

// Clamping
double applyBounds(double value, double min_val, double max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

// Generar Datos para Gráfica 3D de la Función Objetivo
void generatePlotDataFunction() {
    std::cout << "Generando datos para la grafica 3D de la funcion objetivo..." << std::endl;
    std::ofstream plotFile("plot_data_function.dat");
    if (!plotFile.is_open()) {
        std::cerr << "Error al abrir el archivo plot_data_function.dat" << std::endl;
        return;
    }
    const int N_POINTS_X1_PLOT = 50; 
    const int N_POINTS_X2_PLOT = 50;
    double step_x1_plot = (X1_MAX - X1_MIN) / (double)(N_POINTS_X1_PLOT - 1);
    double step_x2_plot = (X2_MAX - X2_MIN) / (double)(N_POINTS_X2_PLOT - 1);
    plotFile << std::fixed << std::setprecision(8);
    for (int i = 0; i < N_POINTS_X1_PLOT; ++i) {
        double current_x1 = X1_MIN + i * step_x1_plot;
        for (int j = 0; j < N_POINTS_X2_PLOT; ++j) {
            double current_x2 = X2_MIN + j * step_x2_plot;
            double z = objectiveFunctionEval(current_x1, current_x2);
            plotFile << current_x1 << " " << current_x2 << " " << z << std::endl;
        }
        plotFile << std::endl; 
    }
    plotFile.close();
    std::cout << "Datos para la grafica guardados en plot_data_function.dat" << std::endl;
    std::cout << "Puedes usar Gnuplot con: splot 'plot_data_function.dat' with lines" << std::endl;
}

// Main Program
// Este es el punto de entrada del algoritmo evolutivo (1+1)-EE
int main(int argc, char *argv[]) {
    if (argc >= 2 && std::string(argv[1]) == "--plotdata") {
        generatePlotDataFunction();
        return 0; 
    }

    int Gmax = 100; 
    std::vector<double> initial_parent_x_config(N_VARIABLES);
    bool useCustomInitialX = false;
    long customSeed = 0; 
    bool useCustomSeed = false;

    if (argc > 1) { 
        try {
            Gmax = std::stoi(argv[1]);
            if (Gmax <= 10) {
                std::cerr << "Advertencia: Gmax (" << Gmax << ") debe ser > 10. Usando Gmax = 100." << std::endl;
                Gmax = 100;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error al leer Gmax. Usando Gmax = 100. Error: " << e.what() << std::endl;
            Gmax = 100;
        }
    } else { 
        std::cout << "Modo: Ejecutar Estrategia Evolutiva (1+1)-EE." << std::endl;
        std::cout << "Ingrese el numero maximo de generaciones (Gmax > 10) [default 100]: ";
        std::string gmax_input;
        std::getline(std::cin, gmax_input);
        if (!gmax_input.empty()) {
            try {
                Gmax = std::stoi(gmax_input);
                if (Gmax <= 10) {
                     std::cerr << "Advertencia: Gmax debe ser > 10. Usando Gmax = 100." << std::endl;
                     Gmax = 100;
                }
            } catch (const std::exception& e) {
                std::cerr << "Entrada invalida para Gmax. Usando Gmax = 100." << std::endl;
                Gmax = 100;
            }
        } else {
            std::cout << "Usando Gmax = 100 por defecto." << std::endl;
        }
    }

    if (argc > 3) { 
        try {
            initial_parent_x_config[0] = std::stod(argv[2]);
            initial_parent_x_config[1] = std::stod(argv[3]);
            useCustomInitialX = true;
        } catch (const std::exception& e) {
            std::cerr << "Error al leer x_iniciales. Usando aleatorios. Error: " << e.what() << std::endl;
            useCustomInitialX = false;
        }
    }
    if (argc > 4) { 
        try {
            customSeed = std::stol(argv[4]);
            useCustomSeed = true;
            std::cout << "Usando semilla personalizada: " << customSeed << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error al leer semilla. Usando time(NULL). Error: " << e.what() << std::endl;
            useCustomSeed = false;
        }
    }
    
    initrandom(useCustomSeed ? static_cast<int>(customSeed) : 0); 

    std::vector<double> parent_x(N_VARIABLES);
    if (useCustomInitialX) {
        parent_x = initial_parent_x_config;
    } else {
        std::cout << "Inicializando x1, x2 aleatoriamente dentro de los limites para la EE." << std::endl;
        parent_x[0] = X1_MIN + (X1_MAX - X1_MIN) * randreal(); 
        parent_x[1] = X2_MIN + (X2_MAX - X2_MIN) * randreal(); 
    }
    parent_x[0] = applyBounds(parent_x[0], X1_MIN, X1_MAX);
    parent_x[1] = applyBounds(parent_x[1], X2_MIN, X2_MAX);
    std::cout << "Valores iniciales para EE: x1 = " << parent_x[0] << ", x2 = " << parent_x[1] << std::endl;
    
    double current_parent_fitness = evaluateObjectiveFunctionEE(parent_x); 
    
    // Cálculo de estadísticas de la corrida actual (mejor y peor f(x), media, varianza, desviación estándar.)
    double best_fitness_in_run = current_parent_fitness;
    double worst_fitness_in_run = current_parent_fitness; // El peor fitness evaluado
    std::vector<double> fitness_history_of_parent; // Almacena el fitness del padre en cada generación
    fitness_history_of_parent.push_back(current_parent_fitness);


    std::vector<double> sigma_history;
    sigma_history.push_back(SIGMA_INITIAL); 

    double current_ps = 0.2; 
    int successes_in_ps_window = 0;
    int gens_in_ps_window_count = 0;

    std::ofstream outputFile("results_ee.txt"); 
    if (!outputFile.is_open()) {
        std::cerr << "Error al abrir el archivo results_ee.txt" << std::endl;
        return 1;
    }
    outputFile << std::fixed << std::setprecision(8);
    outputFile << "Generacion\tx1\tx2\tFitness_Padre\tSigma_Usado" << std::endl;

    for (int t = 0; t <= Gmax; ++t) {
        double sigma_for_mutation;
        if (t == 0) {
            sigma_for_mutation = SIGMA_INITIAL;
        } else {
            double new_sigma_val;
            if (t % N_VARIABLES == 0) { 
                if (t < N_VARIABLES) { 
                     new_sigma_val = sigma_history.back(); 
                } else {
                    double sigma_t_minus_n = sigma_history[t - N_VARIABLES];
                    if (current_ps > (1.0/5.0 + 1e-9)) { 
                        new_sigma_val = sigma_t_minus_n / C_VAL;
                    } else if (current_ps < (1.0/5.0 - 1e-9)) { 
                        new_sigma_val = sigma_t_minus_n * C_VAL;
                    } else { 
                        new_sigma_val = sigma_t_minus_n;
                    }
                }
            } else { 
                new_sigma_val = sigma_history.back(); 
            }
            sigma_history.push_back(new_sigma_val);
            sigma_for_mutation = new_sigma_val;
        }
        
        std::vector<double> offspring_x = parent_x; // Copia del padre para mutar
        // Solo mutar si no es la primera "evaluación" (t=0 es la evaluación inicial del padre)
        if (t > 0 || Gmax == 0) { // Mutar a partir de la segunda iteración (t=1) o si Gmax=0 (una sola mutación)
                                  // O considerar que la generación 0 ya implica una mutación sobre un padre "conceptual" anterior
                                  // El pseudocódigo del PDF es: t=0, init x, eval f(x), LUEGO entra al while t<=Gmax, muta.
                                  // Así que el primer sigma[0] se usa para la primera mutación.
                                  // Aquí, parent_x ya está inicializado, así que la primera mutación (t=0) es correcta.
            for (int i = 0; i < N_VARIABLES; ++i) {
                offspring_x[i] += sigma_for_mutation * Gauss(1.0); 
            }
            offspring_x[0] = applyBounds(offspring_x[0], X1_MIN, X1_MAX);
            offspring_x[1] = applyBounds(offspring_x[1], X2_MIN, X2_MAX);
        }
        
        double offspring_fitness = evaluateObjectiveFunctionEE(offspring_x);

        // Actualizar peor fitness (considerando todos los evaluados, incluyendo el hijo actual)
        if (offspring_fitness > worst_fitness_in_run) {
            worst_fitness_in_run = offspring_fitness;
        }
        // El fitness inicial del padre también contribuye al peor
        if (t == 0 && current_parent_fitness > worst_fitness_in_run) {
             worst_fitness_in_run = current_parent_fitness;
        }


        bool mutation_was_successful = false;
        // En (1+1)-EE, el hijo reemplaza al padre solo si es estrictamente mejor (para minimización)
        // Si t=0, offspring_x es igual a parent_x si no hubo mutación arriba, o es el primer mutante.
        // El pseudocódigo compara x_barra con x_barra_prima y selecciona el mejor.
        // current_parent_fitness es f(x_barra). offspring_fitness es f(x_barra_prima).
        if (t == 0 && Gmax > 0) { // Para la primera iteración, solo registramos el padre inicial
             // No hay "mutación exitosa" aún, solo la evaluación inicial.
             // El best_fitness_in_run ya se inicializó con current_parent_fitness.
        } else if (offspring_fitness < current_parent_fitness) {
            parent_x = offspring_x;
            current_parent_fitness = offspring_fitness;
            mutation_was_successful = true; 
        }
        // Si no es mejor, parent_x y current_parent_fitness no cambian.
        
        // Actualizar el mejor fitness de la corrida (el del padre actual es siempre el mejor o igual que antes)
        if (current_parent_fitness < best_fitness_in_run) {
            best_fitness_in_run = current_parent_fitness;
        }
        
        // Guardar el fitness del padre de esta generación (después de posible selección)
        if (t > 0 || Gmax == 0) { // Si t=0, ya se guardó el inicial
             fitness_history_of_parent.push_back(current_parent_fitness);
        } else if (t==0 && fitness_history_of_parent.empty()){ // Caso Gmax > 0, t=0
             // fitness_history_of_parent ya tiene el primer fitness.
        }


        outputFile << t << "\t"
                   << parent_x[0] << "\t"
                   << parent_x[1] << "\t"
                   << current_parent_fitness << "\t" // Fitness del padre actual
                   << sigma_for_mutation << std::endl;
        
        if (mutation_was_successful) {
            successes_in_ps_window++;
        }
        if (t > 0 || Gmax == 0) { // No contar la "generación 0" como parte de la ventana de p_s
                                  // a menos que Gmax sea 0 (una sola iteración de mutación)
            gens_in_ps_window_count++;
        }


        if (PS_UPDATE_INTERVAL > 0 && gens_in_ps_window_count == PS_UPDATE_INTERVAL) {
             current_ps = (double)successes_in_ps_window / PS_UPDATE_INTERVAL;
            successes_in_ps_window = 0;
            gens_in_ps_window_count = 0;
        }
    } 

    // Calcular estadísticas de los fitness del padre a lo largo de la corrida
    double sum_fitness = 0.0;
    for (double fit : fitness_history_of_parent) {
        sum_fitness += fit;
    }
    double mean_fitness_of_parent_trajectory = sum_fitness / fitness_history_of_parent.size();

    double sum_sq_diff = 0.0;
    for (double fit : fitness_history_of_parent) {
        sum_sq_diff += (fit - mean_fitness_of_parent_trajectory) * (fit - mean_fitness_of_parent_trajectory);
    }
    double variance_fitness_of_parent_trajectory = sum_sq_diff / fitness_history_of_parent.size();
    double stddev_fitness_of_parent_trajectory = sqrt(variance_fitness_of_parent_trajectory);


    outputFile << "\n--- ESTADISTICAS DE ESTA CORRIDA ---" << std::endl;
    outputFile << "Mejor f(x) (padre) encontrado en la corrida: " << best_fitness_in_run << std::endl;
    outputFile << "Peor f(x) (evaluado) encontrado en la corrida: " << worst_fitness_in_run << std::endl;
    outputFile << "Fitness final del padre: " << current_parent_fitness << std::endl;
    outputFile << "Solucion final: x1=" << parent_x[0] << ", x2=" << parent_x[1] << std::endl;
    outputFile << "Media de f(x) del padre a lo largo de las generaciones: " << mean_fitness_of_parent_trajectory << std::endl;
    outputFile << "Varianza de f(x) del padre a lo largo de las generaciones: " << variance_fitness_of_parent_trajectory << std::endl;
    outputFile << "Desviacion Estandar de f(x) del padre a lo largo de las generaciones: " << stddev_fitness_of_parent_trajectory << std::endl;
    outputFile << "-----------------------------------" << std::endl;

    outputFile.close();

    // Salida a consola 
    std::cout << "Proceso EE completado. Resultados y estadisticas de la corrida guardados en results_ee.txt" << std::endl;
    std::cout << "--- Resumen de ESTA corrida (tambien en results_ee.txt) ---" << std::endl;
    std::cout << "Mejor f(x) (padre) encontrado: " << std::fixed << std::setprecision(8) << best_fitness_in_run << std::endl;
    std::cout << "Peor f(x) (evaluado) encontrado: " << worst_fitness_in_run << std::endl;
    std::cout << "Fitness final del padre: " << current_parent_fitness << std::endl;
    std::cout << "Solucion final: x1=" << parent_x[0] << ", x2=" << parent_x[1] << std::endl;
    std::cout << "Media f(x) del padre (trayectoria): " << mean_fitness_of_parent_trajectory << std::endl;
    std::cout << "Varianza f(x) del padre (trayectoria): " << variance_fitness_of_parent_trajectory << std::endl;
    std::cout << "Desv. Est. f(x) del padre (trayectoria): " << stddev_fitness_of_parent_trajectory << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    return 0;
}

// Fin del código
