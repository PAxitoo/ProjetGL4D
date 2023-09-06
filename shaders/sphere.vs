#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragPosition;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 mod;

const int MAX_LIGHTS = 4; // Nombre maximum de sources lumineuses

struct Light {
    vec3 position;
    vec4 color;
};

uniform Light lights[MAX_LIGHTS]; // Tableau des sources lumineuses
uniform int numLights; // Nombre de sources lumineuses actives

void main()
{
    gl_Position = proj * view * mod * vec4(position, 1.0);
    fragTexCoord = texCoord;
    fragNormal = normalize(mat3(transpose(inverse(mod))) * normal);
    fragPosition = vec3(mod * vec4(position, 1.0));
}
