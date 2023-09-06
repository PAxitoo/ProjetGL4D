#version 330

in float ild;
in vec2 texCoord;
uniform sampler2D tex;

out vec4 fragColor;

void main() {
  vec4 texColor = texture(tex, texCoord);
  fragColor = vec4(texColor.rgb * ild, texColor.a);
}
