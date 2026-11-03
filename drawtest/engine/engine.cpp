#include "engine.hpp"
#include <iostream>

bool Engine::init_window(int width, int height, const char* title) {
    if (!glfwInit()) return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return false;

    // Depth Test (Z-buffering) voor correcte diepteweergave
    glEnable(GL_DEPTH_TEST);

    // Alpha Blending inschakelen voor transparantie/opaciteit
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

bool Engine::init_shaders() {
    // VERTEX SHADER: Transformeert vertices met MVP matrix
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

    // FRAGMENT SHADER: Berekent Blinn-Phong belichting + opaciteit
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
            float opacity;
        };

        uniform Material material;
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 viewPos;

        void main() {
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            vec3 viewDir = normalize(viewPos - FragPos);

            // 1. Ambient
            vec3 ambient = lightColor * material.ambient;

            // 2. Diffuse (Lambertiaans)
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = lightColor * (diff * material.diffuse);

            // 3. Specular (Blinn-Phong met Halfway Vector)
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
            vec3 specular = lightColor * (spec * material.specular);

            vec3 finalColor = ambient + diffuse + specular;
            FragColor = vec4(finalColor, material.opacity);
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

    return true;
}

void Engine::run() {
    float angleDelta = 0.01f;

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (height == 0) height = 1;

        glViewport(0, 0, width, height);
        float aspect = static_cast<float>(width) / static_cast<float>(height);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        Matrix4 view = camera.getViewMatrix();
        Perspective projection(45.0f, aspect, 0.1f, 100.0f);

        // GL_FALSE meegegeven omdat onze C++ Transform Column-Major is
        GLint loc_view = glGetUniformLocation(shaderProgram, "view");
        GLint loc_proj = glGetUniformLocation(shaderProgram, "projection");
        if (loc_view != -1) glUniformMatrix4fv(loc_view, 1, GL_FALSE, &view.m[0][0]);
        if (loc_proj != -1) glUniformMatrix4fv(loc_proj, 1, GL_FALSE, &projection.m[0][0]);

        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), light.position.x, light.position.y, light.position.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), light.color.x, light.color.y, light.color.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), camera.position.x, camera.position.y, camera.position.z);

        // Render alle vormen in de scene
        for (size_t i = 0; i < scene.size(); ++i) {
            auto& shape = scene[i];

            // Roteer per frame op X, Y en Z as
            shape->rotation.y += angleDelta * (i + 1);
            shape->rotation.x += angleDelta * 0.5f;
            shape->rotation.z += angleDelta * 0.2f;

            Matrix4 model = shape->getModelMatrix();
            GLint loc_model = glGetUniformLocation(shaderProgram, "model");
            if (loc_model != -1) glUniformMatrix4fv(loc_model, 1, GL_FALSE, &model.m[0][0]);

            glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), shape->material.ambient.x, shape->material.ambient.y, shape->material.ambient.z);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), shape->material.diffuse.x, shape->material.diffuse.y, shape->material.diffuse.z);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), shape->material.specular.x, shape->material.specular.y, shape->material.specular.z);
            glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), shape->material.shininess);
            glUniform1f(glGetUniformLocation(shaderProgram, "material.opacity"), shape->material.opacity);

            shape->draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

Engine::~Engine() {
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}