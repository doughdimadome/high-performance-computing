#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <string>

#include "library.hpp"
#include "camera_light.hpp"
#include "shapes.hpp"

// Hoofdklasse die het GLFW-venster, de shader-pijplijn, invoer en renderloop beheert.
class Engine {
private:
    GLFWwindow* window = nullptr;
    GLuint shaderProgram = 0;
    GLuint defaultTexture = 0; // 1x1 witte fallback-textuur voor objecten zonder JPG

public:
    Camera camera;
    Light light;
    std::vector<std::unique_ptr<Shape>> scene; // Vector met dynamische 3D shapes (Factory pattern)

    Engine() = default;
    ~Engine();

    // Initialiseert het GLFW-venster, OpenGL-context (3.3 Core Profile) en GLEW
    bool init_window(int width = 1024, int height = 768, const char* title = "HPC Graphics Engine", bool fullscreen = false);

    // Compileert de Vertex en Fragment shaders en linkt het shaderprogramma
    bool init_shaders();

    // Start de hoofdrenderlus (invoer afhandelen, matrices updaten en scene tekenen)
    void run();

    // Laadt een JPG/PNG afbeelding via stb_image en maakt een OpenGL 2D-textuur aan
    GLuint loadTexture(const char* filepath);
    
    // Leest scene.txt uit en initialiseert alle 3D-objecten met hun transformaties en materialen
    bool loadSceneFromFile(const std::string& filepath);
};

#endif // ENGINE_HPP