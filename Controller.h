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
#include <array>

std::array<double, 10> lastMoveTime = { 0, 0, 0, 0,0, 0, 0, 0,0,0 }; // Время последнего движения для каждого сегмента
double lastTailMoveTime = 0.0; // Время последнего движения хвоста
const double tailMoveDelay = 0.05; // Задержка между обновлениями хвоста

using namespace std;
using namespace glm;

vector<mat4> wormSegmentTransforms(10, mat4(1.0f));

float step1 = .1f / 500.f;
int horizontal = 0;
int vertical = 0;


vec3 getRandomGridPosition() {
	float x = static_cast<float>(rand() % 10) * 0.2f - 1.f;
	float y = static_cast<float>(rand() % 10) * 0.2f - 1.f;
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



void updateWormSegments() {
    // Сохраняем предыдущую позицию головы
    mat4 previousTransform = wormSegmentTransforms[0];

    // Обновляем позиции хвостовых сегментов
    for (size_t i = wormSegmentTransforms.size() - 1; i >= 1; --i) {
        wormSegmentTransforms[i] = wormSegmentTransforms[i - 1];
    }

    // Обновляем позицию головы
    wormSegmentTransforms[0] = translate(wormSegmentTransforms[0], vec3(step1 * horizontal, step1 * vertical, 0));
}

void updateTail(double currentTime) {
    if (currentTime - lastTailMoveTime >= tailMoveDelay) {
        // Перемещаем каждый сегмент на позицию предыдущего
        for (size_t i = wormSegmentTransforms.size() - 1; i > 0; --i) {
            wormSegmentTransforms[i] = wormSegmentTransforms[i - 1];
        }

        lastTailMoveTime = currentTime; // Обновляем время последнего движения хвоста
    }
}

bool controlHead(GLFWwindow* window, mat4& wormHeadTransform, mat4& treatTransform, vec3& treatPosition, int& score, double currentTime) {
    vec3 headPos = vec3(step1 * horizontal, step1 * vertical, 0);
    vec3 newHeadPosition = vec3(wormHeadTransform[3]) + headPos;
    lastMoveTime[0] = currentTime;

    // Проверяем, что новая позиция находится в пределах поля
    bool moved = false;
    if (newHeadPosition.x >= -2.29f && newHeadPosition.x <= 2.29f &&
        newHeadPosition.y >= -1.89f && newHeadPosition.y <= 1.79f) {
        wormHeadTransform = translate(wormHeadTransform, headPos);
        moved = true;
        
    }

    vec3 treatPos = vec3(treatTransform[3]);

    if (newHeadPosition.x >= -2.29f && newHeadPosition.x <= 2.29f &&
        newHeadPosition.y >= -1.69f && newHeadPosition.y <= 1.89f) {
        wormHeadTransform = translate(wormHeadTransform, headPos);
    }

    if (abs(newHeadPosition.x - treatPos.x) <= 0.3f && abs(newHeadPosition.y - treatPos.y) <= 0.3f) {
        score++;
        cout << score << endl;
        treatPosition = getRandomGridPosition();
    }

    return moved;

}
