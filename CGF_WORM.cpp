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
	out vec3 vPos;
	void main() {
		gl_Position = transform * vec4(pos, 1.0);
		vPos = pos;
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
	in vec3 vPos;

	void main() {
		float shininess = 8.0;
		vec3 cameraPosition = vec3(0, 0.5, 0.8);
		vec3 lightDirection = normalize(vec3(0.5, 0.0, 1) - vPos);
		vec3 viewDirection = normalize(cameraPosition - vPos);
		vec3 norm = normalize(vPos);
		
		float diff = max(dot(norm, lightDirection), 0.0);

		vec3 diffuse = diff * vec3(1) * vec3(0.8, 0.5, 0.0);
		vec3 reflectDir = reflect(-lightDirection, norm);

		float spec = pow(max(dot(viewDirection, reflectDir), 0.0), shininess);

		vec3 specular = spec * vec3(1);
		vec3 result = (diffuse + specular) + vec3(0.8, 0.3, 0.0);

		color = vec4(result, 1.0);
	}
)";

string treatFSH = R"(
	#version 330 core
	out vec4 color;
	in vec3 vPos;
	void main() {
		float shininess = 8.0;
		vec3 cameraPosition = vec3(0, 0.5, 0.8);
		vec3 lightDirection = normalize(vec3(0.5, 0, 1) - vPos);
		vec3 viewDirection = normalize(cameraPosition - vPos);
		vec3 norm = normalize(vPos);

		float diff = max(dot(norm, lightDirection), 0.0);		
		vec3 diffuse = diff * vec3(1) * vec3(0.8, 0.0, 0.0);
		vec3 reflectDir = reflect(-lightDirection, norm);

		float spec = pow(max(dot(viewDirection, reflectDir), 0.0), shininess);		
		vec3 specular = spec * vec3(1);
		vec3 result = (diffuse + specular) + vec3(0.8, 0, 0.0);
		color = vec4(result, 1);
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
	bindCube(lightObjVAO, lightObjVBO, 0.2f);

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
	wormTransform = translate(cameraView, vec3(0, 0, 0.2));

	treatTransform = translate(cameraView, treatPosition);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw light
		drawCube(lightObjectProgram, lightObjVAO, 0.2f, lightTransform);

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