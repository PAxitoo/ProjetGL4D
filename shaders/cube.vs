/* TP6 */

#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 proj;
uniform mat4 modview;
uniform mat4 view;

out vec4 vsoColor;
out float il;

const vec3 Ld = vec3(0., -.7, -.7);


void main(void) {
    vec3 n = (transpose(inverse(modview)) * vec4(normal, 0.)).xyz;
    il = clamp(dot(n, -Ld), 0., 1.);

    gl_Position = proj * view * modview * vec4(position, 1.);

    vsoColor = vec4(texCoord, 0.5, 1);
}
