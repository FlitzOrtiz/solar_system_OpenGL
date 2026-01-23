#ifndef VENTANA_H
#define VENTANA_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

class Ventana {
public:
    GLFWwindow* window;
    Ventana(int ancho, int alto, const char* titulo);
    ~Ventana();
    bool debeCerrar();
    void refrescar();
};

#endif