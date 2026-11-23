# Paint 2D

## Autor

Daniel Alejandro Fuentes Bojaca - 6014544

## Descripción
Proyecto desarrollado en C++ utilizando OpenGL, GLFW, GLAD y ImGui. 
Es un programa de dibujo que permite usar figuras geométricas y rotarlas, usando Bresenham para hacer líneas, que posteriormente construyen geometrías como rectángulos, círculos y triángulos.

## Incluye

- Dibujo de líneas mediante el algoritmo de Bresenham.
- Dibujo de círculos con el algoritmo del punto medio.
- Dibujo de rectángulos.
- Dibujo de triángulos.
- Relleno de áreas.
- Herramienta de borrador y pincel.
- Rotación de figuras mediante un control de ángulo.
- Selección de colores.
## Estructura
Grilla/
│
├── Grilla/
│   ├── external/
│   │   └── imgui/
│   │
│   └── main.cpp
│
├── librerias/
│   ├── include/
│   └── lib/
│
├── glad.c
│
├── .gitattributes
├── .gitignore
└── README.md

## Arquitectura del programa

El programa está dividido en las siguientes partes:

1. **Inicialización:** configuración de GLFW, GLAD, OpenGL e ImGui.
2. **Estructuras de datos:** definición de `Vertex` para almacenar posiciones y colores, y `Punto` para trabajar con las coordenadas de los puntos que conforman las figuras.
3. **Algoritmos de dibujo:** funciones para rellenar, dibujar líneas, círculos, rectángulos y triángulos.
4. **Transformaciones:** funciones para calcular la rotación de figuras mediante operaciones trigonométricas.
5. **Cuadrícula:** generación de una matriz de celdas mediante vectores de vértices e índices.
6. **Interfaz gráfica:** menú desarrollado con ImGui para seleccionar herramientas y colores.
7. **Renderizado:** actualización de los buffers de OpenGL y visualización de la cuadrícula.

## Funcionamiento

La cuadrícula está formada por 60 filas y 60 columnas. Cada celda contiene cuatro vértices que almacenan su posición y color.
Mediante los clics del mouse se seleccionan las coordenadas de las celdas para ejecutar las herramientas de dibujo. Los cambios se actualizan en el VBO para posteriormente visualizarse mediante OpenGL.

## Pipeline del programa
             ┌──────────────┐
             │    INICIO    │
             └──────┬───────┘
                    ▼
        ┌───────────────────────┐
        │ Inicialización OpenGL │
        │ GLFW, GLAD e ImGui    │
        └───────────┬───────────┘
                    ▼
        ┌───────────────────────┐
        │ Creación de cuadrícula│
        │       60 x 60         │
        └───────────┬───────────┘
                    ▼
        ┌───────────────────────┐
        │ Configuración VAO,    │
        │       VBO y EBO       │
        └───────────┬───────────┘
                    ▼
        ┌───────────────────────┐
        │ Captura de eventos    │◄──────────┐
        │ y selección de        │           │
        │ herramienta           │           │
        └───────────┬───────────┘           │
                    ▼                       │
        ┌───────────────────────┐           │
        │ Conversión de clics   │           │
        │ a coordenadas         │           │
        └───────────┬───────────┘           │
                    ▼                       │
        ┌───────────────────────┐           │
        │ Procesamiento de      │           │
        │ la herramienta        │           │
        └───────────┬───────────┘           │
                    ▼                       │
          ┌─────────┼─────────┐             │
          ▼         ▼         ▼             │
       FIGURAS   RELLENO   PINCEL /         │
          │         │      BORRADOR         │
          ▼         │         │             │
        LÍNEA       │         │             │
        2 clics /   │         │             │
        RECTÁNGULO  │         │             │
        2 clics /   │         │             │
        TRIÁNGULO   │         │             │
        3 clics     │         │             │
          │         │         │             │
          ▼         │         │             │
       BRESENHAM    │         │             │
       Y PUNTO      │         │             │
       MEDIO        │         │             │
          │         │         │             │
          ▼         │         │             │
       ROTACIÓN     │         │             │
       + PREVIEW    │         │             │
          │         │         │             │
          └─────────┼─────────┘             │
                    │                       │
       CÍRCULO: 2 clics                     │
       Dibujo sin rotación                  │
       ni preview                           │
                    │                       │
                    ▼                       │
        ┌───────────────────────┐           │
        │ Actualización de      │           │
        │ vértices y colores    │           │
        └───────────┬───────────┘           │
                    ▼                       │
        ┌───────────────────────┐           │
        │ Actualización del VBO │           │
        └───────────┬───────────┘           │
                    ▼                       │
        ┌───────────────────────┐           │
        │ Renderizado OpenGL    │           │
        │ e interfaz ImGui      │           │
        └───────────┬───────────┘           │
                    ▼                       │
        ┌───────────────────────┐           │
        │ ¿Cerrar ventana?      │           │
        └───────┬───────┬───────┘           │
                │       │                   │
               SÍ       NO ─────────────────┘
                │
                ▼
        ┌───────────────────────┐
        │ Liberación de recursos│
        └───────────┬───────────┘
                    ▼
                   FIN

