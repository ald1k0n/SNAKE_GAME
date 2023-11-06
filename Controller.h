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

void controll(GLFWwindow* window, mat4& wormTransform) {
	float step = 0.1f / 100;
	int horizontal = 0;
	int vertical = 0;

	if (glfwGetKey(window, GLFW_KEY_LEFT)) {
		horizontal = -1;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
		horizontal = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_UP)) {
		vertical = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN)) {
		vertical = -1;
	}
	if (horizontal != 0 && vertical != 0) {
		if (abs(horizontal) > abs(vertical)) {
			vertical = 0;
		}
		else {
			horizontal = 0;
		}
	}

	wormTransform = translate(wormTransform, vec3(step * horizontal, step * vertical, 0));

	vec3 objectPosition = vec3(wormTransform[3]);

	if (abs(objectPosition.x) >= 1.99f || abs(objectPosition.y) >= 1.49f) {
		cout << "Game Over";
		exit(0);
	}
}