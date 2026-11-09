#include "engine.hpp"
#include <iostream>

int main() {
    Engine engine;

    if (!engine.init_window(1024, 768, "High Performance Computing Engine - Extra Features", false)) {
        std::cerr << "Fout bij initialiseren van venster!" << std::endl;
        return -1;
    }

    if (!engine.init_shaders()) {
        std::cerr << "Fout bij initialiseren van shaders!" << std::endl;
        return -1;
    }

    // LAAD SCENE RECHTSTREEKS UIT `.txt` BESTAND
    if (!engine.loadSceneFromFile("scene.txt")) {
        std::cerr << "Fout bij inladen van scene.txt!" << std::endl;
        return -1;
    }

    engine.camera.position = {0.0f, 1.0f, 5.2f};

    std::cout << "Engine gestart!" << std::endl;
    std::cout << "BESTURING" << std::endl;
    std::cout << "WASD / Spatie / Shift : Camera bewegen" << std::endl;
    std::cout << "Muiswiel              : In-/Uitzoomen (FOV)" << std::endl;
    std::cout << "I / K / J / L / U / O  : Lichtbron verplaatsen" << std::endl;
    std::cout << "C                     : Lichtkleur wisselen" << std::endl;
    std::cout << "R                     : Rotatie pauzeren/hervatten" << std::endl;
    std::cout << "1 of 2                   : Wissel tussen Licht 1 en Licht 2" << std::endl;
    std::cout << "ESC                   : Afsluiten" << std::endl;

    engine.run();

    return 0;
}