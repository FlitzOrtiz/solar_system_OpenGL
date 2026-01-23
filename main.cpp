#include "model/Ventana.h"
#include "model/Shader.h"
#include "model/Sphere.h" // Clase que encapsula la geometría

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// ================= VARIABLES GLOBALES =================
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Cámara dinámica (Orbitando)
float cameraAngleX = 0.0f;
float cameraDistance = 10.0f; 
glm::vec3 cameraPos;
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float rotationSpeed = 60.0f; // Grados por segundo

// Propiedades de la LUZ Global (El Sol es la fuente)
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 La(1.0f, 1.0f, 1.0f); // Luz Ambiental blanca
glm::vec3 Ld(1.0f, 1.0f, 1.0f); // Luz Difusa blanca
glm::vec3 Le(1.0f, 1.0f, 1.0f); // Luz Especular blanca

// ================= FUNCIONES DE APOYO =================
void updateCamera() {
    cameraPos.x = cameraDistance * sin(cameraAngleX);
    cameraPos.y = cameraDistance * 0.5f; 
    cameraPos.z = cameraDistance * cos(cameraAngleX);
}

void processInput(GLFWwindow* window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // Control de cámara con flechas
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        cameraAngleX -= glm::radians(rotationSpeed) * deltaTime;
        updateCamera();
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        cameraAngleX += glm::radians(rotationSpeed) * deltaTime;
        updateCamera();
    }
}

// ================= MAIN =================
int main() {
    // 1. Inicializar Ventana (Usa tu clase modular)
    Ventana app(SCR_WIDTH, SCR_HEIGHT, "Sistema Solar - Phong & GLM");

    // 2. Cargar Shaders (Usa tu clase modular)
    Shader shaderEsfera("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    // 3. Configuración de los Planetas (Usando la clase Sphere)
    // El Sol (Radio 0.2 para que no tape todo, aunque es el centro)
    Sphere sun(0.5f);
    sun.setMaterial(
        glm::vec3(0.8f, 0.6f, 0.0f), // Ambiental (Dorado)
        glm::vec3(1.0f, 0.9f, 0.0f), // Difusa (Amarillo brillante)
        glm::vec3(1.0f, 1.0f, 1.0f), // Especular (Blanco)
        16.0f                        // Brillo
    );

    // La Tierra (Radio 2.0 según tu requerimiento)
    Sphere earth(2.0f);
    earth.setMaterial(
        glm::vec3(0.0f, 0.1f, 0.2f), // Ambiental (Azul profundo)
        glm::vec3(0.0f, 0.4f, 0.8f), // Difusa (Cian)
        glm::vec3(0.5f, 0.5f, 0.5f), // Especular (Gris - Reflejo moderado)
        500.0f                        // Brillo (Más concentrado)
    );

    updateCamera(); // Posición inicial de la cámara

    // --- Bucle de Renderizado ---
    float lastFrame = 0.0f;
    while (!app.debeCerrar()) {
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        processInput(app.window, deltaTime);

        glClearColor(0.02f, 0.02f, 0.04f, 1.0f); // Fondo espacio oscuro
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderEsfera.use();

        // --- MATRICES DE CÁMARA (GLM) ---
        // Generamos la matriz View con lookAt
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        // Generamos la matriz Projection con perspective
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

        // Enviamos matrices a la GPU (Casting a float* usando value_ptr)
        shaderEsfera.setMat4("view", glm::value_ptr(view));
        shaderEsfera.setMat4("projection", glm::value_ptr(projection));

        // --- CONFIGURACIÓN DE LUZ GLOBAL ---
        shaderEsfera.setVec3("p0", glm::value_ptr(lightPos));
        shaderEsfera.setVec3("v_pos", glm::value_ptr(cameraPos));
        shaderEsfera.setVec3("La", glm::value_ptr(La));
        shaderEsfera.setVec3("Ld", glm::value_ptr(Ld));
        shaderEsfera.setVec3("Le", glm::value_ptr(Le));

        // ----------------------------------------
        // DIBUJAR SOL (En el centro)
        // ----------------------------------------
        // La clase Sphere se encarga de aplicar sus propios materiales internamente
        sun.Draw(shaderEsfera.ID, currentTime, glm::vec3(0.0f), glm::vec3(1.0f));

        // ----------------------------------------
        // DIBUJAR TIERRA (Orbitando)
        // ----------------------------------------
        float orbitRadius = 5.0f;
        float x = sin(currentTime * 0.5f) * orbitRadius;
        float z = cos(currentTime * 0.5f) * orbitRadius;
        
        // Dibujamos con desplazamiento y escala reducida para que quepa en pantalla
        earth.Draw(shaderEsfera.ID, currentTime, glm::vec3(x, 0.0f, z), glm::vec3(0.4f));

        app.refrescar();
    }

    return 0;
}