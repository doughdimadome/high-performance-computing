#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define BUFSIZE 256
#define ROTATION_SPEED 0.5f

int main() {
    // Initialiseer GLFW
    if (!glfwInit()) {
        return -1;
    }

    // Stel 4x oversampling in voor anti-aliasing (Opdracht 1.10)[cite: 1]
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "TINHPC - Gekleurde Draaiende Kubus", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialiseer GLEW
    if (glewInit() != GLEW_OK) {
        return -1;
    }

    // Hoekpunten van de kubus met posities en kleuren (Opdracht 2.4)[cite: 1]
    GLfloat vertices[] = {
         1,  1,  1, 1, 1, 1, // front top right, white[cite: 1]
         1, -1,  1, 1, 0, 1, // front bottom right, purple[cite: 1]
        -1, -1,  1, 0, 0, 1, // front bottom left, blue[cite: 1]
        -1,  1,  1, 0, 1, 1, // front top left, cyan[cite: 1]
         1,  1, -1, 1, 1, 0, // back top right, yellow[cite: 1]
         1, -1, -1, 1, 0, 0, // back bottom right, red[cite: 1]
        -1, -1, -1, 0, 0, 0, // back bottom left, black[cite: 1]
        -1,  1, -1, 0, 1, 0  // back top left, green[cite: 1]
    };

    // Indices van de 12 driehoeken voor dichte oppervlakken (Opdracht 2.2)[cite: 1]
    GLushort edges[] = {
        0, 1, 2, 2, 3, 0, // front[cite: 1]
        4, 5, 6, 6, 7, 4, // back[cite: 1]
        0, 4, 1, 1, 4, 5, // bottom[cite: 1]
        2, 6, 3, 3, 6, 7, // top[cite: 1]
        0, 3, 4, 4, 7, 3, // left[cite: 1]
        1, 2, 5, 5, 6, 2  // right[cite: 1]
    };

    // Vertex Array Object (VAO), Vertex Buffer Object (VBO) en Element Buffer Object (EBO) aanmaken
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edges), edges, GL_STATIC_DRAW);

    // Shaders compileren (Opdracht 1.3, 1.4, 2.5, 2.6)[cite: 1]
    const char *vsSource =
        R"(#version 330 core
        in vec3 pos;
        in vec3 col;
        out vec3 color;
        uniform mat4 projection;
        uniform mat4 translation;
        uniform mat4 rotation;
        uniform float rotationAngle;
        void main() {
            float s = sin(rotationAngle);
            float c = cos(rotationAngle);
            mat4 rotationY = mat4(c, 0, -s, 0,
                                  0, 1,  0, 0,
                                  s, 0,  c, 0,
                                  0, 0,  0, 1);
            color = col;
            gl_Position = projection * translation * rotation * rotationY * vec4(pos, 1.0);
        })";

    const char *fsSource = 
        R"(#version 330 core
        in vec3 color;
        out vec4 fragColor;
        void main() {
            fragColor = vec4(color, 1.0);
        })";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsSource, NULL);
    glCompileShader(vs);

    GLint status;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
    if (!status) {
        char log[BUFSIZE];
        glGetShaderInfoLog(vs, BUFSIZE, NULL, log);
        puts(log);
        exit(-1);
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsSource, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
    if (!status) {
        char log[BUFSIZE];
        glGetShaderInfoLog(fs, BUFSIZE, NULL, log);
        puts(log);
        exit(-1);
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glUseProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    // Attribuutpointers configureren voor positie en kleur (Opdracht 2.7)[cite: 1]
    GLint posLocation = glGetAttribLocation(program, "pos");
    glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 24, (void*)0);
    glEnableVertexAttribArray(posLocation);

    GLint colLocation = glGetAttribLocation(program, "col");
    glVertexAttribPointer(colLocation, 3, GL_FLOAT, GL_FALSE, 24, (void*)12);
    glEnableVertexAttribArray(colLocation);

    // Uniforms instellen (Rotatie rond X-as, Translatie, en Perspectiefprojectie met z-bufferbehoud)[cite: 1]
    GLint rotationLocation = glGetUniformLocation(program, "rotation");
    GLfloat c = cos(M_PI / 6);
    GLfloat s = sin(M_PI / 6);
    GLfloat rotation[] = {1, 0,  0, 0,
                          0, c, -s, 0,
                          0, s,  c, 0,
                          0, 0,  0, 1};
    glUniformMatrix4fv(rotationLocation, 1, GL_TRUE, rotation);

    GLint translationLocation = glGetUniformLocation(program, "translation");
    GLfloat translation[] = {1, 0, 0,  0,
                             0, 1, 0,  0,
                             0, 0, 1, -3,
                             0, 0, 0,  1};
    glUniformMatrix4fv(translationLocation, 1, GL_TRUE, translation);

    GLint projectionLocation = glGetUniformLocation(program, "projection");
    GLfloat projection[] = {1, 0,  0,  0,
                            0, 1,  0,  0,
                            0, 0, -1, -1,
                            0, 0, -1,  0};
    glUniformMatrix4fv(projectionLocation, 1, GL_TRUE, projection);

    GLint rotationAngleLocation = glGetUniformLocation(program, "rotationAngle");

    // Dieptetest inschakelen voor de z-buffer (Opdracht 3.3)[cite: 1]
    glEnable(GL_DEPTH_TEST);

    // Tekenlus (Render loop)[cite: 1]
    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport((width - height) / 2, 0, height, height); // Vierkante viewport (Opdracht 1.9)[cite: 1]

        // Wis zowel de kleurenbuffer als de dieptebuffer (Opdracht 3.4)[cite: 1]
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update de rotatiehoek voor animatie (Opdracht 1.12)[cite: 1]
        GLfloat angle = ROTATION_SPEED * glfwGetTime();
        glUniform1f(rotationAngleLocation, angle);

        // Teken de driehoeken van de kubus (Opdracht 2.3)[cite: 1]
        glDrawElements(GL_TRIANGLES, sizeof(edges) / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents(); // Niet-blokkerende event-afhandeling (Opdracht 1.12)[cite: 1]
    }

    glfwTerminate();
    return 0;
}