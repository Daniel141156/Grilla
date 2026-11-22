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

struct Vertex{
	GLfloat x;
	GLfloat y;
	GLfloat z;

	GLfloat r;
	GLfloat g;
	GLfloat b;
};
//datos rotacion
struct Punto {
	float x;
	float y;
};
float angulo= 0.0;
bool rotar = false;
bool segundoPunto = false;
Punto rotarPunto(float x, float y, float cx, float cy, float angulo) {
	float rad = (angulo * 3.14159265) / 180.0;
	float dx = x - cx;
	float dy = y - cy;

	return {
		cx + dx * cos(rad) - dy * sin(rad),
		cy + dx * sin(rad) + dy * cos(rad)
	};
}

int opciones = 0; //opciones 2 rectangulo, 1 circulo, 0 linea
bool  mostrarMenu = true;
bool primerClic = false; //clic

int xi=0, xf=0, yi=0, yf=0, x2=0, y2=0;

float colorDibujo[3] = { 0.941f ,0.941f , 0.235f};
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
		vertices[base + i].r = colorDibujo[0];
		vertices[base + i].g = colorDibujo[1];
		vertices[base + i].b = colorDibujo[2];
	}
}
void borrar(int x, int y, std::vector<Vertex>& vertices, int filas, int columnas) {
	if (x < 0 || x >= columnas || y < 0 || y >= filas) {
		return;
	}
	int base = (y * columnas + x) * 4;
	for (int i = 0; i < 4; i++)
	{
		vertices[base + i].r = 0.212f;
		vertices[base + i].g = 0.278f;
		vertices[base + i].b = 0.463f;
	}
}
void pincel(int x, int y, std::vector<Vertex>& vertices, int filas, int columnas) {
	if (x < 0 || x >= columnas || y < 0 || y >= filas) {
		return;
	}
	int base = (y * columnas + x) * 4;
	for (int i = 0; i < 4; i++)
	{
		vertices[base + i].r = colorDibujo[0];
		vertices[base + i].g = colorDibujo[1];
		vertices[base + i].b = colorDibujo[2];
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

void rotarLinea(int x0, int y0, int x1, int y1, std::vector<Vertex>& vertices, int columnas, int filas, float r, float g, float b, float angulo) {
	float cx = (x1 + x0) / 2.0 ;
	float cy = (y1 + y0) / 2.0;
	Punto p1 = rotarPunto(x0, y0, cx, cy, angulo);
	Punto p2 = rotarPunto(x1, y1, cx, cy, angulo); 
	drawLine(p1.x, p1.y, p2.x, p2.y, vertices, columnas, filas, r, g, b);
}

void drawRectangle (int xi, int yi, int xf, int yf,std::vector<Vertex>& vertices, int columnas, int filas, float r, float g, float b, float angulo = 0.0) {
	float cx = (xi+xf)/2 ;
	float cy = (yi+yf)/2 ;

	Punto esquinas[4] = {
	   {(float)xi, (float)yi},
	   {(float)xf, (float)yi},
	   {(float)xf, (float)yf},
	   {(float)xi, (float)yf}
	};
	for (int i = 0; i < 4; i++) {
		esquinas[i] = rotarPunto(esquinas[i].x, esquinas[i].y, cx, cy, angulo);
	}
	for (int i = 0; i < 4; i++) {
		int sig = (i + 1) % 4;

		drawLine((esquinas[i].x), round(esquinas[i].y), round(esquinas[sig].x), round(esquinas[sig].y), vertices, columnas, filas, r, g, b);
	}
	/*int xMin, yMin, xMax, yMax;

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
	*/

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

void relleno(int x, int y, std::vector<Vertex>& vertices, int columnas, int filas, float r, float g, float b) {
	
	if (x < 0 || x >= columnas || y < 0 || y >= filas) {
		return;
	}
	int pos = (y * columnas + x) * 4;

	if (pos + 3 >= vertices.size()) {
		return;
	}

	

	float rActual = vertices[pos].r;
	float gActual = vertices[pos].g;
	float bActual = vertices[pos].b;
	//se guarda antes de cambiar el valor

	if (rActual == r && gActual == g && bActual == b)
		return;

	std::vector<std::pair<int, int>> pendientes;//////////////////
	pendientes.push_back({ x, y });

	while (!pendientes.empty())////
	{
		x = pendientes.back().first;
		y = pendientes.back().second;
		pendientes.pop_back();

		pos = (y * columnas + x) * 4;

		if (vertices[pos].r == rActual &&vertices[pos].g == gActual &&vertices[pos].b == bActual){
			putPixel(x, y, vertices, filas, columnas, r, g, b);

			if (x + 1 < columnas) pendientes.push_back({ x + 1, y });
			if (x - 1 >= 0) pendientes.push_back({ x - 1, y });
			if (y + 1 < filas) pendientes.push_back({ x, y + 1 });
			if (y - 1 >= 0) pendientes.push_back({ x, y - 1 });
		}
	}

}

void drawTriangle(int x0, int y0, int x1, int y1,int x2, int y2, std::vector<Vertex>& vertices,int columnas, int filas, float r, float g, float b) {
	drawLine(x0, y0, x1, y1, vertices, columnas, filas, r, g, b);
	drawLine(x1, y1, x2, y2, vertices, columnas, filas, r, g, b);
	drawLine(x2, y2, x0, y0, vertices, columnas, filas, r, g, b);
}
void rotarTriangulo(int x0, int y0, int x1, int y1,int x2, int y2, std::vector<Vertex>& vertices,int columnas, int filas, float r, float g, float b,float angulo) {
	float cx = (x0 + x1 + x2) / 3.0f;
	float cy = (y0 + y1 + y2) / 3.0f;
	Punto p0 = rotarPunto(x0, y0, cx, cy, angulo);
	Punto p1 = rotarPunto(x1, y1, cx, cy, angulo);
	Punto p2 = rotarPunto(x2, y2, cx, cy, angulo);
	drawTriangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, vertices, columnas, filas, r, g, b);
}

int viewportX = 0;
int viewportY = 0;
int viewportSize = 800;


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	viewportSize = (width<height)? width:height;
	//centros
	viewportX = (width - viewportSize) / 2;
	viewportY = (height - viewportSize) / 2;

	glViewport(viewportX, viewportY, viewportSize, viewportSize);
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

	int columnas = 60;
	int filas = 60;

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
	std::vector<Vertex> verticesOriginales = vertices;//copia de la cuadricula
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
		//borrador
		if ((opciones == 4 || opciones == 6 ) && clic && !ImGui::GetIO().WantCaptureMouse) {
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			int celdaX = (int)((mouseX - viewportX) * columnas / viewportSize);
			int celdaY = (int)((mouseY - viewportY) * filas / viewportSize);

			if (celdaX >= 0 && celdaX < columnas &&
				celdaY >= 0 && celdaY < filas) {
				if (opciones == 4 ) {
				borrar(celdaX, celdaY, vertices, filas, columnas);
			}
			else if (opciones == 6 ) {
			pincel(celdaX, celdaY, vertices, filas, columnas);
			}
			verticesOriginales = vertices;
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
			glBindBuffer(GL_ARRAY_BUFFER, 0);

		}
		}


		//herramientas clic
		if (clic && !clicMouse && !ImGui::GetIO().WantCaptureMouse && opciones != 4 && opciones != 6) {
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
		//conversion
		int celdaX = (int)((mouseX - viewportX) * columnas/viewportSize);
		int celdaY = (int)((mouseY - viewportY) * filas /viewportSize);

		if (celdaX < 0 || celdaX >= columnas ||celdaY < 0 || celdaY >= filas) {
			clicMouse = clic;
			continue;
		}
		if (opciones == 3) {
			relleno(celdaX, celdaY, vertices, columnas, filas, colorDibujo[0], colorDibujo[1], colorDibujo[2]);
			verticesOriginales = vertices;//cambia el fondo de la grilla original
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0,vertices.size() * sizeof(Vertex), vertices.data());
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else if (segundoPunto) {
			x2 = celdaX;
			y2 = celdaY;
			segundoPunto = false;
			rotar = true;
			angulo = 0.0f;
		}
		else if (primerClic) {
			xf = celdaX;
			yf = celdaY;
			primerClic = false;
			if (opciones == 5) {
				segundoPunto = true;
			}
			else if (opciones ==1 ) {
				int dx = xf - xi;
				int dy = yf - yi;
				int radio = (int)(std::round(std::sqrt((dx * dx) + (dy * dy))));
				drawCircle(xi, yi, radio, vertices, columnas, filas, 0.941f, 0.941f, 0.235f);
				verticesOriginales = vertices;
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			else {
				rotar = true;
				angulo = 0.0;
			}
		}
		else if(rotar){
			rotar = false;
			vertices = verticesOriginales;

			if (opciones == 0) {
				rotarLinea(xi, yi, xf, yf, vertices, columnas, filas, colorDibujo[0], colorDibujo[1], colorDibujo[2], angulo);
			}
			else if (opciones == 2) {
				drawRectangle(xi, yi, xf, yf, vertices, columnas, filas, 0.941f, 0.941f, 0.235f,angulo);
			}
			else if (opciones == 5) {
				rotarTriangulo(xi, yi, xf, yf, x2, y2, vertices, columnas, filas, 0.941f, 0.941f, 0.235f, angulo);
			}
			verticesOriginales = vertices;

		}
		else {
			// Primer clic
			xi = celdaX;
			yi = celdaY;
			primerClic = true;
		}
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		
		clicMouse = clic;

		//Inicia nuevo frame de ImGui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//propiedades
		ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);
		ImGuiWindowFlags flag = ImGuiWindowFlags_NoScrollbar;
		if (!rotar) {
			ImGui::SetNextWindowSize(ImVec2(650, 75), ImGuiCond_Always);
		}
		else if (rotar) {
			ImGui::SetNextWindowSize(ImVec2(650, 150), ImGuiCond_Always);
		}
		//Crea ventana de ImGui
		if (mostrarMenu) {
			ImGui::Begin("Herramientas", nullptr, flag);
			ImGui::SameLine();
			if (ImGui::Button("Linea"))
			{
				opciones = 0;
				primerClic = false;
				segundoPunto = false;
				rotar = false;
			}ImGui::SameLine();
			if (ImGui::Button("Circulo"))
			{
				opciones = 1;
				primerClic = false;
				segundoPunto = false;
				rotar = false;
			}ImGui::SameLine();
			if (ImGui::Button("Rectangulo"))
			{
				opciones = 2;
				primerClic = false;
				segundoPunto = false;
				rotar = false;
			}ImGui::SameLine();
			if (ImGui::Button("Rellenar"))
			{
				opciones = 3;
				primerClic = false;
				segundoPunto = false;
				rotar = false;
			}ImGui::SameLine();
			if (ImGui::Button("Borrador"))
			{
				opciones = 4;
				primerClic = false;
				segundoPunto = false;
				rotar = false;
			}ImGui::SameLine();
			if (ImGui::Button("Triángulo"))
			{
				opciones = 5;
				primerClic = false;
				segundoPunto = false;
				rotar = false;
			}ImGui::SameLine();
			if (ImGui::Button("Pincel"))
			{
				opciones = 6;
				primerClic = false;
				segundoPunto = false;
				rotar = false;
			}
			ImGui::Separator();
			ImGui::Text("Herramienta actual: ");
			ImGui::SameLine();
			if (opciones == 0) {
				ImGui::Text("Línea");
			}
			else if (opciones == 1) {
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
			else if (opciones == 5) {
				ImGui::Text("Triángulo");
			}
			else if (opciones == 6) {
				ImGui::Text("Pincel");
			}
			if (rotar && (opciones == 0 || opciones == 2 || opciones == 5)) {
				ImGui::Separator();
				ImGui::Text("Rotación: ");
				ImGui::SliderFloat("Ángulo", &angulo, 0.0, 360.0);
				ImGui::Text("Click en la grilla para confirmar.");
			}
			ImGui::End();
			ImGui::SetNextWindowPos(ImVec2(650, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(150, 75), ImGuiCond_Always);
			ImGui::Begin("Color", nullptr, flag);
			ImGui::Text("Seleccionar Color:");
			ImGui::Separator();
			ImGui::ColorEdit3("", colorDibujo);
			ImGui::End();
		}
		float ancho = 125.0;
		float alto = 35.0 ;
		float posX = viewportX + (viewportSize - ancho) / 2.0;
		float posY = viewportY + viewportSize - alto;
		ImGui::SetNextWindowPos(ImVec2(posX, posY), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(ancho, alto), ImGuiCond_Always);
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;
		ImGui::Begin("Opciones adicionales", nullptr, flags);
		if (mostrarMenu == true) {
			if (ImGui::Button("Ocultar")) {
				mostrarMenu = false;
			}
		}
		else if (mostrarMenu == false) {
			if (ImGui::Button("Mostrar")) {
				mostrarMenu = true;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("salir")) {
			glfwSetWindowShouldClose(window, true);
		}
		ImGui::End();
		//Dibuja escena OpenGL
		if (rotar && (opciones == 0 || opciones == 2 || opciones == 5 )) {
			vertices = verticesOriginales;
			if (opciones == 0) {
				rotarLinea(xi, yi, xf, yf,vertices, columnas, filas,colorDibujo[0], colorDibujo[1],colorDibujo[2], angulo);

			}
			else if (opciones==2) {
				drawRectangle(xi, yi, xf, yf, vertices, columnas, filas, colorDibujo[0], colorDibujo[1], colorDibujo[2], angulo);
			}
			else if (opciones == 5) {
				rotarTriangulo(xi, yi, xf, yf, x2, y2, vertices, columnas, filas, colorDibujo[0], colorDibujo[1], colorDibujo[2], angulo);
			}
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			
		}
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,reinterpret_cast<void*>(0));

		//Renderiza la interfaz de ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}	