#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>

//LEctura de archivos
#include <fstream>
#include <sstream>
#include <string>

// Definición de la Esfera
#define N_SUBDIVISIONES 6
#define RADIO_ESFERA 1.0f

//Tamaño de la matriz de vertices: 4 caras * 4^N * 3 vértices por triángulo * 6 datos (x,y,z, nx,ny,nz)
#define VERTEX_ARRAY_SIZE (4 * (int)pow(4, N_SUBDIVISIONES) * 3 * 6)

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Variables de Iluminación (Tal cual tus diapositivas)
float p0[] = { 0.9f, 0.9f, 0.9f }; // Posición Luz
float v_pos[] = { 0.0f, 1.0f, 3.0f }; // Posición Cámara

// Matriz de Incidencia (L)
float La[]={0.2f,0.2f,0.2f};
float Ld[]={1.0f,1.0f,1.0f};
float Le[]={1.0f,1.0f,1.0f};
// Matriz de Reflexión (R[K])
float Ka[]={0.0f,0.1f,0.3f};
float Kd[]={0.0f,0.5f,0.8f};
float Ke[]={0.5f,0.5f,0.5f};

//Coeficiente de brillo
float alpha = 10.0f;

std::string leerShader(const char* ruta) {
    std::string contenido;
    std::ifstream archivo;
    // Configuramos para que el stream pueda lanzar excepciones
    archivo.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        archivo.open(ruta);
        std::stringstream stream;
        stream << archivo.rdbuf();
        archivo.close();
        contenido = stream.str();
    } catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::ARCHIVO_NO_LEIDO: " << ruta << std::endl;
    }
    return contenido;
}

// SHADERS
std::string vertexCode = leerShader("vertex_shader.glsl");
std::string fragmentCode = leerShader("fragment_shader.glsl");
// SHADERS ACTUALIZADOS (Phong Shading - Per Pixel)
const char *vertexShaderSource = vertexCode.c_str();

const char *fragmentShaderSource = fragmentCode.c_str();

// ===================================================================
// Construccion de la Esfera
// ===================================================================
struct Vec3 { float x, y, z; };

Vec3 sumar(Vec3 a, Vec3 b) { 
    return {a.x+b.x, a.y+b.y, a.z+b.z};
}

Vec3 proyectar(Vec3 v, float r) {
    float len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    return { (v.x/len)*r, (v.y/len)*r, (v.z/len)*r };
}

// Recursión
void subdividir(Vec3 v1, Vec3 v2, Vec3 v3, int depth, float r, float* buf, int& i) {
    if (depth == 0) {
        // Guardamos los 3 vértices del triángulo uno por uno
        // Vertice 1
        buf[i++] = v1.x; buf[i++] = v1.y; buf[i++] = v1.z; // Pos
        Vec3 n1 = proyectar(v1, 1.0f); 
        buf[i++] = n1.x; buf[i++] = n1.y; buf[i++] = n1.z; // Norm
        // Vertice 2
        buf[i++] = v2.x; buf[i++] = v2.y; buf[i++] = v2.z; 
        Vec3 n2 = proyectar(v2, 1.0f);
        buf[i++] = n2.x; buf[i++] = n2.y; buf[i++] = n2.z;
        // Vertice 3
        buf[i++] = v3.x; buf[i++] = v3.y; buf[i++] = v3.z; 
        Vec3 n3 = proyectar(v3, 1.0f);
        buf[i++] = n3.x; buf[i++] = n3.y; buf[i++] = n3.z;
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

void identidad(float* m) { 
    for(int i=0;i<16;i++) m[i]=0; 
    m[0]=1; m[5]=1; m[10]=1; m[15]=1; 
}
void vista(float* m, float z) { 
    identidad(m); m[14] = -z; 
}
void proyeccion(float* m, float fov, float aspect, float n, float f) {
    identidad(m); float t = tan(fov/2);
    // Matriz de Proyección Perspectiva
    m[0]=1/(aspect*t);
    m[5]=1/t;
    m[10]=-(f+n)/(f-n);
    m[11]=-1;
    m[14]=-(2*f*n)/(f-n);
    m[15]=0;
}

// ===================================================================
// MAIN
// ===================================================================
int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Esfera Iluminada Phong", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();

    // Shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Buffer
    float* vertices = new float[VERTEX_ARRAY_SIZE];
    generarEsfera(vertices); // Llenar datos

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_ARRAY_SIZE * sizeof(float), vertices, GL_STATIC_DRAW);
    
    // Posición (0) y Normal (1)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);
    glUseProgram(shaderProgram);

    // Enviar Uniforms
    glUniform3fv(glGetUniformLocation(shaderProgram, "p0"), 1, p0);
    glUniform3fv(glGetUniformLocation(shaderProgram, "v_pos"), 1, v_pos);
    glUniform3fv(glGetUniformLocation(shaderProgram, "La"), 1, La);
    glUniform3fv(glGetUniformLocation(shaderProgram, "Ld"), 1, Ld);
    glUniform3fv(glGetUniformLocation(shaderProgram, "Le"), 1, Le);
    glUniform3fv(glGetUniformLocation(shaderProgram, "Ka"), 1, Ka);
    glUniform3fv(glGetUniformLocation(shaderProgram, "Kd"), 1, Kd);
    glUniform3fv(glGetUniformLocation(shaderProgram, "Ke"), 1, Ke);
    glUniform1f(glGetUniformLocation(shaderProgram, "alpha"), alpha);

    // Matrices Estáticas
    float m_model[16], m_view[16], m_proj[16];
    identidad(m_model);
    vista(m_view, 3.0f);
    proyeccion(m_proj, 0.8f, 800.0f/600.0f, 0.1f, 100.0f);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, m_model);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, m_view);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, m_proj);

    while(!glfwWindowShouldClose(window)) {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        // Dibujamos: Tamaño total / 6 floats por vertice = Numero total de vertices
        glDrawArrays(GL_TRIANGLES, 0, VERTEX_ARRAY_SIZE / 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    delete[] vertices;
    return 0;
}