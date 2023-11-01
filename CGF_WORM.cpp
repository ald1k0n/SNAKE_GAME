#include "Shader.h"
#include "Cube.h"
#include "Board.h"
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

int main() {
	GLuint boardShaderProgram, cubeShaderProgram;
	GLuint boardVAO, boardVBO, boardEBO, cubeVAO, cubeVBO;

	mat4 cubeTransform = mat4(1);

	Shader boardShader, cubeShader;

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
	cubeShader.setShaders(vsh, fsh);
	cubeShaderProgram = cubeShader.getProgram();

	boardShader.setShaders(boardVsh, boardFsh);
	boardShaderProgram = boardShader.getProgram();
	glUniformMatrix4fv(glGetUniformLocation(cubeShaderProgram, "transform"), 1, GL_FALSE, value_ptr(cameraView));

	bindCube(cubeVAO, cubeVBO, .5f);
	bindBoard(boardVAO, boardVBO, boardEBO, boardShaderProgram);
	
	glUseProgram(boardShaderProgram);
	glUseProgram(cubeShaderProgram);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		
		drawBoard(boardShaderProgram, boardVAO);
		drawCube(cubeShaderProgram, cubeVAO,.5f, cameraView);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}