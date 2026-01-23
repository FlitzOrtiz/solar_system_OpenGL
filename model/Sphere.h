#ifndef SPHERE_H
#define SPHERE_H

#include <vector>
#include <glm/glm.hpp>

class Sphere {
private:
    unsigned int VAO, VBO, normalVBO;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    
    // Propiedades de movimiento
    float selfRotationSpeed;
    float currentAngle;
    
    // Propiedades del Material
    glm::vec3 Ka;
    glm::vec3 Kd;
    glm::vec3 Ke;
    float alpha;

    const int SUBDIVISION_LEVEL = 5;

    glm::vec3 normalizeToSphere(glm::vec3 p);
    void subdivide(glm::vec3 a, glm::vec3 b, glm::vec3 c, int depth);
    void generateGeometry();
    void setupMesh();

public:
    Sphere(float speed = 1.0f);
    ~Sphere();
    void setMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);
    void Draw(unsigned int shaderProgram, float time, glm::vec3 position, glm::vec3 scale);
};

#endif