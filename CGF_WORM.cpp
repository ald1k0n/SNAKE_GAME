#include "Shader.h"
#include "Cube.h"
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

string vsh = R"(
    #version 330 core
    layout(location = 0) in vec3 position;
	uniform mat4 transform;


    void main() {
        gl_Position = transform * vec4(position, 1.0);
    }
)";


string fsh = R"(
	#version 330 core
	layout(location = 0) out vec4 color;
	void main() {
		color = vec4(.3, 0.8, 0.01, 1.0);
	}
)";

string vsh1 = R"(
    #version 330 core
    layout(location = 0) in vec3 position;
	uniform mat4 transform;

    void main() {
        gl_Position = transform * vec4(position, 1.0);
    }
)";


string fsh1 = R"(
	#version 330 core
	layout(location = 0) out vec4 color;
	void main() {
		color = vec4(.8, 0.3, 0.01, 1.0);
	}
)";

int main() {
	GLuint shader, shader1;
	GLuint cubeVAO, cubeVBO, cubeVAO1, cubeVBO1;
	mat4 transform, transform1;

	transform = mat4(1);
	transform1 = mat4(1);
	
	Shader shaderClass, shaderClass1;
	
	GLFWwindow* window;
	if (!glfwInit())
		exit(EXIT_FAILURE);
	int width = 800;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(width, width, "WORM GAME", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glewInit();
	glEnable(GL_DEPTH_TEST);

	bindCube(cubeVAO, cubeVBO, 0.5f);
	bindCube(cubeVAO1, cubeVBO1, 0.3f);

	shaderClass.setShaders(vsh,fsh);
	shader = shaderClass.getProgram();

	shaderClass1.setShaders(vsh1, fsh1);
	shader1 = shaderClass1.getProgram();

	transform = translate(transform, vec3(-.5, 0, 0));
	glUniformMatrix4fv(glGetUniformLocation(shader, "transform"), 1, GL_FALSE, value_ptr(transform));

	transform1 = translate(transform1, vec3(.5, .5, 0));
	glUniformMatrix4fv(glGetUniformLocation(shader1, "transform"), 1, GL_FALSE, value_ptr(transform1));


	glUseProgram(shader);
	glUseProgram(shader1);


	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

		drawCube(shader, cubeVAO, .5f, transform);
		drawCube(shader1, cubeVAO1, .3f, transform1);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}