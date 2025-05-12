% --- Script de MATLAB para Graficar la Función Objetivo en 3D ---

% Limpiar el espacio de trabajo, cerrar figuras y limpiar la ventana de comandos
clear;
clc;
close all;

% --- Definición de la Función Objetivo ---
% Esta es una función anónima en MATLAB.
% Alternativamente, podrías definirla en un archivo .m separado si es más compleja.
objectiveFunction = @(x1, x2) (4 - 2.1*x1.^2 + (x1.^4)/3).*x1.^2 + ...
                               x1.*x2 + ...
                               (-4 + 4*x2.^2).*x2.^2;

% --- Definición de Rangos y Resolución ---
x1_min = -3.0;
x1_max = 3.0;
x2_min = -2.0;
x2_max = 2.0;

num_points_x1 = 100; % Número de puntos para el eje x1 (resolución)
num_points_x2 = 100; % Número de puntos para el eje x2 (resolución)

% Crear vectores de puntos para x1 y x2
x1_vals = linspace(x1_min, x1_max, num_points_x1);
x2_vals = linspace(x2_min, x2_max, num_points_x2);

% Crear una malla (grid) de puntos (X1, X2) a partir de los vectores
[X1, X2] = meshgrid(x1_vals, x2_vals);

% Evaluar la función objetivo en cada punto de la malla
Z = objectiveFunction(X1, X2);

% --- Creación de la Gráfica 3D ---
figure; % Crear una nueva figura

% Usar surf para una gráfica de superficie con colores
% surf(X1, X2, Z);
% Usar mesh para una gráfica de malla (solo líneas)
% mesh(X1, X2, Z);

% Usar surf con 'EdgeColor','none' para una superficie más suave
surf(X1, X2, Z, 'EdgeColor', 'none', 'FaceAlpha', 0.9); 
% 'FaceAlpha' controla la transparencia de la superficie (0=transparente, 1=opaco)


% Añadir etiquetas y título
xlabel('x1');
ylabel('x2');
zlabel('f(x1, x2)');
title('Gráfica 3D de la Función Objetivo f(x1,x2)');

% Añadir una barra de colores para indicar los valores de Z
colorbar;

% Rotar la vista para una mejor perspectiva (opcional)
% view(az, el) - az es el azimut (rotación horizontal), el es la elevación
view(-35, 30); % Puedes experimentar con estos valores

% Otras opciones de visualización (opcional):
% shading interp; % Interpola los colores para una apariencia más suave
% axis tight;     % Ajusta los ejes al rango de los datos
grid on;        % Muestra una rejilla en el fondo

% Para guardar la figura (opcional):
% print('funcion_objetivo_3d', '-dpng', '-r300'); % Guarda como PNG con 300 DPI

disp('Gráfica 3D generada.');
disp('Puedes rotar la gráfica interactivamente usando el ratón en la ventana de la figura.');