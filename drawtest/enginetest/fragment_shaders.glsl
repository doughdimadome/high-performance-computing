#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 baseColor = texture(ourTexture, TexCoord).rgb;

    // 1. Ambient Lighting 
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // 2. Diffuse Lighting 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * 0.5 * lightColor;

    // 3. Specular Lighting 
    float specularStrength = 0.3;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // Shininess = 32
    vec3 specular = specularStrength * spec * lightColor;

    // Gecombineerde kleur (Ambient + Diffuse + Specular) * Texture
    vec3 result = (ambient + diffuse) * baseColor + specular;
    FragColor = vec4(result, 1.0);
}