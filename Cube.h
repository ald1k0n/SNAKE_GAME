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

vector<float> getCubeVertices(float size) {
	float halfSize = (size / 2.0f);

	vector<float> cube_vertices = {
		-halfSize, -halfSize, halfSize,
		halfSize, -halfSize, halfSize,
		halfSize, halfSize, halfSize,
		-halfSize, halfSize, halfSize,

		-halfSize, -halfSize, -halfSize,
		halfSize, -halfSize, -halfSize,
		halfSize, halfSize, -halfSize,
		-halfSize, halfSize, -halfSize,

		-halfSize, halfSize, halfSize,
		halfSize, halfSize, halfSize,
		halfSize, halfSize, -halfSize,
		-halfSize, halfSize, -halfSize,

		-halfSize, -halfSize, halfSize,
		halfSize, -halfSize, halfSize,
		halfSize, -halfSize, -halfSize,
		-halfSize, -halfSize, -halfSize,

		halfSize, -halfSize, halfSize,
		halfSize, -halfSize, -halfSize,
		halfSize, halfSize, -halfSize,
		halfSize, halfSize, halfSize,

		-halfSize, -halfSize, halfSize,
		-halfSize, -halfSize, -halfSize,
		-halfSize, halfSize, -halfSize,
		-halfSize, halfSize, halfSize,
	};

	return cube_vertices;
}

void bindCube(GLuint& VAO, GLuint& VBO, float size) {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, getCubeVertices(size).size() * sizeof(getCubeVertices(size)[0]), getCubeVertices(size).data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void drawCube(GLuint& program, GLuint& VAO, float size, mat4& transformation) {
	glUseProgram(program);
	glBindVertexArray(VAO);
	transformation = rotate(transformation, 0.001f, vec3(0.5f, 0.5f, 0.5f));
	glUniformMatrix4fv(glGetUniformLocation(program, "transform"), 1, GL_FALSE, value_ptr<float>(transformation));
	glDrawArrays(GL_QUADS, 0, getCubeVertices(size).size() / 3);
}