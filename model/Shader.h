#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
    // Utilidades para enviar uniforms
    void setVec3(const std::string &name, float x, float y, float z);
    void setVec3(const std::string &name, float* values);
    void setMat4(const std::string &name, float* mat);
    void setFloat(const std::string &name, float value);
};

#endif