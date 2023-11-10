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
	out vec2 TexCoord;
	void main() {
		gl_Position = transform * vec4(pos, 1.0);
		vPos = pos;
		TexCoord = vec2((pos.x + 1.0) / 2.0, (pos.y + 1.0) / 2.0);
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

string treatFSH = R"(
	#version 330 core
	out vec4 color;
	in vec3 vPos;
	in vec2 TexCoord;
	uniform sampler2D textureMap;
	uniform sampler2D normalMapTexture;
	uniform vec3 customColor;

	void main() {
		float shininess = 4.0;
		vec3 cameraPosition = vec3(0, 0.5, 0.8);
		vec3 lightDirection = normalize(vec3(0.5, 0, 1) - vPos);
		vec3 viewDirection = normalize(cameraPosition - vPos);
		vec3 norm = normalize(vPos);

		float diff = max(dot(norm, lightDirection), 0.0);
		vec3 diffuse = diff * vec3(1) * vec3(0.8, 0.0, 0.0);
		vec3 reflectDir = reflect(-lightDirection, norm);

		float spec = pow(max(dot(viewDirection, reflectDir), 0.0), shininess);
		vec3 specular = spec * vec3(1);
		vec3 result = (diffuse + specular) + customColor;

		vec3 textureColor = texture(textureMap, TexCoord).rgb;

		vec3 normalMap = texture(normalMapTexture, TexCoord).rgb;
		normalMap = normalize(normalMap * 2.0 - 1.0);

		result += textureColor * normalMap;

		color = vec4(result, 1);
	}
)";

void loadTextureMap(GLuint& texture, const char* path) {
	int width, height, nrChannels;

	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		cout << "Failed to load texture" << endl;
	}
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

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
	wormShader.setShaders(vsh, treatFSH);
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

	GLuint normalMapTreat, textureMapTreat;
	loadTextureMap(textureMapTreat, "C:\\Users\\cjube\\OneDrive\\Рабочий стол\\CGF_WORM — копия\\snake.png");
	loadTextureMap(normalMapTreat, "C:\\Users\\cjube\\OneDrive\\Рабочий стол\\CGF_WORM — копия\\snake_normal.png");
	glUseProgram(treatProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(treatProgram, "textureMap"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMapTexture);
	glUniform1i(glGetUniformLocation(treatProgram, "normalMapTexture"), 1);
	glUniform3f(glGetUniformLocation(treatProgram, "customColor"), 0.3f, 0.0f, 0.0f);

	glUseProgram(wormObjectProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(wormObjectProgram, "textureMap"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMapTexture);
	glUniform1i(glGetUniformLocation(wormObjectProgram, "normalMapTexture"), 1);
	glUniform3f(glGetUniformLocation(wormObjectProgram, "customColor"), 0.5, 0.3, 0.0);


	vector<GLuint> wormVAOs(10);
	vector<GLuint> wormVBOs(10); // Если вам нужны отдельные VBO
	for (int i = 0; i < 10; ++i) {
		bindCube(wormVAOs[i], wormVBOs[i], 0.1f);
		wormSegmentTransforms[i] = translate(cameraView, vec3(0, -0.2f * i, 0.2)); // Начальное положение каждого кубика
	}

	for (int i = 0; i < wormSegmentTransforms.size(); ++i) {
		drawCube(wormObjectProgram, wormVAOs[i], 0.1f, wormSegmentTransforms[i]);
	}

	// В основном игровом цикле в функции main()
	while (!glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime(); // Получаем текущее время
		// Очищаем буферы
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		controlHead(window, wormSegmentTransforms[0], treatTransform, treatPosition, score, currentTime);
		
		updateTail(currentTime);

		// Рисуем свет
		drawCube(lightObjectProgram, lightObjVAO, 0.2f, lightTransform);

		// Рисуем сегменты червяка
		for (int i = 0; i < wormSegmentTransforms.size(); ++i) {

			drawCube(wormObjectProgram, wormVAOs[i], 0.1f, wormSegmentTransforms[i]);
		}

		// Рисуем лакомство
		treatTransform = translate(cameraView, treatPosition);
		drawCube(treatProgram, treatVAO, 0.1f, treatTransform);

		// Рисуем доску
		drawBoard(boardShaderProgram, boardVAO);

		// Обмен буферов и обработка событий
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}