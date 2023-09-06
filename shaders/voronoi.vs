#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec2 vsoTexCoord;

void main(void) {
    gl_Position = vec4(position, 1.0);
    vsoTexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}
