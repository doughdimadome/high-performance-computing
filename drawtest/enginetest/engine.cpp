#include "engine.hpp"
#include <iostream>
#include <cstddef>

void Mesh::create_cube_gpu() {
    Vertex vertices[] = {
        // Front face (Z+)
        {-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f},
         {0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f},
         {0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f},
        {-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f},

        // Back face (Z-)
        {-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f},
        {-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f},
         {0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f},
         {0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f},

        // Top face (Y+)
        {-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f},
        {-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f},
         {0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f},
         {0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f},

        // Bottom face (Y-)
        {-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f},
         {0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f},
         {0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f},
        {-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f},

        // Right face (X+)
         {0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
         {0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f},
         {0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f},
         {0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f},

        // Left face (X-)
        {-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f},
        {-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
        {-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f},
        {-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f}
    };

    unsigned int indices[] = {
        0,  1,  2,  2,  3,  0,   // Front
        4,  5,  6,  6,  7,  4,   // Back
        8,  9,  10, 10, 11, 8,   // Top
        12, 13, 14, 14, 15, 12,  // Bottom
        16, 17, 18, 18, 19, 16,  // Right
        20, 21, 22, 22, 23, 20   // Left
    };

    index_count = sizeof(indices) / sizeof(unsigned int);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

bool Engine::init_window(int width, int height, const char* title) {
    if (!glfwInit()) {
        std::cerr << "Fout bij initialiseren van GLFW!" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Fout bij aanmaken van GLFW-venster!" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Fout bij initialiseren van GLEW!" << std::endl;
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    return true;
}

bool Engine::init_shaders() {
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoord;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec3 Normal;

        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            Normal = mat3(model) * aNormal;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 Normal;
        out vec4 FragColor;

        void main() {
            vec3 lightDir = normalize(vec3(0.5, 1.0, 1.0));
            float diff = max(dot(normalize(Normal), lightDir), 0.2);
            vec3 objectColor = vec3(0.2, 0.6, 1.0);
            FragColor = vec4(objectColor * diff, 1.0);
        }
    )";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    cube_mesh.create_cube_gpu();
    return true;
}

void Engine::run() {
    float angle = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        angle += 0.01f;

        RotateY model_rot(angle);
        Translate model_pos(position.x, position.y, position.z);
        Matrix4 model = model_pos * model_rot;

        Translate view(0.0f, 0.0f, -4.0f);
        Perspective projection(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

        glUseProgram(shaderProgram);

        GLint loc_model = glGetUniformLocation(shaderProgram, "model");
        GLint loc_view = glGetUniformLocation(shaderProgram, "view");
        GLint loc_proj = glGetUniformLocation(shaderProgram, "projection");

        if (loc_model != -1) glUniformMatrix4fv(loc_model, 1, GL_TRUE, &model.m[0][0]);
        if (loc_view != -1)  glUniformMatrix4fv(loc_view, 1, GL_TRUE, &view.m[0][0]);
        if (loc_proj != -1)  glUniformMatrix4fv(loc_proj, 1, GL_TRUE, &projection.m[0][0]);

        glBindVertexArray(cube_mesh.vao);
        glDrawElements(GL_TRIANGLES, cube_mesh.index_count, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

Engine::~Engine() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}