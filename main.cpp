#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model/Sphere.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// ================= SHADERS (SIN CAMBIOS) =================
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform vec3 uDesplazamiento; 
uniform vec3 uEscala;         
uniform vec3 uAngulos;        

uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    mat4 t = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, uDesplazamiento.x, uDesplazamiento.y, uDesplazamiento.z, 1.0);
    mat4 s = mat4(uEscala.x, 0.0, 0.0, 0.0, 0.0, uEscala.y, 0.0, 0.0, 0.0, 0.0, uEscala.z, 0.0, 0.0, 0.0, 0.0, 1.0);

    float cx = cos(uAngulos.x); float sx = sin(uAngulos.x);
    mat4 rx = mat4(1.0, 0.0, 0.0, 0.0, 0.0, cx, sx, 0.0, 0.0, -sx, cx, 0.0, 0.0, 0.0, 0.0, 1.0);
    float cy = cos(uAngulos.y); float sy = sin(uAngulos.y);
    mat4 ry = mat4(cy, 0.0, -sy, 0.0, 0.0, 1.0, 0.0, 0.0, sy, 0.0, cy, 0.0, 0.0, 0.0, 0.0, 1.0);
    float cz = cos(uAngulos.z); float sz = sin(uAngulos.z);
    mat4 rz = mat4(cz, sz, 0.0, 0.0, -sz, cz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    
    mat4 rot = rz * ry * rx;
    mat4 model = t * rot * s;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(rot) * aNormal; 
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 p0; 
uniform vec3 v_pos; 
uniform vec3 La, Ld, Le; 
uniform vec3 Ka, Kd, Ke; 
uniform float alpha; 

void main() {
    vec3 n = normalize(Normal);
    vec3 l = normalize(p0 - FragPos);
    vec3 v = normalize(v_pos - FragPos);
    vec3 r = reflect(-l, n);

    float dist = length(FragPos - p0);
    float dist2 = dist * dist;
    if(dist2 < 0.01) dist2 = 0.0001; 

    vec3 Ia = Ka * La;
    vec3 Id = (Kd * Ld * max(dot(n, l), 0.0)) / dist2; 
    vec3 Ie = (Ke * Le * pow(max(dot(r, v), 0.0), alpha)) / dist2;

    FragColor = vec4(Ia + Id + Ie, 1.0);
}
)";

// ================= VARIABLES CÁMARA =================
float cameraAngleX = 0.0f;
float cameraDistance = 10.0f; 
glm::vec3 cameraPos;
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float rotationSpeed = 60.0f;

void updateCamera() {
    cameraPos.x = cameraDistance * sin(cameraAngleX);
    cameraPos.y = cameraDistance * 0.5f; 
    cameraPos.z = cameraDistance * cos(cameraAngleX);
}

void processInput(GLFWwindow* window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        cameraAngleX -= glm::radians(rotationSpeed) * deltaTime;
        updateCamera();
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        cameraAngleX += glm::radians(rotationSpeed) * deltaTime;
        updateCamera();
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

unsigned int compileShader(const char* source, GLenum type) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    return shader;
}

unsigned int createShaderProgram() {
    unsigned int vs = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    unsigned int fs = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs); glLinkProgram(prog);
    glDeleteShader(vs); glDeleteShader(fs);
    return prog;
}

// ================= MAIN =================
int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Planetas - Material Interno", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glewExperimental = GL_TRUE; 
    glewInit();
    glEnable(GL_DEPTH_TEST);

    unsigned int shaderProgram = createShaderProgram();
    updateCamera();

    // ===========================================
    // 1. CONFIGURACIÓN DEL SOL
    // ===========================================
    Sphere sun(0.2f);
    // Material Sol:
    // Ka (Ambiental): Amarilla brillante
    // Kd (Difusa): Amarilla Intensa
    // Ke (Especular): Blanca
    // Shininess: 16
    sun.setMaterial(
        glm::vec3(0.8f, 0.6f, 0.0f), 
        glm::vec3(1.0f, 0.9f, 0.0f), 
        glm::vec3(1.0f, 1.0f, 1.0f), 
        16.0f
    );

    // ===========================================
    // 2. CONFIGURACIÓN DE LA TIERRA
    // ===========================================
    Sphere earth(2.0f);
    // Material Tierra:
    // Ka (Ambiental): Azul oscuro muy tenue
    // Kd (Difusa): Azul/Cian
    // Ke (Especular): Blanco brillante (reflejo océanos)
    // Shininess: 64 (más brillante/húmedo)
    earth.setMaterial(
        glm::vec3(0.0f, 0.1f, 0.2f), 
        glm::vec3(0.0f, 0.4f, 0.8f), 
        glm::vec3(0.5f, 0.5f, 0.5f), 
        64.0f
    );

    // Propiedades de la LUZ (externas a las esferas)
    glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
    glm::vec3 La(1.0f, 1.0f, 1.0f);
    glm::vec3 Ld(1.0f, 1.0f, 1.0f);
    glm::vec3 Le(1.0f, 1.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - 0.0f; // Simplificado para ejemplo

        processInput(window, 0.016f); // Delta time fijo aprox

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Actualizar Matrices Globales
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Actualizar Luz Global
        glUniform3fv(glGetUniformLocation(shaderProgram, "p0"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "v_pos"), 1, glm::value_ptr(cameraPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "La"), 1, glm::value_ptr(La));
        glUniform3fv(glGetUniformLocation(shaderProgram, "Ld"), 1, glm::value_ptr(Ld));
        glUniform3fv(glGetUniformLocation(shaderProgram, "Le"), 1, glm::value_ptr(Le));

        // ----------------------------------------
        // DIBUJAR SOL
        // ----------------------------------------
        // Ya no pasamos color aquí, el objeto 'sun' ya sabe que es amarillo
        sun.Draw(shaderProgram, currentTime, glm::vec3(0.0f), glm::vec3(1.5f));

        // ----------------------------------------
        // DIBUJAR TIERRA
        // ----------------------------------------
        float orbitRadius = 4.0f;
        float x = sin(currentTime * 0.5f) * orbitRadius;
        float z = cos(currentTime * 0.5f) * orbitRadius;
        // El objeto 'earth' ya sabe que es azul
        earth.Draw(shaderProgram, currentTime, glm::vec3(x, 0.0f, z), glm::vec3(0.5f));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}