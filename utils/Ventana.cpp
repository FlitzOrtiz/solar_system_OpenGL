#include "../model/Ventana.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glewInit();
    glEnable(GL_DEPTH_TEST);
}

Ventana::~Ventana() {
    glfwTerminate();
}

bool Ventana::debeCerrar() { 
    return glfwWindowShouldClose(window);
}

void Ventana::refrescar() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}