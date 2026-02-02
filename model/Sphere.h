#ifndef SPHERE_H
#define SPHERE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class Sphere {
public:
    // Nivel de subdivisión para la suavidad de la esfera
    const int SUBDIVISION_LEVEL = 6;

    // Constructor: recibe la velocidad de rotación propia
    Sphere(float speed = 1.0f);
    ~Sphere();

    // Configura las propiedades del material (Phong)
    void setMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);

    // Carga una imagen de textura desde un archivo (usando stb_image)
    void loadTexture(const char* path);

    // Dibuja la esfera aplicando transformaciones y materiales
    void Draw(unsigned int shaderProgram, float time, glm::vec3 position, glm::vec3 scale);

private:
    // Datos Geométricos
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords; // Coordenadas U, V

    // Objetos de OpenGL
    unsigned int VAO;
    unsigned int VBO, normalVBO, texVBO;
    unsigned int textureID; // ID de la textura cargada en la GPU

    // Estado de la Esfera
    float selfRotationSpeed;
    float currentAngle;

    // Propiedades del Material
    glm::vec3 Ka; // Ambiental
    glm::vec3 Kd; // Difusa
    glm::vec3 Ke; // Especular
    float alpha;  // Brillo (Shininess)

    // Funciones Internas de Construcción
    void generateGeometry();
    void subdivide(glm::vec3 a, glm::vec3 b, glm::vec3 c, int depth);
    void setupMesh();
    glm::vec3 normalizeToSphere(glm::vec3 p);
};

#endif