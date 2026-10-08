// compile on MacOS:
// c++ -o drawFigure -I /opt/homebrew/include -L /opt/homebrew/lib -lglfw -framework OpenGL -DGL_SILENCE_DEPRECATION draw.cpp drawFigure.cpp

// compile on Windows:
// g++ -o drawFigure.exe draw.cpp drawFigure.cpp -lglfw3 -lopengl32

#include "draw.h"

struct {float x1, y1, x2, y2;} lines[] = {
    {10, 10, 100, 10},
    {100, 10, 100, 100},
    {100, 100, 55, 145},
    {55, 145, 10, 100},
    {10, 100, 10, 10}
};

void myDrawing(Draw &d) {
    for (auto &l : lines) {
        d.line(l.x1 * 3, l.y1 * 2, l.x2 * 3, l.y2 * 2);
        d.line(100 + l.x1, 40 + l.y1, 100 + l.x2, 40 + l.y2);
    }
}

int main() {
    Draw d(800, 600, "Test Figure", &myDrawing);
    return 0;
}