#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace glm;

float step = .1f / 500.f;
int horizontal = 0;
int vertical = 0;

vec3 getRandomGridPosition() {
	float x = static_cast<float>(rand() % 10) * 0.2f - 1.0f;
	float y = static_cast<float>(rand() % 10) * 0.2f - 1.0f;
	float z = 0.2f;

	return vec3(x, y, z);
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_RELEASE) {
        int newHorizontal = 0;
        int newVertical = 0;

        if (key == GLFW_KEY_LEFT) {
            newHorizontal = -1;
        }
        else if (key == GLFW_KEY_RIGHT) {
            newHorizontal = 1;
        }
        else if (key == GLFW_KEY_UP) {
            newVertical = 1;
        }
        else if (key == GLFW_KEY_DOWN) {
            newVertical = -1;
        }

        if (newHorizontal != -horizontal || newVertical != -vertical) {
            horizontal = newHorizontal;
            vertical = newVertical;
        }
    }
}

void controll(GLFWwindow* window, mat4& wormTransform, mat4& treatTransform, vec3& treatPosition, int& score) {
    vec3 wormPos = vec3(::step * horizontal, ::step * vertical, 0);
    vec3 newWormPosition = vec3(wormTransform[3]) + wormPos;

    vec3 treatPos = vec3(treatTransform[3]);

    if (newWormPosition.x >= -1.99f && newWormPosition.x <= 1.99f &&
        newWormPosition.y >= -1.59f && newWormPosition.y <= 1.59f) {
        wormTransform = translate(wormTransform, wormPos);
    }


    if (abs(newWormPosition.x - treatPos.x) <= 0.2f && abs(newWormPosition.y - treatPos.y) <= 0.2f) {
        score++;
        cout << score << endl;
        treatPosition = getRandomGridPosition();
    }
}
