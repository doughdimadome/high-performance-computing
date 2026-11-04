#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    vec3 lightPos = vec3(0.2f, -2.0f, 0.0f);
    vec3 lightColor = vec3(0.5f, 0.5f, 0.5f);
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
    // In de fragment shader:
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = lightColor * (spec * material.specular);

    // Gecombineerde kleur (Ambient + Diffuse + Specular) * Texture
    vec3 result = (ambient + diffuse) * baseColor + specular;
    FragColor = vec4(result, 1.0);

    struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    };

    uniform Material material;
}