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
uniform sampler2D ourTexture; // La textura del planeta

void main() {
    // 1. Preparación de vectores (Phong Estándar)
    vec3 n = normalize(Normal);
    float d = length(p0 - FragPos); 
    vec3 l = normalize(p0 - FragPos);
    vec3 v = normalize(v_pos - FragPos);
    vec3 r = reflect(-l, n);

    vec3 texColor = texture(ourTexture, TexCoord).rgb;

    // 2. NORMALIZACIÓN BASADA EN EL PLANETA LEJANO
    float d_max = 40.52;      // Distancia de Neptuno en tu código
    float intensity_min = 0.09; // El brillo mínimo que quieres en Neptuno (9%)
    
    // Calculamos la potencia necesaria para que Neptuno se vea al 9%
    float lightPower = intensity_min * (d_max * d_max);
    
    // Aplicamos la atenuación física obligatoria
    float attenuation = lightPower / (d * d);

    // Clamp para que Mercurio no se vea blanco puro (máximo 1.2 veces la luz original)
    attenuation = clamp(attenuation, 0.0, 1.2);

    // 3. COMPONENTES DE PHONG
    vec3 ambient = Ka * La * texColor;
    
    float diffFactor = max(dot(n, l), 0.0);
    vec3 diffuse = (Kd * Ld * diffFactor * texColor) * attenuation;
    
    float specFactor = pow(max(dot(r, v), 0.0), alpha);
    vec3 specular = (Ke * Le * specFactor) * attenuation;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}