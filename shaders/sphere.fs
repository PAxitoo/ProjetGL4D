#version 330 core

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragPosition;

out vec4 fragColor;

uniform sampler2D tex;

const int MAX_LIGHTS = 4; // Nombre maximum de sources lumineuses

struct Light {
    vec3 position;
    vec4 color;
};

uniform Light lights[MAX_LIGHTS]; // Tableau des sources lumineuses
uniform int numLights; // Nombre de sources lumineuses actives

uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;

void main()
{
    vec4 ambientColor = vec4(0.0);
    vec4 diffuseColor = vec4(0.0);
    vec4 specularColor = vec4(0.0);

    for (int i = 0; i < numLights; i++) {
        Light light = lights[i];
        
        // Calcul de la luminosité ambiante pour chaque source lumineuse
        ambientColor += light.color * ambientStrength;
        
        // Calcul de la direction et de l'intensité de la lumière diffuse pour chaque source lumineuse
        vec3 lightDir = normalize(light.position - fragPosition);
        float diff = max(dot(fragNormal, lightDir), 0.0);
        diffuseColor += light.color * diff;
        
        // Calcul de la direction de la vue de la caméra
        vec3 viewDir = normalize(-fragPosition);
        
        // Calcul de la réflexion spéculaire pour chaque source lumineuse
        vec3 reflectDir = reflect(-lightDir, fragNormal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        specularColor += light.color * spec * specularStrength;
    }

    vec4 texColor = texture(tex, fragTexCoord);
    
    // Couleur finale du fragment en combinant la couleur de texture et les composantes d'éclairage
    fragColor = texColor * (ambientColor + diffuseColor + specularColor);
}
