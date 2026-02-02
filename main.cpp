#include "model/Ventana.h"
#include "model/Shader.h"
#include "model/Sphere.h" 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// ================= VARIABLES GLOBALES =================
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

float cameraAngleX = 0.0f;
float cameraDistance = 30.0f; // 30.0f por defecto
glm::vec3 cameraPos;
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float traslationSpeed = 60.0f; 

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
        cameraAngleX -= glm::radians(traslationSpeed) * deltaTime;
        updateCamera();
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        cameraAngleX += glm::radians(traslationSpeed) * deltaTime;
        updateCamera();
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        cameraDistance -= 10.0f * deltaTime;
        if (cameraDistance < 5.0f) cameraDistance = 5.0f; 
        updateCamera();
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        cameraDistance += 10.0f * deltaTime;
        if (cameraDistance > 100.0f) cameraDistance = 100.0f; 
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
    sun.loadTexture("textures/2k_sun.jpg");

    // ---------------------------------------------------------
    // 2. CONFIGURACIÓN DE LOS PLANETAS
    // ---------------------------------------------------------

    // DEFINIR RADIOS ORBITALES (como variables)
    float orbitRadiusM = 3.0f;    // Mercurio
    float orbitRadiusV = 5.5f;    // Venus
    float orbitRadiusE = 7.0f;    // Tierra
    float orbitRadiusMa = 10.0f;  // Marte
    float orbitRadiusJ = 15.0f;   // Júpiter
    float orbitRadiusS = 22.0f;   // Saturno
    float orbitRadiusU = 30.0f;   // Urano
    float orbitRadiusN = 40.0f;   // Neptuno
    
    // DEFINIR VELOCIDADES ORBITALES (la mitad)
    float orbitalSpeedM = 2.0f;     // Mercurio (4.0f / 2)
    float orbitalSpeedV = 1.25f;    // Venus (2.5f / 2)
    float orbitalSpeedE = 0.5f;     // Tierra - base (1.0f / 2)
    float orbitalSpeedMa = 0.375f;  // Marte (0.75f / 2)
    float orbitalSpeedJ = 0.125f;   // Júpiter (0.25f / 2)
    float orbitalSpeedS = 0.075f;   // Saturno (0.15f / 2)
    float orbitalSpeedU = 0.04f;    // Urano (0.08f / 2)
    float orbitalSpeedN = 0.025f;   // Neptuno (0.05f / 2)

    // PARÁMETROS DE LA LUNA
    // Parámetros de la órbita de la Luna alrededor de la Tierra
    float moonOrbitRadius = 1.5f; // Distancia de la Luna a la Tierra
    float moonOrbitalSpeed = 5.0f; // Velocidad orbital de la Luna (más rápido que la Tierra alrededor del Sol)

    // CONFIGURACIÓN DE PLANETAS

    Sphere mercury(0.05f);
    mercury.setMaterial(
        glm::vec3(0.5f, 0.5f, 0.5f), 
        glm::vec3(0.7f, 0.7f, 0.7f), 
        glm::vec3(0.3f, 0.3f, 0.3f), 
        10.0f
    );

    Sphere venus(-0.02f);
    venus.setMaterial(
        glm::vec3(0.9f, 0.7f, 0.2f), 
        glm::vec3(1.0f, 0.8f, 0.3f), 
        glm::vec3(0.5f, 0.4f, 0.2f), 
        12.0f
    );


    Sphere earth(1.0f);
    earth.setMaterial(
        glm::vec3(0.0f, 0.1f, 0.2f), 
        glm::vec3(0.0f, 0.4f, 0.8f), 
        glm::vec3(0.5f, 0.5f, 0.5f), 
        15.0f
    );

    Sphere mars(0.95f);
    mars.setMaterial(
        glm::vec3(0.6f, 0.2f, 0.1f), 
        glm::vec3(0.8f, 0.3f, 0.1f), 
        glm::vec3(0.4f, 0.2f, 0.1f), 
        13.0f
    );

    Sphere jupyter(2.5f);
    jupyter.setMaterial(
        glm::vec3(0.8f, 0.4f, 0.1f),  
        glm::vec3(0.9f, 0.5f, 0.2f), 
        glm::vec3(0.5f, 0.5f, 0.5f), 
        20.0f
    );

    Sphere saturn(2.3f);
    saturn.setMaterial(
        glm::vec3(0.9f, 0.8f, 0.6f), 
        glm::vec3(1.0f, 0.9f, 0.7f), 
        glm::vec3(0.6f, 0.5f, 0.4f), 
        18.0f
    );


    // URANO
    Sphere uranus(1.5f);
    uranus.setMaterial(
        glm::vec3(0.4f, 0.6f, 0.8f), 
        glm::vec3(0.5f, 0.7f, 0.9f), 
        glm::vec3(0.3f, 0.5f, 0.7f), 
        16.0f
    );

    // NEPTUNO
    Sphere neptune(1.6f);
    neptune.setMaterial(
        glm::vec3(0.1f, 0.1f, 0.8f), 
        glm::vec3(0.2f, 0.2f, 1.0f), 
        glm::vec3(0.1f, 0.1f, 0.6f), 
        17.0f
    );

    // ---------------------------------------------------------
    // 3. CONFIGURACIÓN DE LA LUNA
    // ---------------------------------------------------------
    Sphere moon(0.5f); // Rotación sobre su eje (si queremos que siempre mueva la misma cara, podríamos ajustar)
    moon.setMaterial(
        glm::vec3(0.2f, 0.2f, 0.2f), 
        glm::vec3(0.5f, 0.5f, 0.5f), 
        glm::vec3(0.1f, 0.1f, 0.1f), 
        5.0f
    );


    // Cargamos la misma textura para la Tierra [cite: 161, 164]
    mercury.loadTexture("textures/2k_mercury.jpg");
    venus.loadTexture("textures/2k_venus_surface.jpg");
    earth.loadTexture("textures/2k_earth_daymap.jpg");
    mars.loadTexture("textures/2k_mars.jpg");
    jupyter.loadTexture("textures/2k_jupiter.jpg");
    saturn.loadTexture("textures/2k_saturn.jpg");
    uranus.loadTexture("textures/2k_uranus.jpg");
    neptune.loadTexture("textures/2k_neptune.jpg");

    moon.loadTexture("textures/2k_moon.jpg");
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
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 1000.0f);

        shaderEsfera.setMat4("view", glm::value_ptr(view));
        shaderEsfera.setMat4("projection", glm::value_ptr(projection));

        shaderEsfera.setVec3("p0", glm::value_ptr(lightPos));
        shaderEsfera.setVec3("v_pos", glm::value_ptr(cameraPos));
        shaderEsfera.setVec3("La", glm::value_ptr(La));
        shaderEsfera.setVec3("Ld", glm::value_ptr(Ld));
        shaderEsfera.setVec3("Le", glm::value_ptr(Le));
        
        // ========================================================
        // Dibujar SOL
        sun.Draw(shaderEsfera.ID, currentTime, glm::vec3(0.0f), glm::vec3(2.0f));

        // =======================================================
        // Dibujar los planetas

        // MERCURIO
        float xM = sin(currentTime * orbitalSpeedM) * orbitRadiusM;
        float zM = cos(currentTime * orbitalSpeedM) * orbitRadiusM;
        mercury.Draw(shaderEsfera.ID, currentTime, glm::vec3(xM, 0.0f, zM), glm::vec3(0.15f));
        
        // VENUS
        float xV = sin(currentTime * orbitalSpeedV) * orbitRadiusV;
        float zV = cos(currentTime * orbitalSpeedV) * orbitRadiusV;
        venus.Draw(shaderEsfera.ID, currentTime, glm::vec3(xV, 0.0f, zV), glm::vec3(0.38f));
        
        // TIERRA
        float xE = sin(currentTime * orbitalSpeedE) * orbitRadiusE;
        float zE = cos(currentTime * orbitalSpeedE) * orbitRadiusE;
        earth.Draw(shaderEsfera.ID, currentTime, glm::vec3(xE, 0.0f, zE), glm::vec3(0.4f));
        
        // MARTE
        float xMa = sin(currentTime * orbitalSpeedMa) * orbitRadiusMa;
        float zMa = cos(currentTime * orbitalSpeedMa) * orbitRadiusMa;
        mars.Draw(shaderEsfera.ID, currentTime, glm::vec3(xMa, 0.0f, zMa), glm::vec3(0.21f));
        
        // JÚPITER
        float xJ = sin(currentTime * orbitalSpeedJ) * orbitRadiusJ;
        float zJ = cos(currentTime * orbitalSpeedJ) * orbitRadiusJ;
        jupyter.Draw(shaderEsfera.ID, currentTime, glm::vec3(xJ, 0.0f, zJ), glm::vec3(1.65f));
        
        // SATURNO
        float xS = sin(currentTime * orbitalSpeedS) * orbitRadiusS;
        float zS = cos(currentTime * orbitalSpeedS) * orbitRadiusS;
        saturn.Draw(shaderEsfera.ID, currentTime, glm::vec3(xS, 0.0f, zS), glm::vec3(1.4f));
        
        // URANO
        float xU = sin(currentTime * orbitalSpeedU) * orbitRadiusU;
        float zU = cos(currentTime * orbitalSpeedU) * orbitRadiusU;
        uranus.Draw(shaderEsfera.ID, currentTime, glm::vec3(xU, 0.0f, zU), glm::vec3(0.55f));
        
        // NEPTUNO
        float xN = sin(currentTime * orbitalSpeedN) * orbitRadiusN;
        float zN = cos(currentTime * orbitalSpeedN) * orbitRadiusN;
        neptune.Draw(shaderEsfera.ID, currentTime, glm::vec3(xN, 0.0f, zN), glm::vec3(0.52f));


         // LUNA: orbita alrededor de la Tierra
        // La posición de la Luna es la posición de la Tierra más un desplazamiento circular
        float moonX = xE + sin(currentTime * moonOrbitalSpeed) * moonOrbitRadius;
        float moonZ = zE + cos(currentTime * moonOrbitalSpeed) * moonOrbitRadius;
        moon.Draw(shaderEsfera.ID, currentTime, glm::vec3(moonX, 0.0f, moonZ), glm::vec3(0.1f)); // La Luna es más pequeña

        app.refrescar();
    }

    return 0;
}