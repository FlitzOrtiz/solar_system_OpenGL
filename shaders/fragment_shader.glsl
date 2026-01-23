#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 p0;
uniform vec3 v_pos;
uniform vec3 La, Ld, Le;
uniform vec3 Ka, Kd, Ke;
uniform float alpha;

void main() {
    vec3 n = normalize(Normal);
    vec3 l = normalize(p0 - FragPos);
    vec3 v = normalize(v_pos - FragPos);
    vec3 r = reflect(-l, n);

    float dist = length(FragPos - p0);
    float dist2 = dist * dist;
    if(dist2 < 0.01) dist2 = 0.0001;

    vec3 Ia = Ka * La;
    vec3 Id = (Kd * Ld * max(dot(n, l), 0.0)) / dist2;
    vec3 Ie = (Ke * Le * pow(max(dot(r, v), 0.0), alpha)) / dist2;

    FragColor = vec4(Ia + Id + Ie, 1.0);
}