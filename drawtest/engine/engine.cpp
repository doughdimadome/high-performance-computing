// Engine implementeren
#include "engine.hpp"
#include "materials.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
// GLM voor matrixtransformaties
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// STB Image voor textuur laden
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Callback functie voor muis scrollen (zoom)
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    if (engine) {
        engine->camera.zoom(static_cast<float>(yoffset));
    }
}

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

bool Engine::loadSceneFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Kon scene bestand niet openen: " << filepath << std::endl;
        return false;
    }

    scene.clear();

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string typeStr, materialName, texPath;
        float px, py, pz, sx, sy, sz, opacity;

        if (ss >> typeStr >> px >> py >> pz >> sx >> sy >> sz >> opacity >> materialName >> texPath) {
            ShapeType type;
            if (typeStr == "CUBE") type = ShapeType::CUBE;
            else if (typeStr == "PYRAMID") type = ShapeType::PYRAMID;
            else if (typeStr == "CYLINDER") type = ShapeType::CYLINDER;
            else if (typeStr == "SPHERE") type = ShapeType::SPHERE;
            else continue;

            auto shape = ShapeFactory::createShape(type);
            shape->position = {px, py, pz};
            shape->scale = {sx, sy, sz};

            if (materialName != "NONE") {
                shape->material = MaterialLibrary::getMaterial(materialName);
            } else {
                shape->material.ambient = {0.2f, 0.2f, 0.2f};
                shape->material.diffuse = {1.0f, 1.0f, 1.0f};
                shape->material.specular = {0.0f, 0.0f, 0.0f};
                shape->material.shininess = 1.0f;
            }

            shape->material.opacity = opacity;

            if (texPath != "NONE") {
                shape->textureID = loadTexture(texPath.c_str());
            }

            scene.push_back(std::move(shape));
        }
    }

    std::cout << "Succesvol " << scene.size() << " objecten geladen uit " << filepath << std::endl;
    return true;
}

bool Engine::init_window(int width, int height, const char* title, bool fullscreen) {
    if (!glfwInit()) return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;
    if (fullscreen) {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        width = mode->width;
        height = mode->height;
    }

    window = glfwCreateWindow(width, height, title, monitor, nullptr);
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

    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window, scroll_callback);

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
            Normal = mat3(transpose(inverse(model))) * aNormal;
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
    float rotationSpeed = 1.0f;
    bool enableRotation = true;
    bool rKeyPressed = false;
    int colorCycle = 0;
    bool cKeyPressed = false;

    auto lightMarker = ShapeFactory::createShape(ShapeType::SPHERE);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // TOETSENBORD INVOER
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.moveForward(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.moveBackward(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.moveLeft(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.moveRight(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.moveUp(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.moveDown(deltaTime);

        // Rotatie pauzeren (R) 
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !rKeyPressed) {
            rKeyPressed = true;
            enableRotation = !enableRotation;
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) rKeyPressed = false;

        // Licht Positie (I/K/J/L/U/O) 
        float lightSpeed = 6.0f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) light.position.z -= lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) light.position.z += lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) light.position.x -= lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) light.position.x += lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) light.position.y += lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) light.position.y -= lightSpeed;

        // Licht Kleur Wisselen (C) 
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !cKeyPressed) {
            cKeyPressed = true;
            colorCycle = (colorCycle + 1) % 7;
            if (colorCycle == 0)      light.color = {1.0f, 1.0f, 1.0f};  // Neutraal Wit
            else if (colorCycle == 1) light.color = {1.0f, 0.85f, 0.6f}; // Warm Zonlicht
            else if (colorCycle == 2) light.color = {1.0f, 0.2f, 0.2f};  // Rood
            else if (colorCycle == 3) light.color = {0.2f, 1.0f, 0.2f};  // Groen
            else if (colorCycle == 4) light.color = {0.2f, 0.5f, 1.0f};  // Blauw
            else if (colorCycle == 5) light.color = {0.2f, 1.0f, 1.0f};  // Cyan
            else if (colorCycle == 6) light.color = {1.0f, 0.2f, 1.0f};  // Magenta
        }
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) cKeyPressed = false;

        // MUIS INVOER
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        if (firstMouse) { lastX = mouseX; lastY = mouseY; firstMouse = false; }

        float xoffset = static_cast<float>(mouseX - lastX);
        float yoffset = static_cast<float>(mouseY - lastY);
        lastX = mouseX; lastY = mouseY;

        float sensitivity = 0.0025f;
        camera.yaw += xoffset * sensitivity;
        camera.pitch += yoffset * sensitivity;

        if (camera.pitch > 1.5f) camera.pitch = 1.5f;
        if (camera.pitch < -1.5f) camera.pitch = -1.5f;

        // RENDEREN
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (height == 0) height = 1;

        glViewport(0, 0, width, height);
        float aspect = static_cast<float>(width) / static_cast<float>(height);

        glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        Matrix4 view = camera.getViewMatrix();
        GLint loc_view = glGetUniformLocation(shaderProgram, "view");
        if (loc_view != -1) glUniformMatrix4fv(loc_view, 1, GL_FALSE, &view.m[0][0]);

        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), aspect, 0.1f, 100.0f);
        GLint loc_proj = glGetUniformLocation(shaderProgram, "projection");
        if (loc_proj != -1) glUniformMatrix4fv(loc_proj, 1, GL_FALSE, glm::value_ptr(projection));

        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), light.position.x, light.position.y, light.position.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), light.color.x, light.color.y, light.color.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), camera.position.x, camera.position.y, camera.position.z);

        // VORMEN RENDEREN
        for (size_t i = 0; i < scene.size(); ++i) {
            auto& shape = scene[i];

            if (enableRotation) {
                shape->rotation.y += rotationSpeed * (i + 1) * deltaTime;
                // shape->rotation.x += rotationSpeed * 0.5f * deltaTime;
            }

            glm::mat4 model = shape->getModelMatrix();
            GLint loc_model = glGetUniformLocation(shaderProgram, "model");
            if (loc_model != -1) glUniformMatrix4fv(loc_model, 1, GL_FALSE, glm::value_ptr(model));

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

        // LICHTMARKER RENDEREN 
        lightMarker->position = light.position;
        lightMarker->scale = {0.15f, 0.15f, 0.15f};
        glm::mat4 lightModel = lightMarker->getModelMatrix();
        
        GLint loc_model = glGetUniformLocation(shaderProgram, "model");
        if (loc_model != -1) glUniformMatrix4fv(loc_model, 1, GL_FALSE, glm::value_ptr(lightModel));

        glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), light.color.x, light.color.y, light.color.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), light.color.x, light.color.y, light.color.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 128.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.opacity"), 1.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, defaultTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);

        lightMarker->draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

Engine::~Engine() {
    if (defaultTexture) glDeleteTextures(1, &defaultTexture);
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}