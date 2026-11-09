#include "engine.hpp"
#include "materials.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

// GLM bibliotheek voor model- en perspectiefmatrices
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Single-header library voor het inladen van afbeeldingen
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Callback-functie voor het opvangen van muiswiel-scrolls (past de camera FOV aan voor zoom)
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    if (engine) {
        engine->camera.zoom(static_cast<float>(yoffset));
    }
}


// TEXTUUR LADEN VIA STB_IMAGE

GLuint Engine::loadTexture(const char* filepath) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Textuureigenschappen instellen (herhalen en lineair filteren)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    // OpenGL verwacht (0,0) linksonder, stb_image laadt van linksboven -> dus verticaal flippen
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "[FOUT] Kon textuur niet laden: " << filepath << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}


// SCENE PARSER (SCENE.TXT)

bool Engine::loadSceneFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[FOUT] Kon scene.txt niet openen op pad: " << filepath << std::endl;
        return false;
    }

    scene.clear(); // Oude scene leeghalen indien van toepassing

    std::string line;
    while (std::getline(file, line)) {
        // Lege regels of commentaarregels (#) overslaan
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string typeStr, materialName, texPath;
        float px, py, pz, sx, sy, sz, opacity;

        // Lees alle ruimtelijke en materiële eigenschappen regel voor regel uit
        if (ss >> typeStr >> px >> py >> pz >> sx >> sy >> sz >> opacity >> materialName >> texPath) {
            ShapeType type;
            if (typeStr == "CUBE") type = ShapeType::CUBE;
            else if (typeStr == "PYRAMID") type = ShapeType::PYRAMID;
            else if (typeStr == "CYLINDER") type = ShapeType::CYLINDER;
            else if (typeStr == "SPHERE") type = ShapeType::SPHERE;
            else if (typeStr == "CONE") type = ShapeType::CONE;
            else continue;

            // Gebruik het Factory Pattern voor het aanmaken van de juiste Shape subclass
            auto shape = ShapeFactory::createShape(type);
            shape->position = {px, py, pz};
            shape->scale = {sx, sy, sz};

            // Kies tussen een SGI materiaalpreset of een neutraal basis-materiaal voor JPG's
            if (materialName != "NONE") {
                shape->material = MaterialLibrary::getMaterial(materialName);
            } else {
                shape->material.ambient = {0.2f, 0.2f, 0.2f};
                shape->material.diffuse = {1.0f, 1.0f, 1.0f};
                shape->material.specular = {0.0f, 0.0f, 0.0f};
                shape->material.shininess = 1.0f;
            }

            // Explicite opaciteit instellen (voor transparantie support)
            shape->material.opacity = opacity;

            // Optionele textuur koppelen
            if (texPath != "NONE") {
                shape->textureID = loadTexture(texPath.c_str());
            }

            scene.push_back(std::move(shape));
        }
    }

    return true;
}


// INITIALISATIE VENSTER EN OPENGL CONTEXT

bool Engine::init_window(int width, int height, const char* title, bool fullscreen) {
    if (!glfwInit()) return false;

    // OpenGL 3.3 Core Profile afdwingen
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

    // Z-buffer (Depth test) inschakelen voor correcte 3D overlapping
    glEnable(GL_DEPTH_TEST);

    // Alpha blending inschakelen voor (semi-)transparante objecten
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Muis verbergen en vasthouden binnen het venster (voor FPS-style camera)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Koppel de Engine instantie aan het GLFW venster voor callbacks
    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window, scroll_callback);

    // 1x1 witte default textuur genereren (voorkomt lege texture-bindings)
    glGenTextures(1, &defaultTexture);
    glBindTexture(GL_TEXTURE_2D, defaultTexture);
    unsigned char whitePixel[] = { 255, 255, 255, 255 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

    return true;
}


// SHADER PIJPLIJN (GLSL VERTEX & FRAGMENT SHADERS)

