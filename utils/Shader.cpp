#include "../model/Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile; // Archivos de shader, lectura

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); // Habilitar excepciones: failbit (error en la operación) y badbit (error grave)
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath); // Abrir archivos
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream; // Stream de datos para leer los archivos
        vShaderStream << vShaderFile.rdbuf(); // Leer el buffer del archivo en el stream
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str(); // Convertir stream a string
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::ARCHIVO_NO_LEIDO" << std::endl;
    }

    const char* vertexShaderSource = vertexCode.c_str();
    const char* fragmentShaderSource = fragmentCode.c_str();

    unsigned int vertexShader, fragmentShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() { 
    glUseProgram(shaderProgram);
}

void Shader::setVec3(const std::string &name, float* values) {
    glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, values);
}

void Shader::setMat4(const std::string &name, float* mat) {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, mat);
}

void Shader::setFloat(const std::string &name, float value) { 
    glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}