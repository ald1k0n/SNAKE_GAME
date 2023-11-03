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
	layout(location = 0) in vec3 pos;
	uniform mat4 transform;
	void main() {
		gl_Position = transform * vec4(pos, 1.0);
	}
)";

string fsh = R"(
	#version 330 core
	out vec4 color;
	void main() {
		color = vec4(1.0);
	}
)";

vec3 lightPosition = vec3(0.5, 0.5, 1);

int main() {
	GLuint boardShaderProgram, lightObjectProgram;
	GLuint boardVAO, boardVBO, boardEBO, lightObjVAO, lightObjVBO;

	mat4 lightTransform = mat4(1);

	Shader boardShader, lightShader;

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

	lightShader.setShaders(vsh, fsh);
	lightObjectProgram = lightShader.getProgram();
	bindCube(lightObjVAO, lightObjVBO, 0.3f);

	bindBoard(boardVAO, boardVBO, boardEBO, boardShaderProgram);
	loadTexture();
	loadNormalMapTexture();
	glUseProgram(lightObjectProgram);
	glUseProgram(boardShaderProgram);
	bindTextures(boardShaderProgram);
	lightTransform = translate(cameraView, lightPosition);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawCube(lightObjectProgram, lightObjVAO, 0.3f, lightTransform);
		drawBoard(boardShaderProgram, boardVAO);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}