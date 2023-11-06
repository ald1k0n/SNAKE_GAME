#include "Shader.h"
#include "Cube.h"
#include "Board.h"
#include "Controller.h"
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


string wormVSH = R"(
	#version 330 core
	layout(location = 0) in vec3 pos;
	uniform mat4 transform;
	void main(){
		gl_Position = transform * vec4(pos, 1.0);
	}
)";

string wormFSH = R"(
	#version 330 core
	out vec4 color;
	void main() {
		color = vec4(1, 0.5, 0.0, 1.0);
	}
)";


int main() {
	GLuint boardShaderProgram, lightObjectProgram, wormObjectProgram;
	GLuint boardVAO, boardVBO, boardEBO, lightObjVAO, lightObjVBO, 
		wormVAO, wormVBO;

	mat4 lightTransform = mat4(1), wormTransform = mat4(1);

	Shader boardShader, lightShader, wormShader;

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

	//worm
	wormShader.setShaders(wormVSH, wormFSH);
	wormObjectProgram = wormShader.getProgram();
	bindCube(wormVAO, wormVBO, 0.1f);
	
	bindBoard(boardVAO, boardVBO, boardEBO, boardShaderProgram);
	loadTexture();
	loadNormalMapTexture();
	glUseProgram(lightObjectProgram);
	glUseProgram(boardShaderProgram);
	bindTextures(boardShaderProgram);
	
	// All transforms must be applied here
	lightTransform = translate(cameraView, lightPosition);
	lightTransform = translate(lightTransform, vec3(0, 0.4, 0.2));
	wormTransform = translate(cameraView, vec3(0,0,0.2));

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw light
		drawCube(lightObjectProgram, lightObjVAO, 0.3f, lightTransform);

		// Draw worm
		drawCube(wormObjectProgram, wormVAO, 0.1f, wormTransform);
		controll(window, wormTransform);

		// Board
		drawBoard(boardShaderProgram, boardVAO);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}