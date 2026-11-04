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

        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoord;

        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(model) * aNormal;
            TexCoord = aTexCoord;
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoord;

        struct Material {
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            float shininess;
        };

        uniform Material material;
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 viewPos;

        void main() {
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            vec3 viewDir = normalize(viewPos - FragPos);

            // Ambient
            vec3 ambient = lightColor * material.ambient;

            // Diffuse
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = lightColor * (diff * material.diffuse);

            // Specular (Blinn-Phong)
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
            vec3 specular = lightColor * (spec * material.specular);

            vec3 finalColor = ambient + diffuse + specular;
            FragColor = vec4(finalColor, 1.0);
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
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (height == 0) height = 1;

        glViewport(0, 0, width, height);
        float aspect = static_cast<float>(width) / static_cast<float>(height);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        angle += 0.01f;

        RotateY model_rot(angle);
        Translate model_pos(position.x, position.y, position.z);
        Matrix4 model = model_pos * model_rot;

        // --- CAMERA LAGER EN OOGHOOGTE AANPASSEN ---
        float camX = 0.0f;
        float camY = 0.8f;  // Was 2.5f -> flink verlaagd
        float camZ = 3.5f;  // Iets dichterbij gehaald

        // Kleiner maken van de kantelhoek (~8.5 graden i.p.v. ~23 graden)
        RotateX view_pitch(0.15f); 
        Translate view_trans(-camX, -camY, -camZ);
        Matrix4 view = view_pitch * view_trans;

        Perspective projection(45.0f, aspect, 0.1f, 100.0f);

        glUseProgram(shaderProgram);

        GLint loc_model = glGetUniformLocation(shaderProgram, "model");
        GLint loc_view = glGetUniformLocation(shaderProgram, "view");
        GLint loc_proj = glGetUniformLocation(shaderProgram, "projection");

        if (loc_model != -1) glUniformMatrix4fv(loc_model, 1, GL_FALSE, &model.m[0][0]);
        if (loc_view != -1)  glUniformMatrix4fv(loc_view, 1, GL_FALSE, &view.m[0][0]);
        if (loc_proj != -1)  glUniformMatrix4fv(loc_proj, 1, GL_FALSE, &projection.m[0][0]);

        // Materiaal instellingen
        glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.0215f, 0.1745f, 0.0215f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.07568f, 0.61424f, 0.07568f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.633f, 0.727811f, 0.633f);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 6.0f);

        
        // Zet het licht op (1.2f, 1.8f, 2.0f) zodat de reflectie op de voorste hoek valt
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 1.2f, 1.8f, 2.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), camX, camY, camZ);

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