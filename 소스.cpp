#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "InitShader.h"
#include "mat.h"
#include "vec.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

unsigned int color0 = 0;
unsigned int color1 = 1;
unsigned int color2 = 2;
unsigned int color3 = 3;
unsigned int gray = 4;

float size = 0.5;

int right_click = 0;
float anglex = 0.0;
float angley = 0.0;
float anglez = 0.0;

int NumSubdivides = 3;
int NumTriangles;
int NumVertices;

vec3* points;
vec3* colors;
int index = 0;

GLuint vao;
GLuint buffer;
GLuint simpleProgram;
mat4 rotation;

void init();
void display();
void idle();
void divide_triangle(vec3 a, vec3 b, vec3 c, unsigned int cnt, unsigned int coli);
void triangle(vec3 a, vec3 b, vec3 c, unsigned int coli);

void setGasketVertices(unsigned int col1, unsigned int col2, unsigned int col3, unsigned int col4);
void gasketSetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void gasketSetMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

vec3 baseVertices[] =
{
   vec3(0.0f, 0.0f, 1.0f),
   vec3(0.0f, 0.942809f, -0.333333f),
   vec3(-0.816497f, -0.471405f, -0.333333f),
   vec3(0.816497f, -0.471405f, -0.333333f)
};

vec3 baseColors[] =
{
   vec3(1.0f, 0.0f, 0.0f),
   vec3(0.0f, 1.0f, 0.0f),
   vec3(0.0f, 0.0f, 1.0f),
   vec3(1.0f, 0.0f, 1.0f),
   vec3(0.5f, 0.5f, 0.5f)
};

int main()
{
	GLFWwindow* window;
	if (!glfwInit())
	{
		return -1;
	}

	window = glfwCreateWindow(512, 512, "HW2", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error\n";
	}

	init();

	glfwSetKeyCallback(window, gasketSetKeyCallback);
	glfwSetMouseButtonCallback(window, gasketSetMouseButtonCallback);

	while (!glfwWindowShouldClose(window))
	{
		display();
		idle();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	free(points);
	free(colors);
	return 0;
}

void init()
{
	NumTriangles = 4 * pow(4.0f, NumSubdivides); //회색 삼각형을 위해 3.0f에서 4.0f으로 변경
	NumVertices = 3 * NumTriangles;

	points = (vec3*)malloc(sizeof(vec3)*NumVertices);
	colors = (vec3*)malloc(sizeof(vec3)*NumVertices);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vec3)*NumVertices, NULL, GL_STATIC_DRAW);

	setGasketVertices(color0, color1, color2, color3);

	simpleProgram = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(simpleProgram);

	GLuint vPosition = glGetAttribLocation(simpleProgram, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(simpleProgram, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec3)*NumVertices));
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glUseProgram(simpleProgram);
	rotation = RotateX(anglex)*RotateY(angley)*RotateZ(anglez);
	mat4 scaling = Scale(size);
	GLuint uMat = glGetUniformLocation(simpleProgram, "uMat");
	glUniformMatrix4fv(uMat, 1, GL_FALSE, rotation);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

void idle()
{
	if (right_click%4 == 0)
	{
		anglex = 0.0f;
		angley = 0.0f;
		anglez = 0.0f;
	}
	else if (right_click % 4 == 1)//z
	{
		anglex = 0.0f;
		angley = 0.0f;
		anglez = anglez + 0.1f;
	}
	else if (right_click%4 == 2)//x
	{
		anglex = anglex + 0.1f;
		angley = 0.0f;
		anglez = 0.0f;
	}
	else if (right_click%4 == 3)//y
	{
		anglex = 0.0f;
		angley = angley + 0.1f;
		anglez = 0.0f;
	}
}

