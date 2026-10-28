#include "engine.hpp"
#include <iostream>

int main() {
    Engine engine;

    if (!engine.init_window(800, 600, "3D Engine Test")) {
        std::cerr << "Fout bij initialiseren van het venster!" << std::endl;
        return -1;
    }

    if (!engine.init_shaders()) {
        std::cerr << "Fout bij initialiseren van de shaders!" << std::endl;
        return -1;
    }

    std::cout << "Engine gestart. Draaiende kubus wordt gerenderd..." << std::endl;
    engine.run();

    return 0;
}