## Funciones principales
1. Algoritmos de dibujo
- drawLine(): Dibuja líneas entre dos puntos utilizando el algoritmo de Bresenham. Determina si el desplazamiento predominante es horizontal o vertical para calcular las celdas que deben colorearse.
- drawLineH(): Implementa la variante de Bresenham para líneas con pendiente horizontal predominante.
- drawLineV(): Implementa la variante de Bresenham para líneas con pendiente vertical predominante.
- drawCircle(): Dibuja circunferencias utilizando el algoritmo del punto medio. Aprovecha la simetría de ocho octantes para calcular los puntos del círculo.
- drawRectangle(): Construye un rectángulo a partir de cuatro líneas que conectan sus esquinas. También permite aplicar rotación antes de dibujarlo.
- drawTriangle(): Construye un triángulo mediante tres líneas que conectan sus respectivos vértices.
2. Manipulación de píxeles y colores
- putPixel(): Modifica el color de una celda de la cuadrícula. Comprueba que las coordenadas estén dentro de los límites antes de realizar los cambios.
- borrar(): Restaura el color original de una celda, funcionando como herramienta de borrador.
- pincel(): Colorea una celda utilizando el color seleccionado por el usuario.
- relleno(): Implementa un algoritmo de relleno por inundación. A partir de una celda inicial, identifica las celdas vecinas que comparten el mismo color y las reemplaza por el color seleccionado.
3. Transformaciones geométricas
- rotarPunto(): Calcula la nueva posición de un punto después de aplicar una rotación alrededor de un centro determinado, utilizando operaciones trigonométricas con seno y coseno.
- rotarLinea(): Calcula el punto medio de la línea como centro de rotación, transforma sus extremos mediante rotarPunto() y vuelve a dibujarla.
- rotarTriangulo(): Calcula el centro del triángulo mediante el promedio de las coordenadas de sus tres vértices. Posteriormente, rota cada vértice y reconstruye la figura.
- Rotación de rectángulos: Se realiza directamente dentro de drawRectangle(), donde se calcula el centro de la figura, se transforman sus cuatro esquinas y se dibujan nuevamente sus lados.

Las funciones de rotación trabajan con el ángulo seleccionado mediante el slider de ImGui. Durante el proceso se utiliza una copia de los vértices originales para actualizar la vista previa en tiempo real sin acumular rotaciones.

4. Estructuras de datos
- Vertex: Almacena las coordenadas X, Y y Z de cada vértice, junto con sus componentes de color RGB.
- Punto: Almacena las coordenadas X e Y de un punto y facilita los cálculos de las transformaciones geométricas.
5. Configuración y renderizado
- framebuffer_size_callback(): Se ejecuta cuando cambia el tamaño de la ventana. Calcula un viewport cuadrado y lo centra para conservar las proporciones de la cuadrícula.
- main(): Es la función principal del programa. Inicializa las bibliotecas, configura OpenGL, genera la cuadrícula, crea los buffers, procesa los eventos del mouse, administra las herramientas de dibujo, actualiza los vértices y realiza el renderizado de la escena y la interfaz gráfica.

## Relación entre las funciones

Las funciones de dibujo utilizan putPixel() para modificar las celdas de la cuadrícula. Las figuras geométricas se construyen a partir de líneas, mientras que las transformaciones utilizan rotarPunto() para calcular nuevas coordenadas.
Finalmente, los cambios realizados sobre los vértices se envían al VBO mediante glBufferSubData(), permitiendo que OpenGL actualice la imagen mostrada en pantalla.

## Controles
- **Línea:** selecciona dos puntos para dibujar una línea.
- **Círculo:** selecciona el centro y el radio.
- **Rectángulo:** selecciona dos puntos para definir sus dimensiones.
- **Rellenar:** cambia el color de un área.
- **Borrador:** restaura el color original de las celdas.
- **Triángulo:** selecciona tres puntos para construirlo.
- **Pincel:** permite dibujar libremente sobre la cuadrícula.

Las herramientas de línea, rectángulo y triángulo permiten ajustar su rotación mediante un slider de ImGui que controla el ángulo.
## Tecnologías utilizadas
- C++
- OpenGL 3.3
- GLFW
- GLAD
- Dear ImGui

