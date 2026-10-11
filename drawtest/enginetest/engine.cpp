#include "Engine.hpp"
#include <iostream>
#include <chrono>

void Engine::init() {
    cube.mesh = Mesh::create_cube();
    cube.position = Vec{0.0f, 0.0f, -3.5f, 1.0f};
    cube.rotation = Vec{0.0f, 0.0f, 0.0f, 0.0f};
}

void Engine::update(float dt) {
    current_time += dt;

    // Roteer de kubus (graden per seconde)
    cube.rotation.y += 60.0f * dt;
    cube.rotation.x += 30.0f * dt;
}

void Engine::render() {
    // 1. Bouw de transformatiestap met de subclasses uit library.hpp
    Translate T_world(cube.position);
    Rotate R_y(Axis::Y, cube.rotation.y);
    Rotate R_x(Axis::X, cube.rotation.x);

    Transform model = T_world * R_y * R_x;

    Perspective proj(60.0f);
    Scale S_screen(200.0f, 200.0f, 1.0f);
    Translate T_screen(400.0f, 300.0f, 0.0f);

    Transform mvp = T_screen * S_screen * proj * model;

    // 2. Transformeer alle vertices
    std::vector<Vec> transformed_vertices;
    for (const auto& v : cube.mesh.vertices) {
        Vec projected = mvp * v;
        transformed_vertices.push_back(projected);
    }

    // 3. Teken de ribben
    for (const auto& edge : cube.mesh.edges) {
        const Vec& p1 = transformed_vertices[edge.v1];
        const Vec& p2 = transformed_vertices[edge.v2];

        d.line(p1.x, p1.y, p2.x, p2.y);
    }
}

void Engine::run() {
    init();

    auto last_time = std::chrono::high_resolution_clock::now();

    while (d.isOpen()) {
        auto current_frame_time = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(current_frame_time - last_time).count();
        last_time = current_frame_time;

        d.clear();
        update(dt);
        render();
        d.display();
    }
}