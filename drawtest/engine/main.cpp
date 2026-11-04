#include "engine.hpp"
#include <iostream>

int main() {
    Engine engine;

    // 'true' zorgt voor fullscreen op de maximale schermresolutie
    if (!engine.init_window(1024, 768, "High Performance Computing Engine", true)) {
        std::cerr << "Fout bij initialiseren van venster!" << std::endl;
        return -1;
    }

    if (!engine.init_shaders()) {
        std::cerr << "Fout bij initialiseren van shaders!" << std::endl;
        return -1;
}

    engine.camera.position = {0.0f, 1.0f, 5.2f};

    // 1. AFBEELDINGEN INLADEN PER VORM
    // Zorg dat deze afbeeldingsbestanden in je projectmap (drawtest) staan.
    GLuint cubeTex     = engine.loadTexture("../images/Chuck_Norris_May_2015.jpg");
    GLuint pyramidTex  = engine.loadTexture("../images/jeff.jpg");
    GLuint cylinderTex = engine.loadTexture("../images/nico.jpg");
    GLuint sphereTex   = engine.loadTexture("../images/P_Diddy_2000.jpg");

    // 2. KUBUS
    auto cube = ShapeFactory::createShape(ShapeType::CUBE);
    cube->position = {-2.4f, 0.0f, 0.0f};
    cube->material.diffuse = {1.0f, 1.0f, 1.0f}; // Neutraal wit voor originele fotokleuren
    cube->material.ambient = {0.2f, 0.2f, 0.2f}; // Iets meer omgevingslicht voor de kubus
    cube->textureID = cubeTex;
    engine.scene.push_back(std::move(cube));

    // 3. PIRAMIDE
    auto pyramid = ShapeFactory::createShape(ShapeType::PYRAMID);
    pyramid->position = {-0.8f, 0.0f, 0.0f};
    pyramid->material.diffuse = {1.0f, 1.0f, 1.0f};
    pyramid->material.ambient = {0.2f, 0.2f, 0.2f};
    pyramid->textureID = pyramidTex;
    engine.scene.push_back(std::move(pyramid));

    // 4. CILINDER (Semi-transparant met textuur)
    auto cylinder = ShapeFactory::createShape(ShapeType::CYLINDER);
    cylinder->position = {0.8f, 0.0f, 0.0f};
    cylinder->material.diffuse = {1.0f, 1.0f, 1.0f};
    cylinder->material.ambient = {0.2f, 0.2f, 0.2f};
    cylinder->material.opacity = 0.85f; // Optioneel semi-transparant
    cylinder->textureID = cylinderTex;
    engine.scene.push_back(std::move(cylinder));

    // 5. BOL
    auto sphere = ShapeFactory::createShape(ShapeType::SPHERE);
    sphere->position = {2.4f, 0.0f, 0.0f};
    sphere->material.diffuse = {1.0f, 1.0f, 1.0f};
    sphere->material.ambient = {0.2f, 0.2f, 0.2f};
    sphere->textureID = sphereTex;
    engine.scene.push_back(std::move(sphere));

    std::cout << "Engine gestart met textuur op alle " << engine.scene.size() << " vormen!" << std::endl;
    engine.run();

    return 0;
}