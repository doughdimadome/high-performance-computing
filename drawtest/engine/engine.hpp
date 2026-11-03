#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include "library.hpp"
#include "camera_light.hpp"
#include "shapes.hpp"

// GRAPHICS ENGINE CLASS
// Handleiding-eisen: Beheert het venster, de shaders en de render-loop

class Engine {
private:
    GLFWwindow* window = nullptr;
    GLuint shaderProgram = 0;

public:
    Camera camera;
    Light light;
    std::vector<std::unique_ptr<Shape>> scene;

    Engine() = default;
    ~Engine();

    bool init_window(int width = 1024, int height = 768, const char* title = "HPC Graphics Engine");
    bool init_shaders();
    void run();
};

#endif // ENGINE_HPP