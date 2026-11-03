#include "engine.hpp"
#include <iostream>

// Handleiding-eisen: Demoprogramma dat de functies van de library aantoont
int main() {
    Engine engine;

    if (!engine.init_window(1024, 768, "High Performance Computing Engine")) {
        std::cerr << "Fout bij initialiseren van venster!" << std::endl;
        return -1;
    }

    if (!engine.init_shaders()) {
        std::cerr << "Fout bij initialiseren van shaders!" << std::endl;
        return -1;
    }

    // Instellen van de camerapositie
    engine.camera.position = {0.0f, 1.0f, 5.2f};

    // Aanmaken van de 4 verplichte vormen via het Factory Pattern
    
    // 1. Kubus
    auto cube = ShapeFactory::createShape(ShapeType::CUBE);
    cube->position = {-2.4f, 0.0f, 0.0f};
    cube->material.diffuse = {0.1f, 0.7f, 0.1f};
    engine.scene.push_back(std::move(cube));

    // 2. Piramide
    auto pyramid = ShapeFactory::createShape(ShapeType::PYRAMID);
    pyramid->position = {-0.8f, 0.0f, 0.0f};
    pyramid->material.diffuse = {0.8f, 0.7f, 0.1f};
    engine.scene.push_back(std::move(pyramid));

    // 3. Cilinder (met opaciteit / transparantie)
    auto cylinder = ShapeFactory::createShape(ShapeType::CYLINDER);
    cylinder->position = {0.8f, 0.0f, 0.0f};
    cylinder->material.diffuse = {0.8f, 0.1f, 0.1f};
    cylinder->material.opacity = 0.7f; // 70% opaciteit
    engine.scene.push_back(std::move(cylinder));

    // 4. Bol
    auto sphere = ShapeFactory::createShape(ShapeType::SPHERE);
    sphere->position = {2.4f, 0.0f, 0.0f};
    sphere->material.diffuse = {0.1f, 0.3f, 0.8f};
    engine.scene.push_back(std::move(sphere));

    std::cout << "Engine gestart met " << engine.scene.size() << " verplichte vormen." << std::endl;
    
    // Start de applicatie
    engine.run();

    return 0;
}