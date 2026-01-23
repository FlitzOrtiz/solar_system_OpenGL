#include "../model/Ventana.h"

Ventana::Ventana(int ancho, int alto, const char* titulo) {
    if (!glfwInit()) exit(-1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(ancho, alto, titulo, NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glewInit();
    glEnable(GL_DEPTH_TEST);
}

Ventana::~Ventana() { glfwTerminate(); }
bool Ventana::debeCerrar() { return glfwWindowShouldClose(window); }
void Ventana::refrescar() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}