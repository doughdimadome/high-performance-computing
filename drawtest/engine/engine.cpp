#include "engine.hpp"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint Engine::loadTexture(const char* filepath) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Textuur geladen: " << filepath << std::endl;
    } else {
        std::cerr << "Mislukt om textuur te laden: " << filepath << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}

bool Engine::init_window(int width, int height, const char* title, bool fullscreen) {
    if (!glfwInit()) return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primaryMonitor = nullptr;

    // Als fullscreen true is, pak de primaire monitor en zijn maximale resolutie
    if (fullscreen) {
        primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

        width = mode->width;
        height = mode->height;
    }

    // Geef primaryMonitor mee als 4e argument voor fullscreen (nullptr voor venster)
    window = glfwCreateWindow(width, height, title, primaryMonitor, nullptr);
    if (!window) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return false;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Fallback textuur
    glGenTextures(1, &defaultTexture);
    glBindTexture(GL_TEXTURE_2D, defaultTexture);
    unsigned char whitePixel[] = { 255, 255, 255, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

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
            float opacity;
        };

        uniform Material material;
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 viewPos;

        uniform sampler2D uTexture;

        void main() {
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            vec3 viewDir = normalize(viewPos - FragPos);

            vec4 texColor = texture(uTexture, TexCoord);

            vec3 ambient = lightColor * material.ambient;

            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = lightColor * (diff * material.diffuse);

            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
            vec3 specular = lightColor * (spec * material.specular);

            vec3 finalColor = (ambient + diffuse + specular) * texColor.rgb;
            FragColor = vec4(finalColor, material.opacity * texColor.a);
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
    float lastFrame = static_cast<float>(glfwGetTime());
    bool firstMouse = true;
    double lastX = 512.0, lastY = 384.0;

    while (!glfwWindowShouldClose(window)) {
        // Delta Time berekening voor fps-onafhankelijke bewegingssnelheid
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        
        // TOETSENBORD INVOER (WASD + Spatie/Shift + Escape)
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.moveForward(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.moveBackward(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.moveLeft(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.moveRight(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera.moveUp(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera.moveDown(deltaTime);

        
        // MUIS INVOER (Pitch & Yaw)
        
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        if (firstMouse) {
            lastX = mouseX;
            lastY = mouseY;
            firstMouse = false;
        }

        float xoffset = static_cast<float>(mouseX - lastX);
        float yoffset = static_cast<float>(mouseY - lastY); // Omgekeerd omdat Y naar beneden telt op het scherm
        lastX = mouseX;
        lastY = mouseY;

        float sensitivity = 0.0025f; // Muisgevoeligheid
        camera.yaw += xoffset * sensitivity;
        camera.pitch += yoffset * sensitivity;

        // Pitch begrenzen tussen ~ -85 graden (-1.5 rad) en +85 graden (1.5 rad) tegen doordraaien
        if (camera.pitch > 1.5f) camera.pitch = 1.5f;
        if (camera.pitch < -1.5f) camera.pitch = -1.5f;

        // RENDEREN
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (height == 0) height = 1;

        glViewport(0, 0, width, height);
        float aspect = static_cast<float>(width) / static_cast<float>(height);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        float rotationSpeed = 1.0f; 

        // Binnen de render loop:
        for (size_t i = 0; i < scene.size(); ++i) {
            auto& shape = scene[i];

            // Draai de vormen elke frame rond
            shape->rotation.y += rotationSpeed * (i + 1) * deltaTime;
            shape->rotation.x += rotationSpeed * 0.5f * deltaTime;
            shape->rotation.z += rotationSpeed * 0.2f * deltaTime;

            Matrix4 model = shape->getModelMatrix();
            GLint loc_model = glGetUniformLocation(shaderProgram, "model");
            if (loc_model != -1) glUniformMatrix4fv(loc_model, 1, GL_FALSE, &model.m[0][0]);

        }

        Matrix4 view = camera.getViewMatrix();
        Perspective projection(45.0f, aspect, 0.1f, 100.0f);

        GLint loc_view = glGetUniformLocation(shaderProgram, "view");
        GLint loc_proj = glGetUniformLocation(shaderProgram, "projection");
        if (loc_view != -1) glUniformMatrix4fv(loc_view, 1, GL_FALSE, &view.m[0][0]);
        if (loc_proj != -1) glUniformMatrix4fv(loc_proj, 1, GL_FALSE, &projection.m[0][0]);

        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), light.position.x, light.position.y, light.position.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), light.color.x, light.color.y, light.color.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), camera.position.x, camera.position.y, camera.position.z);

        for (size_t i = 0; i < scene.size(); ++i) {
            auto& shape = scene[i];

            Matrix4 model = shape->getModelMatrix();
            GLint loc_model = glGetUniformLocation(shaderProgram, "model");
            if (loc_model != -1) glUniformMatrix4fv(loc_model, 1, GL_FALSE, &model.m[0][0]);

            glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), shape->material.ambient.x, shape->material.ambient.y, shape->material.ambient.z);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), shape->material.diffuse.x, shape->material.diffuse.y, shape->material.diffuse.z);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), shape->material.specular.x, shape->material.specular.y, shape->material.specular.z);
            glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), shape->material.shininess);
            glUniform1f(glGetUniformLocation(shaderProgram, "material.opacity"), shape->material.opacity);

            glActiveTexture(GL_TEXTURE0);
            GLuint texToBind = (shape->textureID != 0) ? shape->textureID : defaultTexture;
            glBindTexture(GL_TEXTURE_2D, texToBind);
            glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);

            shape->draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

Engine::~Engine() {
    if (defaultTexture) glDeleteTextures(1, &defaultTexture);
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}