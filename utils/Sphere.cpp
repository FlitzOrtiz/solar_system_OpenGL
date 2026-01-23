#include "../model/Sphere.h" 

#include <GL/glew.h> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <vector>
#include <iostream>

// Definición para que stb_image funcione como implementación [cite: 156, 158]
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

Sphere::Sphere(float speed) {
    selfRotationSpeed = speed;
    currentAngle = 0.0f;
    Ka = glm::vec3(0.1f);
    Kd = glm::vec3(0.5f);
    Ke = glm::vec3(1.0f);
    alpha = 32.0f;
    textureID = 0; 
    generateGeometry();
    setupMesh();
}

Sphere::~Sphere() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &normalVBO);
    glDeleteBuffers(1, &texVBO); 
    if (textureID != 0) glDeleteTextures(1, &textureID);
}

// Carga de textura optimizada según el manual [cite: 152, 153]
void Sphere::loadTexture(const char* path) {
    glGenTextures(1, &textureID); // Generar ID [cite: 169]
    glBindTexture(GL_TEXTURE_2D, textureID); // Vincular textura [cite: 175, 182]

    // ConfigurarWrapping y Filtering para evitar artefactos [cite: 85, 116, 142]
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Corregir la inversión del eje Y típica de los archivos de imagen [cite: 337, 339]
    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    // Cargar datos de la imagen [cite: 161, 163]
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data) {
        // Determinar el formato basado en el número de canales (RGB o RGBA) 
        GLenum format;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        // Generar la textura en la GPU con el formato detectado [cite: 178, 211]
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); // Generar mipmaps automáticamente [cite: 194, 195]
    } else {
        std::cout << "ERROR::SPHERE::TEXTURE_FAILED_AT_PATH: " << path << std::endl;
    }

    stbi_image_free(data); // Liberar memoria del CPU tras la carga [cite: 196, 197]
}

void Sphere::setMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess) {
    this->Ka = ambient;
    this->Kd = diffuse;
    this->Ke = specular;
    this->alpha = shininess;
}

glm::vec3 Sphere::normalizeToSphere(glm::vec3 p) {
    if (glm::length(p) < 0.0001f) return p;
    return glm::normalize(p);
}

void Sphere::subdivide(glm::vec3 a, glm::vec3 b, glm::vec3 c, int depth) {
    if (depth <= 0) {
        vertices.push_back(a); 
        vertices.push_back(b); 
        vertices.push_back(c);
        
        normals.push_back(normalizeToSphere(a));
        normals.push_back(normalizeToSphere(b));
        normals.push_back(normalizeToSphere(c));

        // Cálculo de UVs para mapeo equirectangular [cite: 45, 46, 51]
        for (auto& p : {a, b, c}) {
            float u = 0.5f + (atan2(p.z, p.x) / (2.0f * 3.14159265359f));
            float v = 0.5f - (asin(p.y) / 3.14159265359f);
            texCoords.push_back(glm::vec2(u, v));
        }
        return;
    }
    glm::vec3 ab = normalizeToSphere((a + b) / 2.0f);
    glm::vec3 bc = normalizeToSphere((b + c) / 2.0f);
    glm::vec3 ca = normalizeToSphere((c + a) / 2.0f);

    subdivide(a, ab, ca, depth - 1);
    subdivide(b, bc, ab, depth - 1);
    subdivide(c, ca, bc, depth - 1);
    subdivide(ab, bc, ca, depth - 1);
}

void Sphere::generateGeometry() {
    vertices.clear(); normals.clear(); texCoords.clear();
    glm::vec3 v0 = normalizeToSphere(glm::vec3(1.0f, 1.0f, 1.0f));
    glm::vec3 v1 = normalizeToSphere(glm::vec3(-1.0f, -1.0f, 1.0f));
    glm::vec3 v2 = normalizeToSphere(glm::vec3(-1.0f, 1.0f, -1.0f));
    glm::vec3 v3 = normalizeToSphere(glm::vec3(1.0f, -1.0f, -1.0f));

    std::vector<glm::vec3> tempV = { v0, v1, v2, v0, v2, v3, v0, v3, v1, v1, v3, v2 };
    
    for (size_t i = 0; i < tempV.size(); i += 3) {
        subdivide(tempV[i], tempV[i+1], tempV[i+2], SUBDIVISION_LEVEL);
    }
}

void Sphere::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &normalVBO);
    glGenBuffers(1, &texVBO); 

    glBindVertexArray(VAO);

    // 1. Posiciones (Location 0)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // 2. Normales (Location 1)
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(1);

    // 3. Coordenadas de Textura (Location 2) [cite: 232, 239]
    glBindBuffer(GL_ARRAY_BUFFER, texVBO);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(2);
}

void Sphere::Draw(unsigned int shaderProgram, float time, glm::vec3 position, glm::vec3 scale) {
    currentAngle = time * selfRotationSpeed;
    glm::vec3 rotationEuler = glm::vec3(0.0f, currentAngle, 0.0f);

    glUniform3fv(glGetUniformLocation(shaderProgram, "uDesplazamiento"), 1, glm::value_ptr(position));
    glUniform3fv(glGetUniformLocation(shaderProgram, "uAngulos"), 1, glm::value_ptr(rotationEuler));
    glUniform3fv(glGetUniformLocation(shaderProgram, "uEscala"), 1, glm::value_ptr(scale));

    glUniform3fv(glGetUniformLocation(shaderProgram, "Ka"), 1, glm::value_ptr(this->Ka));
    glUniform3fv(glGetUniformLocation(shaderProgram, "Kd"), 1, glm::value_ptr(this->Kd));
    glUniform3fv(glGetUniformLocation(shaderProgram, "Ke"), 1, glm::value_ptr(this->Ke));
    glUniform1f(glGetUniformLocation(shaderProgram, "alpha"), this->alpha);

    // Gestión de unidades de textura según el manual [cite: 291, 292, 293]
    if (textureID != 0) {
        glActiveTexture(GL_TEXTURE0); // Activar la unidad 0 antes de vincular 
        glBindTexture(GL_TEXTURE_2D, textureID);
        // Asignar el sampler del shader a la unidad 0 [cite: 254, 330]
        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);
}