function ee_tarea1_matlab()
    

    % --- Constantes y Parámetros Globales ---
    N_VARIABLES = 2;         
    C_VAL = 0.85;            
    SIGMA_INITIAL = 3.0;     

    % Límites para las variables x1, x2
    X1_MIN = -3.0; X1_MAX = 3.0;
    X2_MIN = -2.0; X2_MAX = 2.0;

    % --- Configuración Inicial ---
    % Inicializar semilla para generadores aleatorios de MATLAB
    rng('shuffle'); % Equivalente a time(0) para la semilla

    % Obtener Gmax del usuario
    Gmax_str = input('Ingrese el numero maximo de generaciones (Gmax > 10): ', 's');
    Gmax = str2double(Gmax_str);
    if isnan(Gmax) || Gmax <= 10
        disp('Gmax invalido o <= 10. Usando Gmax = 100 por defecto.');
        Gmax = 100;
    end

    % Inicializar variables x_bar (padre) aleatoriamente
    parent_x = zeros(1, N_VARIABLES);
    disp('Inicializando x1, x2 aleatoriamente dentro de los limites.');
    parent_x(1) = X1_MIN + (X1_MAX - X1_MIN) * rand();
    parent_x(2) = X2_MIN + (X2_MAX - X2_MIN) * rand();
    
    % Asegurarse de que los valores están dentro de los límites (aunque rand() debería garantizarlo si se usa bien)
    % Esta llamada a applyBounds es una buena práctica por si acaso.
    parent_x(1) = applyBounds(parent_x(1), X1_MIN, X1_MAX);
    parent_x(2) = applyBounds(parent_x(2), X2_MIN, X2_MAX);

    fprintf('Valores iniciales (aleatorios): x1 = %.6f, x2 = %.6f\n', parent_x(1), parent_x(2));

    parent_fitness = evaluateObjectiveFunction(parent_x);

    % sigma_pdf_def(k) almacenará sigma[k-1] según la notación del PDF (sigma[0], sigma[1],...)
    sigma_pdf_def = zeros(1, Gmax + 1); 
    sigma_pdf_def(1) = SIGMA_INITIAL; % sigma_pdf_def(1) es sigma[0]

    current_ps = 0.2; % Valor inicial/por defecto para ps (1/5)
    successes_in_ps_window = 0;
    gens_in_ps_window_count = 0;
    PS_UPDATE_INTERVAL_CALC = 10 * N_VARIABLES;

    % Archivo de salida
    fileID = fopen('results_matlab.txt', 'w');
    if fileID == -1
        error('Error al abrir el archivo results_matlab.txt');
    end
    fprintf(fileID, 'Generacion\tx1\tx2\tFitness\tSigma_Usado\n');

    % --- Bucle Evolutivo (1+1)-EE ---
    % t_gen es el contador de generaciones, de 0 a Gmax (como en el PDF)
    for t_gen = 0:Gmax
        sigma_for_mutation_this_gen = 0; % Se asignará abajo

        % Determinar sigma[t_gen] (sigma a usar para la mutación en la generación t_gen)
        % sigma_pdf_def(t_gen + 1) representa sigma[t_gen]
        if t_gen == 0
            sigma_for_mutation_this_gen = sigma_pdf_def(1); % Usa sigma[0]
        else
            % Calcular sigma_pdf_def(t_gen + 1), que es sigma[t_gen]
            if mod(t_gen, N_VARIABLES) == 0 % Condición t_gen mod n == 0
                idx_t_minus_n = (t_gen - N_VARIABLES) + 1; % Índice para sigma[t_gen - n]
                
                sigma_ref_for_rule = 0;
                if idx_t_minus_n < 1 % No hay suficiente historia para sigma[t_gen-n]
                                     % El PDF para sigma[t] = sigma[t-n]... o sigma[t-1]
                                     % Si t_gen < N_VARIABLES pero t_gen % N_VARIABLES == 0 (ej. n=2, t_gen=0,2,4..)
                                     % (si t_gen=0, ya se manejó)
                                     % si t_gen=N_VARIABLES, idx_t_minus_n = 1 (sigma[0])
                                     % si t_gen=2*N_VARIABLES, idx_t_minus_n = N_VARIABLES+1 (sigma[N_VARIABLES])
                                     % Si t_gen es N_VARIABLES (ej. t_gen=2, n=2), usa sigma_pdf_def( (2-2)+1 ) = sigma_pdf_def(1) (sigma[0])
                    sigma_ref_for_rule = sigma_pdf_def(t_gen); % Usa sigma[t_gen-1] como referencia
                                                               % Esta es una interpretación si sigma[t-n] no está definido
                                                               % El C++ tenia un if (t < N_VARIABLES) para esto.
                                                               % Si t_gen < N_VARIABLES, entonces idx_t_minus_n siempre será < 1.
                                                               % Correcto: si idx_t_minus_n < 1, significa que t_gen-N_VARIABLES < 0.
                                                               % Esto implica t_gen < N_VARIABLES.
                                                               % En este caso, la regla estricta sigma[t-n] no se aplica.
                                                               % La especificación del PDF es sigma[t] = { sigma[t-n]/c ... } o sigma[t] = sigma[t-1].
                                                               % Si t_gen < N_VARIABLES, la condición t_gen mod N_VARIABLES == 0 solo se cumple para t_gen=0.
                                                               % Para t_gen=0, sigma_for_mutation_this_gen ya es SIGMA_INITIAL.
                                                               % Entonces este 'if mod(t_gen, N_VARIABLES) == 0' se evalúa para t_gen >= N_VARIABLES.
                                                               % Por lo tanto, idx_t_minus_n siempre será >= 1.
                    sigma_ref_for_rule = sigma_pdf_def(idx_t_minus_n); % Esto es sigma[t_gen - N_VARIABLES]
                end

                if current_ps > (1.0/5.0 + 1e-9)      % ps > 1/5
                    sigma_pdf_def(t_gen + 1) = sigma_ref_for_rule / C_VAL;
                elseif current_ps < (1.0/5.0 - 1e-9)  % ps < 1/5
                    sigma_pdf_def(t_gen + 1) = sigma_ref_for_rule * C_VAL;
                else % ps == 1/5
                    sigma_pdf_def(t_gen + 1) = sigma_ref_for_rule;
                end
            else % else sigma[t_gen] = sigma[t_gen-1]
                sigma_pdf_def(t_gen + 1) = sigma_pdf_def(t_gen); % sigma_pdf_def(t_gen) es sigma[t_gen-1]
            end
            sigma_for_mutation_this_gen = sigma_pdf_def(t_gen + 1);
        end
        
        % Mutar el vector x_i usando: x_i' = x_i + sigma[t_gen] * N_i(0,1)
        offspring_x = parent_x;
        for i = 1:N_VARIABLES
            % randn() genera N(0,1). Se multiplica por sigma.
            offspring_x(i) = offspring_x(i) + sigma_for_mutation_this_gen * randn();
        end

        % Aplicar límites a las variables del hijo
        offspring_x(1) = applyBounds(offspring_x(1), X1_MIN, X1_MAX);
        offspring_x(2) = applyBounds(offspring_x(2), X2_MIN, X2_MAX);

        % Evaluar f(x_bar')
        offspring_fitness = evaluateObjectiveFunction(offspring_x);

        % Comparar y seleccionar el mejor (minimización)
        mutation_was_successful = false;
        if offspring_fitness < parent_fitness
            parent_x = offspring_x;
            parent_fitness = offspring_fitness;
            mutation_was_successful = true;
        end

        % Imprimir resultados en archivo
        fprintf(fileID, '%d\t%.8f\t%.8f\t%.8f\t%.8f\n', ...
                t_gen, parent_x(1), parent_x(2), parent_fitness, sigma_for_mutation_this_gen);
        
        % Opcional: Imprimir en consola (cada cierto número de generaciones)
        if mod(t_gen, 100) == 0 || t_gen == Gmax
             fprintf('Gen: %d | x1: %.6f | x2: %.6f | Fitness: %.6f | Sigma: %.6f | p_s: %.4f\n', ...
                 t_gen, parent_x(1), parent_x(2), parent_fitness, sigma_for_mutation_this_gen, current_ps);
        end

        % Actualizar contadores para p_s
        if mutation_was_successful
            successes_in_ps_window = successes_in_ps_window + 1;
        end
        gens_in_ps_window_count = gens_in_ps_window_count + 1;

        % Actualizar p_s cada PS_UPDATE_INTERVAL_CALC (10n) iteraciones
        if gens_in_ps_window_count == PS_UPDATE_INTERVAL_CALC
            if PS_UPDATE_INTERVAL_CALC > 0
                 current_ps = successes_in_ps_window / PS_UPDATE_INTERVAL_CALC;
            else
                current_ps = 0.2; % Default
            end
            successes_in_ps_window = 0;
            gens_in_ps_window_count = 0;
        end
    end % Fin del bucle de generaciones

    fclose(fileID);
    disp('Proceso completado. Resultados guardados en results_matlab.txt');
    fprintf('Ultima solucion: x1=%.6f, x2=%.6f, f(x)=%.6f, sigma_final=%.6f\n', ...
        parent_x(1), parent_x(2), parent_fitness, sigma_pdf_def(Gmax + 1));

end % Fin de la función principal ee_tarea1_matlab

% --- Funciones Locales ---

% Función Objetivo f(x1,x2)
function fitness = evaluateObjectiveFunction(x_vec)
    x1 = x_vec(1);
    x2 = x_vec(2);
    fitness = (4.0 - 2.1 * x1^2 + x1^4/3.0) * x1^2 + ...
              x1 * x2 + ...
              (-4.0 + 4.0 * x2^2) * x2^2;
end

% Manejo de Límites (Clamping)
function value = applyBounds(value, min_val, max_val)
    if value < min_val
        value = min_val;
    elseif value > max_val
        value = max_val;
    end
end