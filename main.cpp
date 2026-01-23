#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#define N_SUBDIVISIONES 6
#define RADIO_ESFERA 1.0f
#define VERTEX_ARRAY_SIZE (4 * (int)pow(4, N_SUBDIVISIONES) * 3 * 6)

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// --- Parámetros de Transformación ---
float posEsfera[]    = { 0.0f, 0.0f, 0.0f };
float escalaEsfera[] = { 1.0f, 1.0f, 1.0f };
float rotEsfera[]    = { 0.0f, 0.0f, 0.0f }; // En radianes

// --- Parámetros de Iluminación ---
float p0[] = { 0.9f, 0.9f, 0.9f }; 
float v_pos[] = { 0.0f, 1.0f, 3.0f }; 
float La[]={0.2f,0.2f,0.2f}, Ld[]={1.0f,1.0f,1.0f}, Le[]={1.0f,1.0f,1.0f};
float Ka[]={0.0f,0.1f,0.3f}, Kd[]={0.0f,0.5f,0.8f}, Ke[]={0.5f,0.5f,0.5f};
float alpha = 10.0f;

std::string leerShader(const char* ruta) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cout << "ERROR::SHADER::ARCHIVO_NO_LEIDO: " << ruta << std::endl;
        return "";
    }
    std::stringstream stream;
    stream << archivo.rdbuf();
    return stream.str();
}

// Estructuras para la esfera
struct Vec3 { float x, y, z; };
Vec3 sumar(Vec3 a, Vec3 b) { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
Vec3 proyectar(Vec3 v, float r) {
    float len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    return { (v.x/len)*r, (v.y/len)*r, (v.z/len)*r };
}

void subdividir(Vec3 v1, Vec3 v2, Vec3 v3, int depth, float r, float* buf, int& i) {
    if (depth == 0) {
        buf[i++] = v1.x; buf[i++] = v1.y; buf[i++] = v1.z;
        Vec3 n1 = proyectar(v1, 1.0f); buf[i++] = n1.x; buf[i++] = n1.y; buf[i++] = n1.z;
        buf[i++] = v2.x; buf[i++] = v2.y; buf[i++] = v2.z;
        Vec3 n2 = proyectar(v2, 1.0f); buf[i++] = n2.x; buf[i++] = n2.y; buf[i++] = n2.z;
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

// Matrices auxiliares
void identidad(float* m) { for(int i=0;i<16;i++) m[i]=0; m[0]=1; m[5]=1; m[10]=1; m[15]=1; }
void vista(float* m, float z) { identidad(m); m[14] = -z; }
void proyeccion(float* m, float fov, float aspect, float n, float f) {
    identidad(m); float t = tan(fov/2);
    m[0]=1/(aspect*t); m[5]=1/t; m[10]=-(f+n)/(f-n); m[11]=-1; m[14]=-(2*f*n)/(f-n); m[15]=0;
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Esfera con Matrices Manuales", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();

    // --- Carga de Shaders desde archivos ---
    std::string vCode = leerShader("shaders/vertex_shader.glsl");
    std::string fCode = leerShader("shaders/fragment_shader.glsl");
    const char* vShaderSource = vCode.c_str();
    const char* fShaderSource = fCode.c_str();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Buffer de datos
    float* vertices = new float[VERTEX_ARRAY_SIZE];
    generarEsfera(vertices);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_ARRAY_SIZE * sizeof(float), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);
    glUseProgram(shaderProgram);

    // Matrices de Cámara y Proyección
    float m_view[16], m_proj[16];
    vista(m_view, 3.0f);
    proyeccion(m_proj, 0.8f, (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);

    while(!glfwWindowShouldClose(window)) {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // --- ENVIAR UNIFORMS DE TRANSFORMACIÓN ---
        glUniform3fv(glGetUniformLocation(shaderProgram, "uDesplazamiento"), 1, posEsfera);
        glUniform3fv(glGetUniformLocation(shaderProgram, "uEscala"), 1, escalaEsfera);
        glUniform3fv(glGetUniformLocation(shaderProgram, "uAngulos"), 1, rotEsfera);

        // --- ENVIAR UNIFORMS DE CÁMARA ---
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, m_view);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, m_proj);

        // --- ENVIAR UNIFORMS DE ILUMINACIÓN ---
        glUniform3fv(glGetUniformLocation(shaderProgram, "p0"), 1, p0);
        glUniform3fv(glGetUniformLocation(shaderProgram, "v_pos"), 1, v_pos);
        glUniform3fv(glGetUniformLocation(shaderProgram, "La"), 1, La);
        glUniform3fv(glGetUniformLocation(shaderProgram, "Ld"), 1, Ld);
        glUniform3fv(glGetUniformLocation(shaderProgram, "Le"), 1, Le);
        glUniform3fv(glGetUniformLocation(shaderProgram, "Ka"), 1, Ka);
        glUniform3fv(glGetUniformLocation(shaderProgram, "Kd"), 1, Kd);
        glUniform3fv(glGetUniformLocation(shaderProgram, "Ke"), 1, Ke);
        glUniform1f(glGetUniformLocation(shaderProgram, "alpha"), alpha);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, VERTEX_ARRAY_SIZE / 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    delete[] vertices;
    return 0;
}