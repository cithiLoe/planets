#pragma once

#include <GL/glew.h>

class Shader {
private:
public:
    GLuint program;
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    void use();
};