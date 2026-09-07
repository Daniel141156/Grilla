#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

//codigo fuente del Vertex Shader
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//codigo fuente del Fragment Shader
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";

struct Vertex//!
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

void generarGrilla(int columnas, int filas,std::vector<Vertex>& lineVertices){
	// Líneas verticales
	for (int columna = 0; columna <= columnas; columna++)
	{
		float x = -1.0f + 2.0f * columna / (float)columnas;

		lineVertices.push_back({ x, -1.0f, 0.0f });
		lineVertices.push_back({ x,  1.0f, 0.0f });
	}

	// Líneas horizontales
	for (int fila = 0; fila <= filas; fila++)
	{
		float y = -1.0f + 2.0f * fila / (float)filas;

		lineVertices.push_back({ -1.0f, y, 0.0f });
		lineVertices.push_back({ 1.0f, y, 0.0f });
	}
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
int main() {
	glfwInit();
	//contexto
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "Grilla", NULL, NULL);//creacion de la ventana (Ancho,Alto,Nombre,...,...)

	if (window == nullptr)
	{
		std::cout << "Fallo en la creacion de la ventana" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, 800, 800);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);//crea un objeto de opengl para se run vertex shader
	glShaderSource(vertexShader,1,&vertexShaderSource,nullptr);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader,1,&fragmentShaderSource,nullptr);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	//se borran los objetos temporales que se usaron para construir el shaderProgram
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	int columnas = 80;
	int filas = 80;

	std::vector<Vertex> vertices;//!
	std::vector<GLuint> indices;//!
	std::vector<Vertex> lineVertices;//!
	for (int fila = 0; fila < filas; fila++)
	{
		for (int columna = 0; columna < columnas; columna++)
		{
			float x0 = -1.0f + 2.0f * columna / (float)columnas;
			float x1 = -1.0f + 2.0f * (columna + 1) / (float)columnas;

			float y0 = -1.0f + 2.0f * fila / (float)filas;
			float y1 = -1.0f + 2.0f * (fila + 1) / (float)filas;

			GLuint base = static_cast<GLuint>(vertices.size());

			vertices.push_back({ x0, y1, 0.0f });
			vertices.push_back({ x1, y1, 0.0f });
			vertices.push_back({ x1, y0, 0.0f });
			vertices.push_back({ x0, y0, 0.0f });

			// borde superior
			indices.push_back(base + 0);
			indices.push_back(base + 1);

			// borde derecho
			indices.push_back(base + 1);
			indices.push_back(base + 2);

			// borde inferior
			indices.push_back(base + 2);
			indices.push_back(base + 3);

			// borde izquierdo
			indices.push_back(base + 3);
			indices.push_back(base + 0);

		}
	}
	//generarGrilla(columnas, filas, lineVertices);
	/*
	GLfloat vertices[] = {
		//x, y, z(0)
		-1.0f, 1.0f, 0.0f, // primer punto, de izquierda a derecha, -> de arriba a abajo INDICE 0
		0.0f, 1.0f, 0.0f, //1
		1.0f, 1.0f, 0.0f, //2
		//fila 2
		-1.0f, 0.0f, 0.0f, //3
		0.0f, 0.0f, 0.0f, //4
		1.0f, 0.0f, 0.0f, //5
		//fila 3
		-1.0f, -1.0f, 0.0f, //6
		0.0f, -1.0f, 0.0f, //7
		1.0f, -1.0f, 0.0f //8

	};
	GLuint indices[] = {//6 indices por cuadrado, en un 2x2 serian 6indices*4cuadrados, 24 indices
		//cuadrados, de izquierda a derecha, -> de arriba a abajo, se forman de dos triángulos, 3 indices cada uno, cuadrado 1
		0, 3, 4,
		0, 1, 4,
		//2
		1, 4, 5,
		1, 2, 5,
		//3
		3, 6, 7,
		3, 4, 7,
		//4
		4, 7, 8,
		4, 5, 8
	};*/
	GLuint VAO, VBO, EBO, lineasVAO, lineasVBO;
	//el 1 significa que queremos generar un objeto, & guarda el identificador que genere opengl en la variable, en este caso las variables son VAO, VBO, EBO.
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	//
	//glbind es como decirle a opengl que este va a ser el objeto que se utilize actualmente, si tubiera varios VAO, VAO1, VAO2, ... podria usar el VAO2 con glBindVertexArray(VAO2);
	glBindVertexArray(VAO);
	//array buffer indica que tipo de buffer se esta seleccionando, el buffer (la variable) que queremos utilizar viene del genbuffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//array buffer, estamos llenando el buffer de vertices,
	// sizeof() cuantos bytes vamos a copiar (vertices) calcula el tamaño total del array
	//vertices, es de donde vienen los datos
	//static draw, es una forma de decir como vamos a utilizar los datos,
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(),GL_STATIC_DRAW);
	//lo mismo con el EBO, element array buffer viuene del mismo EBO, element buffer object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(),GL_STATIC_DRAW);
	
	//indica como leer los datos del VBO,  0 es el location, coincide con el vertexshadersource
	//3 porque son 3 valores de posicion xyz
	//GL float porque son valores float
	//normalizacion como pj cuando valores de color rgb de 0 a 255 se quieren convertir a 0=0.0 y 255 = 1.0, en este caso no se usa pq usamos directamente float
	
	//3*sizeof(float) para pasar de un vertice a otro, lo hace con el tamaño de los bytes
	// cada vertice ocupa 3 float cada float son 4 bytes, salta 12 bytes, el size of da el tamaño directamente 
	
	//(void*)0); indica donde comienza el atributo de cada vertice, los atributos pueden ser, posicion, color, normal, coordenadas de textura, en este caso
	//la posición empieza en el byte 0 
	
	// en este caso location=0 indica el atributo de posicion, el atributo 0
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(GLfloat),(void*)0);//location que atributo es, size cuantos valores, type, stride cuanto ocupa cada fila, offset donde empieza
	glEnableVertexAttribArray(0);//activa el atributo numero 0
	//deja de seleccionar los VBO y VAO, practica para evitar modificaciones accidentales
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &lineasVAO);
	glGenBuffers(1, &lineasVBO);
	glBindVertexArray(lineasVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lineasVBO);
	glBufferData(GL_ARRAY_BUFFER,lineVertices.size() * sizeof(Vertex),lineVertices.data(),GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(GLfloat),(void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		//para dibujar, gltriangles interpreta indices de 3 en 3 para formar triangulos, 24 cantidad de indices, undigned int pq los indices son gluint, 0 desde la posicion del EBO que se va a leer, 
		//pero es la orden final
		//cuadrados
		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
		//glDrawElements(GL_TRIANGLES, /*24*/ indices.size(), GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
		//lineas de la grilla
		//glBindVertexArray(lineasVAO);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &lineasVAO);
	glDeleteBuffers(1, &lineasVBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}