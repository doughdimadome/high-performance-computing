#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <vector>
#include "library.hpp"
#include "draw.hpp"

// Structuur voor de ribben van een mesh
struct Edge {
    int v1;
    int v2;
};

// 1. TRANSFORM (Bepaalt de positie, rotatie en schaal in de 3D wereld)
struct Transform {
    Vec position{0.0f, 0.0f, 0.0f, 1.0f};
    Vec rotation{0.0f, 0.0f, 0.0f, 0.0f}; // x, y, z hoeken in radialen
    Vec scale{1.0f, 1.0f, 1.0f, 1.0f};

    // Berekent de gecombineerde Model Matrix: Translatie * RotatieX * RotatieY * Schaal
    Matrix get_model_matrix() const {
        Matrix S = create_scale(scale.x, scale.y, scale.z);
        Matrix Rx = create_rotation_x(rotation.x);
        Matrix Ry = create_rotation_y(rotation.y);
        Matrix T = create_translation(position.x, position.y, position.z);

        Matrix model = T;
        multiply(model, Ry);
        multiply(model, Rx);
        multiply(model, S);
        return model;
    }
};

// 2. MESH (Beheert de geometrie)
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

// 3. GAME OBJECT (Combineert geometrie met een transformatie)
struct GameObject {
    Mesh mesh;
    Transform transform;
};

// 4. ENGINE CORE (Beheert de gameloop en rendering)
class Engine {
private:
    Draw d; // Draw library instance
    bool is_running{false};
    float current_time{0.0f};
    
    // Scherm & Camera instellingen
    Matrix projection_matrix;
    Matrix screen_matrix;

public:
    GameObject cube;

    void init();
    void update(float dt);
    void render();
    void run();
};

#endif // ENGINE_HPP