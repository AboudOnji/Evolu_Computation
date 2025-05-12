import os
import re
import numpy as np

def parse_results_file(filepath):
    """
    Parses a single results_ee.txt file to extract relevant statistics.
    """
    best_fitness_run = None
    final_x1 = None
    final_x2 = None

    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            for line in f:
                if "Mejor f(x) (padre) encontrado en la corrida:" in line:
                    match = re.search(r":\s*(-?\d+\.\d+)", line)
                    if match:
                        best_fitness_run = float(match.group(1))
                elif "Solucion final: x1=" in line:
                    match_x1 = re.search(r"x1=(-?\d+\.\d+)", line)
                    match_x2 = re.search(r"x2=(-?\d+\.\d+)", line)
                    if match_x1:
                        final_x1 = float(match_x1.group(1))
                    if match_x2:
                        final_x2 = float(match_x2.group(1))
        
        if best_fitness_run is not None and final_x1 is not None and final_x2 is not None:
            return {
                "filename": os.path.basename(filepath),
                "best_fitness": best_fitness_run,
                "final_x1": final_x1,
                "final_x2": final_x2
            }
        else:
            print(f"Advertencia: No se pudieron extraer todos los datos de {filepath}")
            return None
    except FileNotFoundError:
        print(f"Error: Archivo no encontrado {filepath}")
        return None
    except Exception as e:
        print(f"Error procesando {filepath}: {e}")
        return None

def calculate_overall_stats(run_data_list):
    """
    Calculates overall statistics from a list of best fitness values.
    """
    best_fitness_values = [data["best_fitness"] for data in run_data_list if data and "best_fitness" in data]
    
    if not best_fitness_values:
        return {
            "mean": "N/A", "variance": "N/A", "std_dev": "N/A",
            "overall_best": "N/A", "overall_worst": "N/A",
            "count": 0
        }

    mean_val = np.mean(best_fitness_values)
    variance_val = np.var(best_fitness_values) # Population variance
    std_dev_val = np.std(best_fitness_values)   # Population standard deviation
    overall_best_val = np.min(best_fitness_values)
    overall_worst_val = np.max(best_fitness_values)
    
    return {
        "mean": mean_val,
        "variance": variance_val,
        "std_dev": std_dev_val,
        "overall_best": overall_best_val,
        "overall_worst": overall_worst_val,
        "count": len(best_fitness_values)
    }

# --- Main script execution ---
if __name__ == "__main__":
    # Lista de los nombres de archivo que proporcionaste
    # Asegúrate de que estos archivos estén en el mismo directorio que este script Python,
    # o proporciona la ruta completa.
    filenames = [
        "results_ee11.txt", "results_ee12.txt", "results_ee13.txt",
        "results_ee14.txt", "results_ee15.txt", "results_ee16.txt",
        "results_ee17.txt", "results_ee18.txt", "results_ee19.txt",
        "results_ee20.txt"
    ]

    all_run_data = []
    for fname in filenames:
        data = parse_results_file(fname)
        if data:
            all_run_data.append(data)

    if not all_run_data:
        print("No se pudieron procesar datos de ninguna corrida. Saliendo.")
    else:
        # Calcular estadísticas generales
        overall_stats = calculate_overall_stats(all_run_data)

        # Imprimir datos para la tabla LaTeX
        print("\n% --- Datos para la Tabla LaTeX de Corridas Individuales ---")
        print("% Gmax = 200 para todas estas corridas (asumido/verificado de los archivos)")
        for i, data in enumerate(all_run_data):
            # Corrida & Mejor $f(x)$ & $x_1$ final & $x_2$ final \\
            print(f"{i+1} & {data['best_fitness']:.8f} & {data['final_x1']:.8f} & {data['final_x2']:.8f} \\\\")
        print("% -----------------------------------------------------")

        # Imprimir estadísticas generales para LaTeX
        print("\n% --- Datos para Estadísticas Generales en LaTeX ---")
        print(f"% Numero de Corridas Exitosamente Procesadas: {overall_stats['count']}")
        print(f"\\def\\statMean{{{overall_stats['mean']:.8f}}")
        print(f"\\def\\statVariance{{{overall_stats['variance']:.8f}}")
        print(f"\\def\\statStdDev{{{overall_stats['std_dev']:.8f}}")
        print(f"\\def\\statOverallBest{{{overall_stats['overall_best']:.8f}}")
        print(f"\\def\\statOverallWorst{{{overall_stats['overall_worst']:.8f}}")
        print("% -------------------------------------------------")

        # Identificar corridas que se acercan a los óptimos conocidos
        # Optima: f(0.0898, -0.7126) = -1.0316 y f(-0.0898, 0.7126) = -1.0316
        opt_val = -1.031628
        tolerance = 1e-4 
        print("\n% --- Corridas Cercanas a los Óptimos Conocidos (aprox. -1.031628) ---")
        found_optima_runs = []
        for i, data in enumerate(all_run_data):
            if abs(data['best_fitness'] - opt_val) < tolerance:
                # Check if x1 and x2 match one of the two optima patterns
                # Opt1: x1 > 0, x2 < 0
                # Opt2: x1 < 0, x2 > 0
                is_opt1 = (data['final_x1'] > 0 and data['final_x2'] < 0 and 
                           abs(abs(data['final_x1']) - 0.0898) < 0.01 and abs(abs(data['final_x2']) - 0.7126) < 0.01)
                is_opt2 = (data['final_x1'] < 0 and data['final_x2'] > 0 and
                           abs(abs(data['final_x1']) - 0.0898) < 0.01 and abs(abs(data['final_x2']) - 0.7126) < 0.01)
                
                if is_opt1 or is_opt2:
                    opt_type = "Optimo 1 (aprox. x1=0.0898, x2=-0.7126)" if is_opt1 else "Optimo 2 (aprox. x1=-0.0898, x2=0.7126)"
                    run_info = f"Corrida {i+1} ({data['filename']}): f(x)={data['best_fitness']:.8f}, x1={data['final_x1']:.6f}, x2={data['final_x2']:.6f} ({opt_type})"
                    found_optima_runs.append(run_info)
                    print(f"% {run_info}")

        if not found_optima_runs:
            print("% Ninguna de las corridas analizadas alcanzó un óptimo conocido con la tolerancia especificada.")
        print("% ----------------------------------------------------------------")