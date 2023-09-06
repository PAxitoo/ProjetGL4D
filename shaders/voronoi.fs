#version 330

uniform int nb_sites;
uniform vec2 coords[500];
uniform vec4 colors[500];
uniform sampler2D tex;

in vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
    float dmin = distance(vsoTexCoord, coords[0]);
    int imin = 0;

    for(int i = 0; i < nb_sites; i++) {
        float d = distance(vsoTexCoord, coords[i]);
        if(d < dmin) {
            imin = i;
            dmin = d;
        }
    }

    // Affichage d'un point au milieu d'une aire terminée
    if(dmin < 0.003) {
        fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        fragColor = colors[imin];
    }
}
