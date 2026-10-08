// install library on MacOS:
// install Homebrew: https://github.com/Homebrew/brew/releases
// open Terminal
// xcode-select --install
// brew install glfw

// compile on MacOS:
// c++ -o drawLines -I /opt/homebrew/include -L /opt/homebrew/lib -lglfw -framework OpenGL -DGL_SILENCE_DEPRECATION draw.cpp drawLines.cpp

// install libraries on Windows:
// install MSYS2: https://www.msys2.org
// start MSYS2 terminal
// install compiler: pacman -S mingw-w64-ucrt-x86_64-gcc
// install GLFW: pacman -S mingw-w64-ucrt-x86_64-glfw

// compile on Windows:
// g++ -o drawLines.exe draw.cpp drawLines.cpp -lglfw3 -lopengl32

#include "draw.h"

void myDrawing(Draw &d) {
    // Replace with your own drawing
    d.point(50, 10);

    d.color(1, 0, 0); // red
    d.line(10, 10, 790, 590);

    d.color(1, 1, 0); // yellow
    double time = d.getTime(); // time in seconds
    d.line(790 - 10 * time, 10, 10 + 10 * time, 590);
}

int main() {
    Draw d(800, 600, "Test Lines", &myDrawing);
    return 0;
}