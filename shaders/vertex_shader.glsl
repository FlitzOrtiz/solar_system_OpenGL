#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

// Tus nuevos Uniforms
uniform vec3 uDesplazamiento; 
uniform vec3 uEscala;         
uniform vec3 uAngulos;        

uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    // 1. Matriz de Traslación
    mat4 t = mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        uDesplazamiento.x, uDesplazamiento.y, uDesplazamiento.z, 1.0
    );

    // 2. Matriz de Escalamiento
    mat4 s = mat4(
        uEscala.x, 0.0, 0.0, 0.0,
        0.0, uEscala.y, 0.0, 0.0,
        0.0, 0.0, uEscala.z, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    // 3. Matrices de Rotación (X, Y, Z)
    float cx = cos(uAngulos.x); float sx = sin(uAngulos.x);
    mat4 rx = mat4(1.0, 0.0, 0.0, 0.0, 0.0, cx, sx, 0.0, 0.0, -sx, cx, 0.0, 0.0, 0.0, 0.0, 1.0);

    float cy = cos(uAngulos.y); float sy = sin(uAngulos.y);
    mat4 ry = mat4(cy, 0.0, -sy, 0.0, 0.0, 1.0, 0.0, 0.0, sy, 0.0, cy, 0.0, 0.0, 0.0, 0.0, 1.0);

    float cz = cos(uAngulos.z); float sz = sin(uAngulos.z);
    mat4 rz = mat4(cz, sz, 0.0, 0.0, -sz, cz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    
    mat4 rot = rz * ry * rx;

    // Construimos la matriz de modelo final: M = T * R * S
    mat4 model = t * rot * s;

    // Posición final para el rasterizador
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // Datos para el Fragment Shader (Iluminación)
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Transformamos la normal solo con la rotación para evitar errores de iluminación al trasladar
    Normal = mat3(rot) * aNormal; 
}