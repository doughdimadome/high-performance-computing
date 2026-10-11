#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <vector>
#include "library.hpp"
#include "draw.hpp"

struct Edge {
    int v1;
    int v2;
};

struct Mesh {
    std::vector<Vec> vertices;
    std::vector<Edge> edges;

    static Mesh create_cube() {
        Mesh cube;
        cube.vertices = {
            { 1.0f,  1.0f,  1.0f, 1.0f},
            { 1.0f, -1.0f,  1.0f, 1.0f},
            {-1.0f, -1.0f,  1.0f, 1.0f},
            {-1.0f,  1.0f,  1.0f, 1.0f},
            { 1.0f,  1.0f, -1.0f, 1.0f},
            { 1.0f, -1.0f, -1.0f, 1.0f},
            {-1.0f, -1.0f, -1.0f, 1.0f},
            {-1.0f,  1.0f, -1.0f, 1.0f}
        };
        cube.edges = {
            {0, 1}, {1, 2}, {2, 3}, {3, 0},
            {4, 5}, {5, 6}, {6, 7}, {7, 4},
            {0, 4}, {1, 5}, {2, 6}, {3, 7}
        };
        return cube;
    }
};

// Bewaar positie en rotatie direct in het GameObject
struct GameObject {
    Mesh mesh;
    Vec position{0.0f, 0.0f, 0.0f, 1.0f};
    Vec rotation{0.0f, 0.0f, 0.0f, 0.0f}; // x, y, z hoeken in graden
};

class Engine {
private:
    Draw d;
    float current_time{0.0f};

public:
    GameObject cube;

    void init();
    void update(float dt);
    void render();
    void run();
};

#endif // ENGINE_HPP