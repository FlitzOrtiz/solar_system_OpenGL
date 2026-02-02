#include "model/Ventana.h"
#include "model/Shader.h"
#include "model/Sphere.h" 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Configuración de la ventana
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

// Parámetros de la cámara
float cameraAngleX = 0.0f; // Ángulo inicial en el eje X
float cameraDistance = 30.0f; // Distancia inicial desde el origen
glm::vec3 cameraPos;
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // Mirando al origen
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); 
float traslationSpeed = 60.0f; 

glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 La(1.0f, 1.0f, 1.0f); 
glm::vec3 Ld(1.0f, 1.0f, 1.0f); 
glm::vec3 Le(1.0f, 1.0f, 1.0f); 

void updateCamera() { // Actualizar la posición de la cámara según el ángulo y la distancia
    cameraPos.x = cameraDistance * sin(cameraAngleX);
    cameraPos.y = cameraDistance * 0.5f; 
    cameraPos.z = cameraDistance * cos(cameraAngleX);
}

void processInput(GLFWwindow* window, float deltaTime) { //deltaTime : tiempo entre frames
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
    Ventana app(SCR_WIDTH, SCR_HEIGHT, "Sistema Solar - Proyecto Final GPC");

    Shader sphereShader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    Sphere sun(0.5f);
    sun.setMaterial(
        glm::vec3(0.8f, 0.6f, 0.0f), 
        glm::vec3(1.0f, 0.9f, 0.0f), 
        glm::vec3(1.0f, 1.0f, 1.0f), 
        16.0f
    );
    // Cargamos la textura para el Sol
    sun.loadTexture("textures/2k_sun.jpg");

    // Configuracion de planetas

    // Radios de Orbitas
    float orbitRadiusM = 3.0f;    // Mercurio
    float orbitRadiusV = 5.5f;    // Venus
    float orbitRadiusE = 7.0f;    // Tierra
    float orbitRadiusMa = 10.0f;  // Marte
    float orbitRadiusJ = 15.0f;   // Júpiter
    float orbitRadiusS = 22.0f;   // Saturno
    float orbitRadiusU = 30.0f;   // Urano
    float orbitRadiusN = 40.0f;   // Neptuno
    
    // Velocidades Orbitales (ángulos por unidad de tiempo)
    float orbitalSpeedM = 2.0f;     // Mercurio
    float orbitalSpeedV = 1.25f;    // Venus 
    float orbitalSpeedE = 0.5f;     // Tierra
    float orbitalSpeedMa = 0.375f;  // Marte
    float orbitalSpeedJ = 0.125f;   // Júpiter
    float orbitalSpeedS = 0.075f;   // Saturno
    float orbitalSpeedU = 0.04f;    // Urano
    float orbitalSpeedN = 0.025f;   // Neptuno

    // Luna
    // Parámetros de la órbita de la Luna alrededor de la Tierra
    float moonOrbitRadius = 1.5f; // Distancia de la Luna a la Tierra
    float moonOrbitalSpeed = 5.0f; // Velocidad orbital de la Luna


    // Configuracion Materiales y Velocidades de Rotación Propia de los Planetas
    Sphere mercury(0.05f);
    mercury.setMaterial(
        glm::vec3(0.25f, 0.25f, 0.25f), 
        glm::vec3(0.35f, 0.35f, 0.35f), 
        glm::vec3(0.15f, 0.15f, 0.15f), 
        1.0f
    );

    Sphere venus(-0.02f);
    venus.setMaterial(
        glm::vec3(0.45f, 0.35f, 0.10f), 
        glm::vec3(0.50f, 0.40f, 0.15f), 
        glm::vec3(0.25f, 0.20f, 0.10f), 
        1.2f
    );


    Sphere earth(1.0f);
    earth.setMaterial(
        glm::vec3(0.0f, 0.05f, 0.10f), 
        glm::vec3(0.0f, 0.20f, 0.40f), 
        glm::vec3(0.25f, 0.25f, 0.25f), 
        1.5f
    );

    Sphere mars(0.95f);
    mars.setMaterial(
        glm::vec3(0.30f, 0.10f, 0.05f), 
        glm::vec3(0.40f, 0.15f, 0.05f), 
        glm::vec3(0.20f, 0.10f, 0.05f), 
        1.3f
    );

    Sphere jupyter(2.5f);
    jupyter.setMaterial(
        glm::vec3(0.40f, 0.20f, 0.05f),  
        glm::vec3(0.45f, 0.25f, 0.10f), 
        glm::vec3(0.25f, 0.25f, 0.25f), 
        2.0f
    );

    Sphere saturn(2.3f);
    saturn.setMaterial(
        glm::vec3(0.45f, 0.40f, 0.30f), 
        glm::vec3(0.50f, 0.45f, 0.35f), 
        glm::vec3(0.30f, 0.25f, 0.20f), 
        1.8f
    );


    // URANO
    Sphere uranus(1.5f);
    uranus.setMaterial(
        glm::vec3(0.20f, 0.30f, 0.40f), 
        glm::vec3(0.25f, 0.35f, 0.45f), 
        glm::vec3(0.15f, 0.25f, 0.35f), 
        1.6f
    );

    // NEPTUNO
    Sphere neptune(1.6f);
    neptune.setMaterial(
        glm::vec3(0.05f, 0.05f, 0.40f), 
        glm::vec3(0.10f, 0.10f, 0.50f), 
        glm::vec3(0.05f, 0.05f, 0.30f), 
        1.7f
    );

    // LUNA
    Sphere moon(0.5f); // Rotación sobre su eje
    moon.setMaterial(
        glm::vec3(0.10f, 0.10f, 0.10f), 
        glm::vec3(0.25f, 0.25f, 0.25f), 
        glm::vec3(0.05f, 0.05f, 0.05f), 
        0.5f
    );

    // Cargar las texturas de los planetas
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

        sphereShader.use();

        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 1000.0f);

        sphereShader.setMat4("view", glm::value_ptr(view));
        sphereShader.setMat4("projection", glm::value_ptr(projection));

        sphereShader.setVec3("p0", glm::value_ptr(lightPos));
        sphereShader.setVec3("v_pos", glm::value_ptr(cameraPos));
        sphereShader.setVec3("La", glm::value_ptr(La));
        sphereShader.setVec3("Ld", glm::value_ptr(Ld));
        sphereShader.setVec3("Le", glm::value_ptr(Le));
        
        // Dibujar SOL
        sun.Draw(sphereShader.shaderProgram, currentTime, glm::vec3(0.0f), glm::vec3(2.0f));

        // Dibujar los planetas

        // MERCURIO
        float xM = sin(currentTime * orbitalSpeedM) * orbitRadiusM;
        float zM = cos(currentTime * orbitalSpeedM) * orbitRadiusM;
        mercury.Draw(sphereShader.shaderProgram, currentTime, glm::vec3(xM, 0.0f, zM), glm::vec3(0.15f));
        
        // VENUS
        float xV = sin(currentTime * orbitalSpeedV) * orbitRadiusV;
        float zV = cos(currentTime * orbitalSpeedV) * orbitRadiusV;
        venus.Draw(sphereShader.shaderProgram, currentTime, glm::vec3(xV, 0.0f, zV), glm::vec3(0.38f));
        
        // TIERRA
        float xE = sin(currentTime * orbitalSpeedE) * orbitRadiusE;
        float zE = cos(currentTime * orbitalSpeedE) * orbitRadiusE;
        earth.Draw(sphereShader.shaderProgram, currentTime, glm::vec3(xE, 0.0f, zE), glm::vec3(0.4f));
        
        // MARTE
        float xMa = sin(currentTime * orbitalSpeedMa) * orbitRadiusMa;
        float zMa = cos(currentTime * orbitalSpeedMa) * orbitRadiusMa;
        mars.Draw(sphereShader.shaderProgram, currentTime, glm::vec3(xMa, 0.0f, zMa), glm::vec3(0.21f));
        
        // JÚPITER
        float xJ = sin(currentTime * orbitalSpeedJ) * orbitRadiusJ;
        float zJ = cos(currentTime * orbitalSpeedJ) * orbitRadiusJ;
        jupyter.Draw(sphereShader.shaderProgram, currentTime, glm::vec3(xJ, 0.0f, zJ), glm::vec3(1.65f));
        
        // SATURNO
        float xS = sin(currentTime * orbitalSpeedS) * orbitRadiusS;
        float zS = cos(currentTime * orbitalSpeedS) * orbitRadiusS;
        saturn.Draw(sphereShader.shaderProgram, currentTime, glm::vec3(xS, 0.0f, zS), glm::vec3(1.4f));
        
        // URANO
        float xU = sin(currentTime * orbitalSpeedU) * orbitRadiusU;
        float zU = cos(currentTime * orbitalSpeedU) * orbitRadiusU;
        uranus.Draw(sphereShader.shaderProgram, currentTime, glm::vec3(xU, 0.0f, zU), glm::vec3(0.55f));
        
        // NEPTUNO
        float xN = sin(currentTime * orbitalSpeedN) * orbitRadiusN;
        float zN = cos(currentTime * orbitalSpeedN) * orbitRadiusN;
        neptune.Draw(sphereShader.shaderProgram, currentTime, glm::vec3(xN, 0.0f, zN), glm::vec3(0.52f));


        // LUNA: orbita alrededor de la Tierra
        // La posición de la Luna es la posición de la Tierra más un desplazamiento circular
        float moonX = xE + sin(currentTime * moonOrbitalSpeed) * moonOrbitRadius;
        float moonZ = zE + cos(currentTime * moonOrbitalSpeed) * moonOrbitRadius;
        moon.Draw(sphereShader.shaderProgram, currentTime, glm::vec3(moonX, 0.0f, moonZ), glm::vec3(0.1f)); // La Luna es más pequeña

        app.refrescar();
    }

    return 0;
}