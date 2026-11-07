#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cstddef>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cmath>
//codigo fuente del Vertex Shader
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"    ourColor = aColor;\n"
"}\0";
//codigo fuente del Fragment Shader
const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 ourColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";

struct Vertex//!
{
	GLfloat x;
	GLfloat y;
	GLfloat z;

	GLfloat r;
	GLfloat g;
	GLfloat b;
};
//opciones 2 rectangulo, 1 circulo, 0 linea
int opciones = 2;
//clic
bool primerClic = false;

int xi=0, xf=0, yi=0, yf=0;


//pintar lineas con bresenham
void drawLineH(int x0, int y0, int x1, int y1, std::vector<Vertex>& vertices, int columnas) {
	int aux=0;
	//
	if(x0>x1){
	aux=x0;
	x0=x1;
	x1=aux;
	//
	aux=y0;
	y0=y1;
	y1=aux;
	}
	//
	int dx = x1 - x0;
	int dy = y1 - y0;
	//
	int dir = (dy<0)?-1:1;
	dy= dy*dir;
	//
	int D = 2 * dy - dx;
	int y = y0;
		for (int x = x0; x <= x1; x++)
		{
		// Pintar la celda (x,y)
		int celda = y * columnas + x;
		int base = celda * 4;
		for (int i = 0; i < 4; i++)
		{
			vertices[base + i].r = 0.941f;
			vertices[base + i].g = 0.941f;
			vertices[base + i].b = 0.235f;
		}
		// Bresenham
		if (D >= 0) {
			y = y + dir;
			D = D - 2 * dx;
		}
		D = D + 2 * dy;
	}
}

void drawLineV(int x0, int y0, int x1, int y1, std::vector<Vertex>& vertices, int columnas) {
	int aux = 0;
	//
	if (y0 > y1) {
		aux = x0;
		x0 = x1;
		x1 = aux;
		//
		aux = y0;
		y0 = y1;
		y1 = aux;
	}
	//
	int dx = x1 - x0;
	int dy = y1 - y0;
	//
	int dir = (dx < 0) ? -1 : 1;
	dx = dx * dir;
	//
	int D = 2 * dx - dy;
	int x = x0;
	for (int y = y0; y <= y1; y++)
	{
		// Pintar la celda (x,y)
		int celda = y * columnas + x;
		int base = celda * 4;
		for (int i = 0; i < 4; i++)
		{
			vertices[base + i].r = 0.941f;
			vertices[base + i].g = 0.941f;
			vertices[base + i].b = 0.235f;
		}
		// Bresenham
		if (D >= 0) {
			x = x + dir;
			D = D - 2 * dy;
		}
		D = D + 2 * dx;
	}
}
void putPixel(int x, int y, std::vector<Vertex>& vertices, int filas, int columnas, float r, float g, float b) {               //putpixel() en drawcircle
	if (x < 0 || x >= columnas || y < 0 || y >= filas) {
		return;
	}
	int celda = y * columnas + x;
	int base = celda * 4;
	for (int i = 0; i < 4; i++)
	{
		vertices[base + i].r = r;
		vertices[base + i].g = g;
		vertices[base + i].b = b;
	}
}

void drawLine(int x0, int y0, int x1, int y1, std::vector<Vertex>& vertices, int columnas, int filas, float r, float g, float b) {
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int dirx = (x1 - x0 < 0) ? -1 : 1;
	int diry = (y1 - y0 < 0) ? -1 : 1;
	//horizontal
	if (dx >= dy) {
		int D = 2 * dy - dx;
		int y = y0;
		for (int x = x0; ; x+=dirx) {
			// Pintar la celda ahora con putpixel
			putPixel(x, y, vertices, filas, columnas, r, g, b);
			if (x == x1)
				break;
			// Bresenham
			if (D >= 0) {
				y = y + diry;
				D = D - 2 * dx;
			}
			D = D + 2 * dy;
		}
	}//vertical
	else {
		int D = 2 * dx - dy;
		int x = x0;
		for (int y = y0;; y+=diry) {
			// Pintar la celda ahora con putpixel
			putPixel(x, y, vertices, filas, columnas, r, g, b);
			if (y == y1)
				break;
			// Bresenham
			if (D >= 0) {
				x = x + dirx;
				D = D - 2 * dy;
			}
			D = D + 2 * dx;
		}
	}
}
/*void putPixwl(int x, int y, std::vector<Vertex>& vertices, int columnas) {

	int celda = y * columnas + x;
	int base = celda * 4;

	for (int i = 0; i < 4; i++)
	{
		vertices[base + i].r = 0.941f;
		vertices[base + i].g = 0.941f;
		vertices[base + i].b = 0.235f;
	}
}*/

