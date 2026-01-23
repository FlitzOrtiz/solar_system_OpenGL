#include "model/Ventana.h"
#include "model/Shader.h"
#include "model/Sphere.h" 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// ================= VARIABLES GLOBALES =================
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float cameraAngleX = 0.0f;
float cameraDistance = 10.0f; 
glm::vec3 cameraPos;
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float rotationSpeed = 60.0f; 

glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 La(1.0f, 1.0f, 1.0f); 
glm::vec3 Ld(1.0f, 1.0f, 1.0f); 
glm::vec3 Le(1.0f, 1.0f, 1.0f); 

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

int main() {
    Ventana app(SCR_WIDTH, SCR_HEIGHT, "Sistema Solar con Texturas");

    Shader shaderEsfera("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    // ---------------------------------------------------------
    // 1. CONFIGURACIÓN DEL SOL
    // ---------------------------------------------------------
    Sphere sun(0.5f);
    sun.setMaterial(
        glm::vec3(0.8f, 0.6f, 0.0f), 
        glm::vec3(1.0f, 0.9f, 0.0f), 
        glm::vec3(1.0f, 1.0f, 1.0f), 
        16.0f
    );
    // Cargamos la textura para el Sol [cite: 161, 164]
    sun.loadTexture("textures/2k_earth_daymap.jpg");

    // ---------------------------------------------------------
    // 2. CONFIGURACIÓN DE LA TIERRA
    // ---------------------------------------------------------
    Sphere earth(2.0f);
    earth.setMaterial(
        glm::vec3(0.0f, 0.1f, 0.2f), 
        glm::vec3(0.0f, 0.4f, 0.8f), 
        glm::vec3(0.5f, 0.5f, 0.5f), 
        500.0f
    );
    // Cargamos la misma textura para la Tierra [cite: 161, 164]
    earth.loadTexture("textures/2k_earth_daymap.jpg");

    updateCamera();

    float lastFrame = 0.0f;
    while (!app.debeCerrar()) {
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        processInput(app.window, deltaTime);

        glClearColor(0.02f, 0.02f, 0.04f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderEsfera.use();

        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

        shaderEsfera.setMat4("view", glm::value_ptr(view));
        shaderEsfera.setMat4("projection", glm::value_ptr(projection));

        shaderEsfera.setVec3("p0", glm::value_ptr(lightPos));
        shaderEsfera.setVec3("v_pos", glm::value_ptr(cameraPos));
        shaderEsfera.setVec3("La", glm::value_ptr(La));
        shaderEsfera.setVec3("Ld", glm::value_ptr(Ld));
        shaderEsfera.setVec3("Le", glm::value_ptr(Le));

        // Dibujar Sol [cite: 266]
        sun.Draw(shaderEsfera.ID, currentTime, glm::vec3(0.0f), glm::vec3(1.0f));

        // Dibujar Tierra orbitando [cite: 266]
        float orbitRadius = 5.0f;
        float x = sin(currentTime * 0.5f) * orbitRadius;
        float z = cos(currentTime * 0.5f) * orbitRadius;
        earth.Draw(shaderEsfera.ID, currentTime, glm::vec3(x, 0.0f, z), glm::vec3(0.4f));

        app.refrescar();
    }

    return 0;
}