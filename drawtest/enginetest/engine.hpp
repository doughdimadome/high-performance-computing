#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "library.hpp"

// Single definition van Vertex struct
struct Vertex {
    float x, y, z;        // Positie
    float nx, ny, nz;     // Normaal
    float u, v;           // UV Texture coördinaten
};

struct Mesh {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    unsigned int index_count = 0;

    void create_cube_gpu();
};

class Engine {
private:
    GLFWwindow* window = nullptr;
    GLuint shaderProgram = 0;
    Mesh cube_mesh;

    Vec position{0.0f, 0.0f, 0.0f, 1.0f};
    Vec rotation{0.0f, 0.0f, 0.0f, 0.0f};

public:
    Engine() = default;
    ~Engine();

    bool init_window(int width = 800, int height = 600, const char* title = "3D Engine");
    bool init_shaders();
    void run();
};

#endif // ENGINE_HPP