void drawRectangle (int xi, int yi, int xf, int yf,std::vector<Vertex>& vertices, int columnas, int filas, float r, float g, float b) {
	int xMin, yMin, xMax, yMax;
	if (xi<xf) {
		xMin = xi;
		xMax = xf;
	}
	else {
		xMin = xf;
		xMax = xi;
	}
	if (yi < yf) {
		yMin = yi;
		yMax = yf;
	}
	else {
		yMin = yf;
		yMax = yi;
	}
	//esquinas
	drawLine(xMin, yMin, xMin, yMax, vertices, columnas, filas, r, g, b);//iz
	drawLine(xMin, yMax, xMax, yMax, vertices, columnas, filas, r, g, b);//;arr
	drawLine(xMax, yMax, xMax, yMin, vertices, columnas, filas, r, g, b);//der
	drawLine(xMin, yMin, xMax, yMin, vertices, columnas, filas, r, g, b);//ab

}

void drawCircle(int cx, int cy, int radio, std::vector<Vertex>& vertices, int columnas, int filas, float r, float g, float b) {
	int x = 0, y = -radio, D=-radio;
	while (x<-y) {
		//
		if (D > 0) {
			y += 1;
			D += 2 * (x + y) + 1;
		}
		else {
			D += 2 * x + 1;
		}
			putPixel(cx + x, cy + y, vertices, columnas, filas, r, g, b);
			putPixel(cx + x, cy - y, vertices, columnas, filas, r, g, b);
			putPixel(cx - x, cy + y, vertices, columnas, filas, r, g, b);
			putPixel(cx - x, cy - y, vertices, columnas, filas, r, g, b);
			putPixel(cx + y, cy + x, vertices, columnas, filas, r, g, b);
			putPixel(cx - y, cy + x, vertices, columnas, filas, r, g, b);
			putPixel(cx + y, cy - x, vertices, columnas, filas, r, g, b);
			putPixel(cx - y, cy - x, vertices, columnas, filas, r, g, b);
			x += 1;
		
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
	
	bool clicMouse=false;

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
	//inicializa DIG
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);//crea un objeto de opengl para se run vertex shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	//se borran los objetos temporales que se usaron para construir el shaderProgram
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	int columnas = 40;
	int filas = 40;

	std::vector<Vertex> vertices;//!
	std::vector<GLuint> indices;//!
	//std::vector<Vertex> lineVertices;//!
	//indices
	for (int fila = 0; fila < filas; fila++)
	{
		for (int columna = 0; columna < columnas; columna++)
		{
			float separacion = 0.002f;

			float x0 = -1.0f + 2.0f * columna / (float)columnas + separacion;
			float x1 = -1.0f + 2.0f * (columna + 1) / (float)columnas - separacion;

			float y0 = 1.0f - 2.0f * fila / (float)filas - separacion;
			float y1 = 1.0f - 2.0f * (fila + 1) / (float)filas + separacion;

			GLuint base = static_cast<GLuint>(vertices.size());

			// 4 vertices del cuadrado
			vertices.push_back({ x0, y1, 0.0f, 0.212f, 0.278f, 0.463f });
			vertices.push_back({ x1, y1, 0.0f, 0.212f, 0.278f, 0.463f });
			vertices.push_back({ x1, y0, 0.0f, 0.212f, 0.278f, 0.463f });
			vertices.push_back({ x0, y0, 0.0f, 0.212f, 0.278f, 0.463f });

			//Primer triángulo
			indices.push_back(base + 0);
			indices.push_back(base + 1);
			indices.push_back(base + 2);

			//Segundo triángulo
			indices.push_back(base + 0);
			indices.push_back(base + 2);
			indices.push_back(base + 3);
		}
	}
	//int cx=20, cy=20, r=19;
	
	//drawCircle(cx, cy, r, vertices, columnas, filas);
	
	//int x0 = 20, y0 = 20, x1 = 10, y1 = 35;
	//int dx=x1-x0 , dy =y1-y0;
	/*if (abs(dx) > abs(dy)) {
		drawLineH(x0, y0, x1, y1, vertices, columnas);
	}else if(abs(dx)<=abs(dy)){
	drawLineV(x0, y0, x1, y1, vertices, columnas);*/
	GLuint VAO, VBO, EBO;
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
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(),GL_DYNAMIC_DRAW);
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
	//posicion
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));//location que atributo es, size cuantos valores, type, stride cuanto ocupa cada fila, offset donde empieza
	glEnableVertexAttribArray(0);//activa el atributo numero 0
	//color
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, r));
	glEnableVertexAttribArray(1);
	//deja de seleccionar los VBO y VAO, practica para evitar modificaciones accidentales
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glClearColor(0.204f, 0.216f, 0.251f, 1.0f);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		//detecta el clic del mouse
		bool clic = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS;
		if (clic && !clicMouse && !ImGui::GetIO().WantCaptureMouse) {
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
		//conversion
		int celdaX = (int)(mouseX / (800.0/columnas));
		int celdaY = (int)(mouseY / (800.0/filas));
		if(!primerClic){
			xi = celdaX;
			yi = celdaY;
			primerClic = true;
			std::cout << "Primer Clic: " << xi << ", " << yi << std::endl;
		}
		else {
			xf = celdaX;
			yf = celdaY;
			primerClic = false;
			std::cout << "Segundo Clic: " << xf << ", "<< yf << std::endl;
			if (opciones == 0) {
				drawLine(xi, yi, xf, yf, vertices, columnas, filas, 0.941f, 0.941f, 0.235f);
			}
			else if (opciones == 1) {
				int dx = xf - xi;
				int dy = yf - yi;
				int radio = (int)(std::round(std::sqrt((dx * dx) + (dy * dy))));
				drawCircle(xi, yi, radio, vertices, columnas, filas, 0.941f, 0.941f, 0.235f);
			}
			else if (opciones == 2) {
				drawRectangle(xi, yi, xf, yf, vertices, columnas, filas, 0.941f, 0.941f, 0.235f);
			}
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER,0,vertices.size() * sizeof(Vertex),vertices.data());
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		}
		clicMouse = clic;

		//Inicia nuevo frame de ImGui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//propiedades
		ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(800,75), ImGuiCond_Always);


		//Crea ventana de ImGui
		ImGui::Begin("Herramientas");

		ImGui::Text("Seleccionar herramienta:");
		ImGui::SameLine();
		if (ImGui::Button("Linea"))
		{
			opciones = 0;
		}ImGui::SameLine();
		if (ImGui::Button("Circulo"))
		{
			opciones = 1;
		}ImGui::SameLine();
		if (ImGui::Button("Rectangulo"))
		{
			opciones = 2;
		}ImGui::SameLine();
		if (ImGui::Button("Rellenar"))
		{
			opciones = 3;
		}ImGui::SameLine();
		if (ImGui::Button("Borrador"))
		{
			opciones = 4;
		}
		ImGui::Separator();
		ImGui::Text("Herramienta actual:");
		ImGui::SameLine();
		if (opciones == 0) {
			ImGui::Text("Línea");
		}else if (opciones == 1) {
			ImGui::Text("Círculo");
		}
		else if (opciones == 2) {
			ImGui::Text("Rectángulo");
		}
		else if (opciones == 3) {
			ImGui::Text("Rellenar");
		}
		else if (opciones == 4) {
			ImGui::Text("Borrador");
		}
		ImGui::End();

		//Dibuja escena OpenGL
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,reinterpret_cast<void*>(0));

		//Renderiza la interfaz de ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}	