bool Engine::init_shaders() {
    // Vertex Shader: Transformeert 3D-hoekpunten (MVP) en berekent de Normal Matrix
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
            
            // Transpose-inverse van de modelmatrix voorkomt vervorming van normaalvectoren bij niet-uniforme schaling
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoord = aTexCoord;
            
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

    // Fragment Shader: Blinn-Phong belichtingsmodel met Ambient, Diffuse, Specular & Opacity
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

    struct LightSource {
        vec3 position;
        vec3 color;
    };

    uniform Material material;
    uniform LightSource lights[2]; // Array voor 2 lichten
    uniform vec3 viewPos;
    uniform sampler2D uTexture;

    void main() {
        vec3 norm = normalize(Normal);
        vec3 viewDir = normalize(viewPos - FragPos); // Richting van het fragment naar de camera
        vec4 texColor = texture(uTexture, TexCoord); // Haal de textuurkleur op voor dit fragment

        vec3 totalLight = vec3(0.0);

        for (int i = 0; i < 2; i++) {
            vec3 lightDir = normalize(lights[i].position - FragPos); // Richting van het fragment naar de lichtbron

            vec3 ambient = lights[i].color * material.ambient; // Ambient component

            float diff = max(dot(norm, lightDir), 0.0); 
            vec3 diffuse = lights[i].color * (diff * material.diffuse); // diffuse component (Lambertian reflectie)

            vec3 halfwayDir = normalize(lightDir + viewDir); // Halfway vector voor Blinn-Phong specular reflectie
            float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
            vec3 specular = lights[i].color * (spec * material.specular); // specular component 

            totalLight += (ambient + diffuse + specular); // Accumuleer de belichting van alle lichten
        }

        vec3 finalColor = totalLight * texColor.rgb; // Combineer belichting met textuurkleur
        FragColor = vec4(finalColor, material.opacity * texColor.a); // Pas de alpha-waarde aan op basis van materiaaleigenschappen en textuur
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


// MAIN RENDER LOOP

void Engine::run() {
    float lastFrame = static_cast<float>(glfwGetTime());
    bool firstMouse = true;
    double lastX = 512.0, lastY = 384.0;
    float rotationSpeed = 1.0f;
    bool enableRotation = true;
    bool rKeyPressed = false;
    int colorCycle = 0;
    bool cKeyPressed = false;

    // Maak twee visuele bollen die de posities van de lichtbronnen aangeven in de 3D-wereld
    Light light1;
    light1.position = {1.2f, 2.5f, 2.0f};
    light1.color = {1.0f, 1.0f, 1.0f};

    Light light2;
    light2.position = {-3.0f, 2.0f, -2.0f};
    light2.color = {0.6f, 0.2f, 0.8f};

    auto lightMarker1 = ShapeFactory::createShape(ShapeType::SPHERE);
    auto lightMarker2 = ShapeFactory::createShape(ShapeType::SPHERE);

    while (!glfwWindowShouldClose(window)) {
        // Tijdverschil berekenen voor framerate-onafhankelijke beweging (deltaTime)
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        
        // TOETSENBORD INVOER (Camera & Licht)
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Camera verplaatsing (WASD + Spatie/Shift)
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.moveForward(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.moveBackward(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.moveLeft(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.moveRight(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.moveUp(deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.moveDown(deltaTime);

        // Rotatie pauzeren/hervatten via toets 'R'
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !rKeyPressed) {
            rKeyPressed = true;
            enableRotation = !enableRotation;
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) rKeyPressed = false;

        // Lichtbron 1 verplaatsen via toetsen I/K/J/L/U/O
        // Houd bij welk licht geselecteerd is (0 = Licht 1, 1 = Licht 2)
        static int selectedLight = 0;

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) selectedLight = 0; // Selecteer Licht 1
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) selectedLight = 1; // Selecteer Licht 2

        // Pak een referentie naar het actieve licht
        Light& currentLight = (selectedLight == 0) ? light1 : light2;

        // Verplaats het geselecteerde licht via I/K/J/L/U/O
        float lightSpeed = 6.0f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) currentLight.position.z -= lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) currentLight.position.z += lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) currentLight.position.x -= lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) currentLight.position.x += lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) currentLight.position.y += lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) currentLight.position.y -= lightSpeed;

        // Wissel de kleur van het geselecteerde licht via 'C'
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !cKeyPressed) {
            cKeyPressed = true;
            colorCycle = (colorCycle + 1) % 7;
            if (colorCycle == 0)      currentLight.color = {1.0f, 1.0f, 1.0f};  // Wit
            else if (colorCycle == 1) currentLight.color = {1.0f, 0.85f, 0.6f}; // Warm Zonlicht
            else if (colorCycle == 2) currentLight.color = {1.0f, 0.2f, 0.2f};  // Rood
            else if (colorCycle == 3) currentLight.color = {0.2f, 1.0f, 0.2f};  // Groen
            else if (colorCycle == 4) currentLight.color = {0.2f, 0.5f, 1.0f};  // Blauw
            else if (colorCycle == 5) currentLight.color = {0.2f, 1.0f, 1.0f};  // Cyan
            else if (colorCycle == 6) currentLight.color = {1.0f, 0.2f, 1.0f};  // Magenta
        }
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) cKeyPressed = false;
        
        // MUIS INVOER (Camera Kijkrichting)
        
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        if (firstMouse) { lastX = mouseX; lastY = mouseY; firstMouse = false; }

        float xoffset = static_cast<float>(mouseX - lastX);
        float yoffset = static_cast<float>(mouseY - lastY);
        lastX = mouseX; lastY = mouseY;

        float sensitivity = 0.0025f;
        camera.yaw += xoffset * sensitivity;
        camera.pitch += yoffset * sensitivity;

        // Pitch begrenzen om te voorkomen dat het scherm inklapt (Gimbal Lock restrictie)
        if (camera.pitch > 1.5f) camera.pitch = 1.5f;
        if (camera.pitch < -1.5f) camera.pitch = -1.5f;

        
        // RENDER PASS
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (height == 0) height = 1;

        glViewport(0, 0, width, height);
        float aspect = static_cast<float>(width) / static_cast<float>(height);

        glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // View Matrix ophalen uit onze eigen wiskunde-library (library.hpp)
        Matrix4 view = camera.getViewMatrix();
        GLint loc_view = glGetUniformLocation(shaderProgram, "view");
        if (loc_view != -1) glUniformMatrix4fv(loc_view, 1, GL_FALSE, &view.m[0][0]); 

        // Perspective Projection Matrix opbouwen via GLM met dynamic FOV (voor zoom)
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), aspect, 0.1f, 100.0f);
        GLint loc_proj = glGetUniformLocation(shaderProgram, "projection");
        if (loc_proj != -1) glUniformMatrix4fv(loc_proj, 1, GL_FALSE, glm::value_ptr(projection));

        // Beide belichtings-uniforms doorsturen naar de Fragment Shader Array
        glUniform3f(glGetUniformLocation(shaderProgram, "lights[0].position"), light1.position.x, light1.position.y, light1.position.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "lights[0].color"), light1.color.x, light1.color.y, light1.color.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "lights[1].position"), light2.position.x, light2.position.y, light2.position.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "lights[1].color"), light2.color.x, light2.color.y, light2.color.z);
        
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), camera.position.x, camera.position.y, camera.position.z);

        // Render alle objecten die vanuit scene.txt zijn in geladen
        for (size_t i = 0; i < scene.size(); ++i) {
            auto& shape = scene[i];

            if (enableRotation) {
                shape->rotation.y += rotationSpeed * (i + 1) * deltaTime;
                shape->rotation.x += rotationSpeed * 0.5f * deltaTime;
            }

            // Model Matrix berekenen via GLM
            glm::mat4 model = shape->getModelMatrix();
            GLint loc_model = glGetUniformLocation(shaderProgram, "model");
            if (loc_model != -1) glUniformMatrix4fv(loc_model, 1, GL_FALSE, glm::value_ptr(model));

            // Materiaaleigenschappen doorgeven
            glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), shape->material.ambient.x, shape->material.ambient.y, shape->material.ambient.z);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), shape->material.diffuse.x, shape->material.diffuse.y, shape->material.diffuse.z);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), shape->material.specular.x, shape->material.specular.y, shape->material.specular.z);
            glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), shape->material.shininess);
            glUniform1f(glGetUniformLocation(shaderProgram, "material.opacity"), shape->material.opacity);

            // Textuur binden
            glActiveTexture(GL_TEXTURE0);
            GLuint texToBind = (shape->textureID != 0) ? shape->textureID : defaultTexture;
            glBindTexture(GL_TEXTURE_2D, texToBind);
            glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);

            shape->draw();
        }

        // Render de fysieke marker van Lichtbron 1
        lightMarker1->position = light1.position;
        lightMarker1->scale = {0.15f, 0.15f, 0.15f};
        glm::mat4 lightModel1 = lightMarker1->getModelMatrix();
        GLint loc_light_model1 = glGetUniformLocation(shaderProgram, "model");
        if (loc_light_model1 != -1) glUniformMatrix4fv(loc_light_model1, 1, GL_FALSE, glm::value_ptr(lightModel1));

        glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), light1.color.x, light1.color.y, light1.color.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), light1.color.x, light1.color.y, light1.color.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 128.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.opacity"), 1.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, defaultTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);

        lightMarker1->draw();

        // Render de fysieke marker van Lichtbron 2
        lightMarker2->position = light2.position;
        lightMarker2->scale = {0.15f, 0.15f, 0.15f};
        glm::mat4 lightModel2 = lightMarker2->getModelMatrix();
        GLint loc_light_model2 = glGetUniformLocation(shaderProgram, "model");
        if (loc_light_model2 != -1) glUniformMatrix4fv(loc_light_model2, 1, GL_FALSE, glm::value_ptr(lightModel2));

        glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), light2.color.x, light2.color.y, light2.color.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), light2.color.x, light2.color.y, light2.color.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 128.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.opacity"), 1.0f);

        lightMarker2->draw();

        // Buffers wisselen (Double buffering) en input events verwerken
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

Engine::~Engine() {
    if (defaultTexture) glDeleteTextures(1, &defaultTexture);
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}