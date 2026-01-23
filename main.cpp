#include "model/Ventana.h"
#include "model/Shader.h"
#include <cmath>
#include <iostream>

// --- Configuración de la Esfera ---
#define N_SUBDIVISIONES 6
#define RADIO_ESFERA 1.0f
// 4 caras iniciales * 4^N triángulos por cara * 3 vértices * 6 datos (pos + norm)
#define VERTEX_ARRAY_SIZE (4 * (int)pow(4, N_SUBDIVISIONES) * 3 * 6)

// --- Parámetros de Transformación ---
float posEsfera[]    = { 0.0f, 0.0f, 0.0f };
float escalaEsfera[] = { 1.0f, 1.0f, 1.0f };
float rotEsfera[]    = { 0.0f, 0.0f, 0.0f }; 

// --- Parámetros de Iluminación ---
float p0[]    = { 1.2f, 1.2f, 2.0f }; // Posición de la luz
float v_pos[] = { 0.0f, 0.0f, 3.0f }; // Posición de la cámara
float La[]={0.2f, 0.2f, 0.2f}, Ld[]={1.0f, 1.0f, 1.0f}, Le[]={1.0f, 1.0f, 1.0f};
float Ka[]={0.1f, 0.1f, 0.1f}, Kd[]={0.0f, 0.5f, 0.8f}, Ke[]={1.0f, 1.0f, 1.0f};
float alpha = 32.0f;

// --- Estructuras y Funciones Geométricas ---
struct Vec3 { float x, y, z; };
Vec3 sumar(Vec3 a, Vec3 b) { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
Vec3 proyectar(Vec3 v, float r) {
    float len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    return { (v.x/len)*r, (v.y/len)*r, (v.z/len)*r };
}

void subdividir(Vec3 v1, Vec3 v2, Vec3 v3, int depth, float r, float* buf, int& i) {
    if (depth == 0) {
        // Vértice 1: Posición y Normal
        buf[i++] = v1.x; buf[i++] = v1.y; buf[i++] = v1.z;
        Vec3 n1 = proyectar(v1, 1.0f); buf[i++] = n1.x; buf[i++] = n1.y; buf[i++] = n1.z;
        // Vértice 2
        buf[i++] = v2.x; buf[i++] = v2.y; buf[i++] = v2.z;
        Vec3 n2 = proyectar(v2, 1.0f); buf[i++] = n2.x; buf[i++] = n2.y; buf[i++] = n2.z;
        // Vértice 3
        buf[i++] = v3.x; buf[i++] = v3.y; buf[i++] = v3.z;
        Vec3 n3 = proyectar(v3, 1.0f); buf[i++] = n3.x; buf[i++] = n3.y; buf[i++] = n3.z;
        return;
    }
    Vec3 m1 = proyectar(sumar(v1,v2), r);
    Vec3 m2 = proyectar(sumar(v2,v3), r);
    Vec3 m3 = proyectar(sumar(v3,v1), r);
    subdividir(v1, m1, m3, depth-1, r, buf, i);
    subdividir(m1, v2, m2, depth-1, r, buf, i);
    subdividir(m3, m2, v3, depth-1, r, buf, i);
    subdividir(m1, m2, m3, depth-1, r, buf, i);
}

void generarEsfera(float* vertices) {
    Vec3 base[4] = { {1,1,1}, {1,-1,-1}, {-1,1,-1}, {-1,-1,1} };
    int idx[4][3] = { {0,2,1}, {0,1,3}, {0,3,2}, {1,2,3} };
    int k = 0;
    for(int j=0; j<4; j++) {
        Vec3 v1 = proyectar(base[idx[j][0]], RADIO_ESFERA);
        Vec3 v2 = proyectar(base[idx[j][1]], RADIO_ESFERA);
        Vec3 v3 = proyectar(base[idx[j][2]], RADIO_ESFERA);
        subdividir(v1, v2, v3, N_SUBDIVISIONES, RADIO_ESFERA, vertices, k);
    }
}

// --- Matrices Auxiliares ---
void identidad(float* m) { for(int i=0;i<16;i++) m[i]=0; m[0]=1; m[5]=1; m[10]=1; m[15]=1; }
void vista(float* m, float z) { identidad(m); m[14] = -z; }
void proyeccion(float* m, float fov, float aspect, float n, float f) {
    identidad(m); float t = tan(fov/2);
    m[0]=1/(aspect*t); m[5]=1/t; m[10]=-(f+n)/(f-n); m[11]=-1; m[14]=-(2*f*n)/(f-n); m[15]=0;
}

int main() {
    // 1. Inicializar Ventana (GLFW/GLEW/Profundidad)
    Ventana app(800, 600, "Shader Phong - Arquitectura Modular");

    // 2. Cargar Shaders
    Shader shaderEsfera("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    // 3. Preparar Geometría
    float* vertices = new float[VERTEX_ARRAY_SIZE];
    generarEsfera(vertices);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_ARRAY_SIZE * sizeof(float), vertices, GL_STATIC_DRAW);

    // Atributo 0: Posición (x,y,z) | Atributo 1: Normal (nx,ny,nz)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 4. Matrices de Cámara Estáticas
    float m_view[16], m_proj[16];
    vista(m_view, 3.5f);
    proyeccion(m_proj, 0.78f, 800.0f/600.0f, 0.1f, 100.0f);

    // --- Bucle de Renderizado ---
    while (!app.debeCerrar()) {
        // Input simple
        if (glfwGetKey(app.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(app.window, true);

        // Limpiar pantalla
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderEsfera.use();

        // Animación simple (opcional)
        rotEsfera[1] += 0.01f; // Rotar en Y

        // Enviar Uniforms de Transformación (GPU calcula la matriz Model)
        shaderEsfera.setVec3("uDesplazamiento", posEsfera);
        shaderEsfera.setVec3("uEscala", escalaEsfera);
        shaderEsfera.setVec3("uAngulos", rotEsfera);

        // Enviar Matrices de Cámara
        shaderEsfera.setMat4("view", m_view);
        shaderEsfera.setMat4("projection", m_proj);

        // Enviar Uniforms de Iluminación
        shaderEsfera.setVec3("p0", p0);
        shaderEsfera.setVec3("v_pos", v_pos);
        shaderEsfera.setVec3("La", La); shaderEsfera.setVec3("Ld", Ld); shaderEsfera.setVec3("Le", Le);
        shaderEsfera.setVec3("Ka", Ka); shaderEsfera.setVec3("Kd", Kd); shaderEsfera.setVec3("Ke", Ke);
        shaderEsfera.setFloat("alpha", alpha);

        // Dibujar
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, VERTEX_ARRAY_SIZE / 6);

        app.refrescar();
    }

    // Limpieza
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    delete[] vertices;

    return 0;
}