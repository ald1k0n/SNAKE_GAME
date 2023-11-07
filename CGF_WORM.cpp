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

#include <cstdlib>
#include <ctime>


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

string wormFSH = R"(
	#version 330 core
	out vec4 color;
	void main() {
		color = vec4(1, 0.5, 0.0, 1.0);
	}
)";

string treatFSH = R"(
	#version 330 core
	out vec4 color;
	void main() {
		color = vec4(1, 0, 0, 1);
	}
)";



int main() {
	int score = 0;
	GLuint boardShaderProgram, lightObjectProgram, wormObjectProgram, treatProgram;
	GLuint boardVAO, boardVBO, boardEBO, lightObjVAO, lightObjVBO, 
		wormVAO, wormVBO, treatVAO, treatVBO;

	mat4 lightTransform = mat4(1), wormTransform = mat4(1), treatTransform = mat4(1);

	Shader boardShader, lightShader, wormShader, treatShader;

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
	glfwSetKeyCallback(window, keyCallback);

	srand(static_cast<unsigned>(time(nullptr)));
	vec3 treatPosition = getRandomGridPosition();

	boardShader.setShaders(boardVsh, boardFsh);
	boardShaderProgram = boardShader.getProgram();
	
	lightShader.setShaders(vsh, fsh);
	lightObjectProgram = lightShader.getProgram();
	bindCube(lightObjVAO, lightObjVBO, 0.3f);

	//worm
	wormShader.setShaders(vsh, wormFSH);
	wormObjectProgram = wormShader.getProgram();
	bindCube(wormVAO, wormVBO, 0.1f);
	
	//Treat
	treatShader.setShaders(vsh, treatFSH);
	treatProgram = treatShader.getProgram();
	bindCube(treatVAO, treatVBO, 0.1f);


	bindBoard(boardVAO, boardVBO, boardEBO, boardShaderProgram);
	loadTexture();
	loadNormalMapTexture();
	glUseProgram(lightObjectProgram);
	glUseProgram(boardShaderProgram);
	bindTextures(boardShaderProgram);
	
	// All transforms must be applied here
	lightTransform = translate(cameraView, lightPosition);
	lightTransform = translate(lightTransform, vec3(0, 0.4, 0.2));

	wormTransform = translate(cameraView, vec3(0, 0, 0.2));
	cout << "worm: " <<endl;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			std::cout << wormTransform[i][j] << " ";
		}
		std::cout << std::endl;
	}


	treatTransform = translate(cameraView, treatPosition);
	cout << "treat: " << endl;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			std::cout << treatTransform[i][j] << " ";
		}
		std::cout << std::endl;
	}


	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw light
		drawCube(lightObjectProgram, lightObjVAO, 0.3f, lightTransform);

		// Draw worm
		drawCube(wormObjectProgram, wormVAO, 0.1f, wormTransform);
		controll(window, wormTransform, treatTransform, treatPosition, score);

		// Draw Treat
		treatTransform = translate(cameraView, treatPosition);
		drawCube(treatProgram, treatVAO, 0.1f, treatTransform);


		// Board
		drawBoard(boardShaderProgram, boardVAO);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}