#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord; // Coordenadas enviadas desde el nuevo Vertex Shader

uniform vec3 p0;      // Posición luz
uniform vec3 v_pos;   // Posición cámara
uniform vec3 La, Ld, Le; 
uniform vec3 Ka, Kd, Ke; 
uniform float alpha; 
uniform sampler2D ourTexture; // La textura del planeta [cite: 259]

void main() {
    vec3 n = normalize(Normal);
    vec3 l = normalize(p0 - FragPos);
    vec3 v = normalize(v_pos - FragPos);
    vec3 r = reflect(-l, n);

    // 1. Muestrear el color de la textura [cite: 262, 263]
    vec4 texColor = texture(ourTexture, TexCoord);

    // 2. Cálculo de iluminación sin división por distancia (para que no se apague)
    vec3 ambient = Ka * La * texColor.rgb;
    
    // Difusa: Si es el Sol (distancia casi 0), forzamos iluminación completa
    float dist = length(p0 - FragPos);
    float diff = max(dot(n, l), 0.0);
    
    if(dist < 0.1) diff = 1.0; // El Sol siempre brilla [cite: 23]

    vec3 diffuse = Kd * Ld * diff * texColor.rgb;
    vec3 specular = Ke * Le * pow(max(dot(r, v), 0.0), alpha);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}