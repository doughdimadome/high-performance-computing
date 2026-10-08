#include "Engine.hpp"
#include <iostream>
#include <chrono>
#include <thread>

void Engine::init() {
    is_running = true;

    // Cube klaarzetten
    cube = GameObject{ .mesh = Mesh::create_cube(), .transform = Transform{} };
    cube.transform.position = Vec{0.0f, 0.0f, -3.0f, 1.0f}; // Plaats de kubus 3 eenheden voor de camera
    cube.transform.scale = Vec{1.0f, 1.0f, 1.0f, 1.0f};

    // Set up Perspectief Projectie (zp = -1)
    projection_matrix = create_projection_perspective(-1.0f);

    // Set up Screen Matrix (Schaal naar bijv. 800x600 pixels en verplaats naar het midden)
    Matrix S_screen = create_scale(200.0f, 200.0f, 1.0f);
    Matrix T_screen = create_translation(400.0f, 300.0f, 0.0f);
    
    screen_matrix = T_screen;
    multiply(screen_matrix, S_screen);

    std::cout << "[Engine Initialized]\n";
}

void Engine::update(float dt) {
    current_time += dt;

    // Animeer de rotatie van de kubus op basis van verlopen tijd (DeltaTime)
    cube.transform.rotation.y += 1.0f * dt; // 1 radiaal per seconde rond Y-as
    cube.transform.rotation.x += 0.5f * dt; // 0.5 radiaal per seconde rond X-as
}

void Engine::render() {
    // 1. Bereken Model Matrix & MVP
    Matrix model = cube.transform.get_model_matrix();
    Matrix mvp = screen_matrix;
    multiply(mvp, projection_matrix);
    multiply(mvp, model);

    // 2. Transformeer vertices
    std::vector<Vec> transformed_vertices;
    for (const auto& v : cube.mesh.vertices) {
        Vec projected = multiply(mvp, v);
        perspective_divide(projected);
        transformed_vertices.push_back(projected);
    }

    // 3. Teken de ribben met de draw-library op het scherm
    for (const auto& edge : cube.mesh.edges) {
        const Vec& p1 = transformed_vertices[edge.v1];
        const Vec& p2 = transformed_vertices[edge.v2];

        // Gebruik de lijn-tekenfunctie van je draw-library
        d.line(p1.x, p1.y, p2.x, p2.y);
    }
}

void Engine::run() {
    init();

    auto last_time = std::chrono::high_resolution_clock::now();

    // Gameloop draait zolang het venster open is
    while (d.isOpen()) {
        auto current_frame_time = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(current_frame_time - last_time).count();
        last_time = current_frame_time;

        d.clear();  // Scherm leegmaken voor het nieuwe frame

        update(dt); // Posities/rotaties bijwerken
        render();   // Lijnen tekenen

        d.display(); // Frame weergeven in het venster
    }
}
