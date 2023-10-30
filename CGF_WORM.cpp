#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace std;
using namespace glm;

GLuint shader;
GLuint cubeVAO, cubeVBO;

string vsh = R"(
    #version 330 core
    layout(location = 0) in vec3 position;

    void main() {
        gl_Position = vec4(position, 1.0);
    }
)";


string fsh = R"(
	#version 330 core
	layout(location = 0) out vec4 color;
	void main() {
		color = vec4(.3, 0.8, 0.01, 1.0);
	}
)";

vector<float> getCubeVertices(float size, vec3 position = vec3(0)) {
	float halfSize = (size / 2.0f);

	vector<float> cube_vertices = {
		-halfSize + position.x, -halfSize + position.y, halfSize + position.z,
		halfSize + position.x, -halfSize + position.y, halfSize + position.z,
		halfSize + position.x, halfSize + position.y, halfSize + position.z,
		-halfSize + position.x, halfSize + position.y, halfSize + position.z,

		-halfSize + position.x, -halfSize + position.y, -halfSize + position.z,
		halfSize + position.x, -halfSize + position.y, -halfSize + position.z,
		halfSize + position.x, halfSize + position.y, -halfSize + position.z,
		-halfSize + position.x, halfSize + position.y, -halfSize + position.z,

		-halfSize + position.x, halfSize + position.y, halfSize + position.z,
		halfSize + position.x, halfSize + position.y, halfSize + position.z,
		halfSize + position.x, halfSize + position.y, -halfSize + position.z,
		-halfSize + position.x, halfSize + position.y, -halfSize + position.z,

		-halfSize + position.x, -halfSize + position.y, halfSize + position.z,
		halfSize + position.x, -halfSize + position.y, halfSize + position.z,
		halfSize + position.x, -halfSize + position.y, -halfSize + position.z,
		-halfSize + position.x, -halfSize + position.y, -halfSize + position.z,

		halfSize + position.x, -halfSize + position.y, halfSize + position.z,
		halfSize + position.x, -halfSize + position.y, -halfSize + position.z,
		halfSize + position.x, halfSize + position.y, -halfSize + position.z,
		halfSize + position.x, halfSize + position.y, halfSize + position.z,

		-halfSize + position.x, -halfSize + position.y, halfSize + position.z,
		-halfSize + position.x, -halfSize + position.y, -halfSize + position.z,
		-halfSize + position.x, halfSize + position.y, -halfSize + position.z,
		-halfSize + position.x, halfSize + position.y, halfSize + position.z,
	};

	return cube_vertices;
}

static unsigned int CompileShader(const string& source,
	GLuint type) {
	GLuint id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	int res;
	glGetShaderiv(id, GL_COMPILE_STATUS, &res);

	if (res == GL_FALSE) {
		std::cerr << "Error happend in compiling shader";
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const string& vertexShader,
	const string& fragmentShader) {

	GLuint program = glCreateProgram();
	GLuint vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
	GLuint fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void display() {
	glUseProgram(shader);
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_QUADS, 0, getCubeVertices(0.5f).size() / 3);
}

int main() {
	GLFWwindow* window;
	if (!glfwInit())
		exit(EXIT_FAILURE);
	int width = 800;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(width, width, "SNAKE GAME", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glewInit();
	glEnable(GL_DEPTH_TEST);

	vector<float> cubeVertices = getCubeVertices(0.5f);
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(cubeVertices[0]), cubeVertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);



	shader = CreateShader(vsh, fsh);

	glUseProgram(shader);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}