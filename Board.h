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
    layout(location = 1) in vec3 aNormal;
	uniform mat4 transform;
    out vec3 Normal;
    out vec3 vPos;
    void main()
    {
        gl_Position = transform * vec4(aPos, 1.0);
        Normal = aNormal;
        vPos = aPos;
    }
)";

string boardFsh = R"(
    #version 330 core
    out vec4 FragColor;
    uniform sampler2D texture1;

    in vec3 Normal;
    in vec3 vPos;
    void main()
    {
       float shininess = 4.0;
       vec3 cameraPosition = vec3(0, 0.5, 0.8);
       vec3 lightDirection = normalize(vec3(0.5, 0.5, 1));
       vec3 viewDirection = normalize(cameraPosition - vPos);
       vec3 normal = normalize(Normal);
       vec3 reflection = reflect(-lightDirection, normal);


       float specular = pow(max(dot(viewDirection, reflection), 0.0), shininess);

       vec3 diffuseColor = vec3(1.0);
       vec3 specularColor = vec3(1.0);        

       vec3 diffuseResult = 0.4 * diffuseColor * max(dot(normal, lightDirection), 0.0);
       vec3 specularResult = specularColor * specular;


       vec3 resultColor = diffuseResult + specularResult;
       vec3 texColor = texture(texture1, gl_FragCoord.xy / textureSize(texture1, 0)).xyz;
       resultColor *= texColor;
       FragColor = vec4(resultColor , 1.0);
    }
)";

mat4 cameraView = mat4(1);
mat4 projection = perspective(radians(50.0f), 1.0f, 1.0f, 10.0f);
mat4 view = lookAt(vec3(0, -2, 2.5), vec3(0, 0, 0), vec3(0, 1, 0));

float boardVertices[] = {
    -1.f, -1.f, 0.0f,
     1.f, -1.f, 0.0f,
     1.f,  1.f, 0.0f,
    -1.f,  1.f, 0.0f
};

float boardNormals[] = {
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f
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

    GLuint normalVBO;
    glGenBuffers(1, &normalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boardNormals), boardNormals, GL_STATIC_DRAW);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    cameraView *= projection * view;
    glUniformMatrix4fv(glGetUniformLocation(program, "transform"), 1, GL_FALSE, value_ptr(cameraView));
}

void drawBoard(GLuint& program, GLuint& VAO) {
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "transform"), 1, GL_FALSE, value_ptr(cameraView));
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

