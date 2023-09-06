#version 330

in float ild;
in vec2 texCoord;
uniform sampler2D tex;

out vec4 fragColor;

void main() {
  vec4 texColor = texture(tex, texCoord);
  vec3 invertedColor = 1.0 - texColor.rgb;
  fragColor = vec4(invertedColor * ild, texColor.a);
}
