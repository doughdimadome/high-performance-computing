#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include "library.hpp"
#include "camera_light.hpp"
#include "shapes.hpp"

class Engine {
private:
    GLFWwindow* window = nullptr;
    GLuint shaderProgram = 0;
    GLuint defaultTexture = 0; // 1x1 witte textuur als fallback

public:
    Camera camera;
    Light light;
    std::vector<std::unique_ptr<Shape>> scene;

    Engine() = default;
    ~Engine();

    bool init_window(int width = 1024, int height = 768, const char* title = "HPC Graphics Engine", bool fullscreen = false);
    bool init_shaders();
    void run();

    // Textuur loader declaratie (alleen declaratie, definitie staat in engine.cpp!)
    GLuint loadTexture(const char* filepath);
};

#endif // ENGINE_HPP