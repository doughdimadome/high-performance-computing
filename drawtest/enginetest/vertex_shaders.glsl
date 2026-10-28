#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Transformeer de positie naar World Space
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Transformeer de normaalvector naar World Space
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    TexCoord = aTexCoord;

    // OpenGL voert automatisch de Perspective Divide (x/w, y/w, z/w) uit
    gl_Position = projection * view * vec4(FragPos, 1.0);
}