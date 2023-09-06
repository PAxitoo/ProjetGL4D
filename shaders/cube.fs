/* TP6 */

#version 330

in  vec4 vsoColor;
out vec4 maSortie;

in float il;

void main(void) {
    maSortie = il * vsoColor;
}
