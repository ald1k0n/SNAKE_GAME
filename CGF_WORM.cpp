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


int main() {
	GLuint boardShaderProgram;
	GLuint boardVAO, boardVBO, boardEBO;

	mat4 cubeTransform = mat4(1);

	Shader boardShader;

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


	boardShader.setShaders(boardVsh, boardFsh);
	boardShaderProgram = boardShader.getProgram();
	bindBoard(boardVAO, boardVBO, boardEBO, boardShaderProgram);
	loadTexture();

	glUseProgram(boardShaderProgram);
	glUniform1i(glGetUniformLocation(boardShaderProgram, "texture1"), 0);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		
		drawBoard(boardShaderProgram, boardVAO);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}