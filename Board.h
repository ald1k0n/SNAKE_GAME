#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "stb_image.h"
using namespace std;
using namespace glm;

string boardVsh = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;
	uniform mat4 transform;
    void main()
    {
        gl_Position = transform *vec4(aPos, 1.0);
    }
)";

string boardFsh = R"(
    #version 330 core
    out vec4 FragColor;
    uniform sampler2D texture1;
    
    void main()
    {
        FragColor = texture(texture1, vec2(gl_FragCoord.x / textureSize(texture1, 0).x, 1.0 - gl_FragCoord.y / textureSize(texture1, 0).y));
    }
)";

mat4 cameraView = mat4(1);
mat4 projection = perspective(radians(50.0f), 1.0f, 1.0f, 10.0f);
mat4 view = lookAt(vec3(0, -1, 2.5), vec3(0, 0, 0), vec3(0, 1, 0));

float boardVertices[] = {
    -1.f, -1.f, 0.0f,
     1.f, -1.f, 0.0f,
     1.f,  1.f, 0.0f,
    -1.f,  1.f, 0.0f
};

unsigned int boardIndices[] = {
    0, 1, 2,
    2, 3, 0
};

void bindBoard(GLuint& VAO, GLuint& VBO, GLuint& EBO, GLuint& program) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boardVertices), boardVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boardIndices), boardIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    cameraView *= projection * view;
    glUniformMatrix4fv(glGetUniformLocation(program, "transform"), 1, GL_FALSE, glm::value_ptr(cameraView));
}

void drawBoard(GLuint& program, GLuint& VAO) {
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "transform"), 1, GL_FALSE, glm::value_ptr(cameraView));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void loadTexture() {
    int width, height, nrChannels;
    unsigned char* data = stbi_load("C:\\Users\\cjube\\OneDrive\\Рабочий стол\\texture\\texture.jpg", &width, &height, &nrChannels, 0);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
