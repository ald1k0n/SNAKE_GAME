#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace std;

class Shader
{
private:
	string vsh, fsh;

    static unsigned int CompileShader(const string& source, GLuint type) {
        GLuint id = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

            // Get the error message
            std::vector<char> error(length);
            glGetShaderInfoLog(id, length, &length, &error[0]);

            std::cerr << "Error occurred while compiling shader:\n" << &error[0] << std::endl;

            glDeleteShader(id);
            return 0;
        }

        return id;
    }

    static unsigned int CreateShader(const string& vertexShader, const string& fragmentShader) {
        GLuint program = glCreateProgram();
        GLuint vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
        GLuint fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        int result;
        glGetProgramiv(program, GL_LINK_STATUS, &result);
        if (result == GL_FALSE) {
            int length;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

            // Get the error message
            std::vector<char> error(length);
            glGetProgramInfoLog(program, length, &length, &error[0]);

            std::cerr << "Error occurred while linking shader program:\n" << &error[0] << std::endl;

            glDeleteProgram(program);
            glDeleteShader(vs);
            glDeleteShader(fs);
            return 0;
        }

        glValidateProgram(program);

        glDeleteShader(vs);
        glDeleteShader(fs);

        return program;
    }

public: 
	void setShaders(string vsh, string fsh) {
		this->vsh = vsh;
		this->fsh = fsh;
	}

	unsigned int getProgram() {
		return CreateShader(this->vsh, this->fsh);
	}
};

