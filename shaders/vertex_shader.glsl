#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord; // Coordenadas UV

uniform vec3 uDesplazamiento, uEscala, uAngulos;        
uniform mat4 view, projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
    mat4 t = mat4(1.0, 0.0, 0.0, 
                  0.0, 0.0, 1.0, 
                  0.0, 0.0, 0.0, 
                  0.0, 1.0, 0.0, 
                  uDesplazamiento.x, uDesplazamiento.y, uDesplazamiento.z, 1.0);

    mat4 s = mat4(uEscala.x, 0.0, 0.0, 0.0, 
                  0.0, uEscala.y, 0.0, 0.0, 
                  0.0, 0.0, uEscala.z, 0.0, 
                  0.0, 0.0, 0.0, 1.0);
    
    // (Tus matrices de rotación rx, ry, rz se mantienen igual)
    float cx = cos(uAngulos.x);
    float sx = sin(uAngulos.x);
    mat4 rx = mat4(1.0, 0.0, 0.0, 0.0, 
                   0.0, cx, sx, 0.0, 
                   0.0, -sx, cx, 0.0, 
                   0.0, 0.0, 0.0, 1.0);

    float cy = cos(uAngulos.y);
    float sy = sin(uAngulos.y);
    mat4 ry = mat4(cy, 0.0, -sy, 0.0, 
                   0.0, 1.0, 0.0, 0.0, 
                   sy, 0.0, cy, 0.0, 
                   0.0, 0.0, 0.0, 1.0);

    float cz = cos(uAngulos.z);
    float sz = sin(uAngulos.z);
    mat4 rz = mat4(cz, sz, 0.0, 0.0, 
                   -sz, cz, 0.0, 0.0, 
                   0.0, 0.0, 1.0, 0.0, 
                   0.0, 0.0, 0.0, 1.0);
    
    mat4 model = t * (rz * ry * rx) * s;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(rz * ry * rx) * aNormal; 
    TexCoord = aTexCoord; // Pasar UV al fragment shader
}