void divide_triangle(vec3 a, vec3 b, vec3 c, unsigned int cnt, unsigned int coli)
{
	if (cnt > 0)
	{
		vec3 ab = (a + b) / 2.0f;
		vec3 ac = (a + c) / 2.0f;
		vec3 bc = (b + c) / 2.0f;

		divide_triangle(a, ab, ac, cnt - 1, coli);
		divide_triangle(c, ac, bc, cnt - 1, coli);
		divide_triangle(b, bc, ab, cnt - 1, coli);

		triangle(ab, bc, ac, gray);
	}
	else
	{
		triangle(a, b, c, coli);
	}
}

void triangle(vec3 a, vec3 b, vec3 c, unsigned int coli)
{
	points[index] = a;
	colors[index++] = baseColors[coli];
	points[index] = b;
	colors[index++] = baseColors[coli];
	points[index] = c;
	colors[index++] = baseColors[coli];
}

void gasketSetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		std::cout << "Key 1 is pressed\n";

		color0 = 0;
		color1 = 1;
		color2 = 2;
		color3 = 3;

		setGasketVertices(color0, color1, color2, color3);
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		std::cout << "Key 2 is pressed\n";

		color0 = 1;
		color1 = 2;
		color2 = 3;
		color3 = 0;

		setGasketVertices(color0, color1, color2, color3);
	}
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		std::cout << "Key 3 is pressed\n";

		color0 = 2;
		color1 = 3;
		color2 = 0;
		color3 = 1;

		setGasketVertices(color0, color1, color2, color3);
	}
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		std::cout << "Key 4 is pressed\n";

		color0 = 3;
		color1 = 0;
		color2 = 1;
		color3 = 2;

		setGasketVertices(color0, color1, color2, color3);
	}
	///////////////////////////////////////////////////////////////////////////
	else if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		std::cout << "Key Q is pressed\n";

		glfwTerminate();
	}
	///////////////////////////////////////////////////////////////////////////
	else if (key == GLFW_KEY_U && action == GLFW_PRESS)
	{
		std::cout << "Key U is pressed\n";

		NumSubdivides++;
		if (NumSubdivides > 10) {
			NumSubdivides = 10;
		}
		init();
		setGasketVertices(color0, color1, color2, color3);
	}
	else if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		std::cout << "Key D is pressed\n";

		NumSubdivides--;
		if (NumSubdivides < 0) {
			NumSubdivides = 0;
		}
		init();
		setGasketVertices(color0, color1, color2, color3);
	}
	///////////////////////////////////////////////////////////////////////////
	else if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
	{
		std::cout << "Key + is pressed\n";

		size = size + 0.1;
		init();
		setGasketVertices(color0, color1, color2, color3);
	}
	else if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
	{
		std::cout << "Key - is pressed\n";

		size = size - 0.1;
		if (size < 0) {
			size = 0;
		}
		init();
		setGasketVertices(color0, color1, color2, color3);
	}
	///////////////////////////////////////////////////////////////////////////
}

void gasketSetMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		std::cout << "Left button is pressed\n";

		color0 = (color0 + 1) % 4;
		color1 = (color1 + 1) % 4;
		color2 = (color2 + 1) % 4;
		color3 = (color3 + 1) % 4;

		setGasketVertices(color0, color1, color2, color3);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		std::cout << "Right button is pressed\n";
		right_click++;

		init();
		setGasketVertices(color0, color1, color2, color3);
	}
}

void setGasketVertices(unsigned int col1, unsigned int col2, unsigned int col3, unsigned int col4)
{
	index = 0;

	// set gasket vertices
	divide_triangle(size*baseVertices[0], size*baseVertices[1], size*baseVertices[2], NumSubdivides, col1);
	divide_triangle(size*baseVertices[3], size*baseVertices[2], size*baseVertices[1], NumSubdivides, col2);
	divide_triangle(size*baseVertices[0], size*baseVertices[3], size*baseVertices[1], NumSubdivides, col3);
	divide_triangle(size*baseVertices[0], size*baseVertices[2], size*baseVertices[3], NumSubdivides, col4);

	// bufferdata
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3)*NumVertices, points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec3)*NumVertices, sizeof(vec3)*NumVertices, colors);
}