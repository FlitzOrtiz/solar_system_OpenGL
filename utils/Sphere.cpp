// =========================================================
// IMPORTANTE: Ruta relativa para encontrar el header
// ".." significa subir un nivel, luego entrar a "model"
// =========================================================
#include "../model/Sphere.h" 

#include <GL/glew.h> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <vector>

Sphere::Sphere(float speed) {
    selfRotationSpeed = speed;
    currentAngle = 0.0f;
    Ka = glm::vec3(0.1f);
    Kd = glm::vec3(0.5f);
    Ke = glm::vec3(1.0f);
    alpha = 32.0f;
    generateGeometry();
    setupMesh();
}

Sphere::~Sphere() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &normalVBO);
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
        vertices.push_back(a); vertices.push_back(b); vertices.push_back(c);
        normals.push_back(normalizeToSphere(a));
        normals.push_back(normalizeToSphere(b));
        normals.push_back(normalizeToSphere(c));
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
    vertices.clear(); normals.clear();
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

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(1);
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

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);
}