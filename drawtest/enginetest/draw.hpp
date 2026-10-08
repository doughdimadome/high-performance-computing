#ifndef DRAW_HPP
#define DRAW_HPP

#include <windows.h>

class Draw {
private:
    HWND hwnd{NULL};
    HDC hdcMem{NULL};
    HBITMAP hbmMem{NULL};
    HBITMAP hbmOld{NULL};
    HDC hdcReal{NULL};
    int width{800};
    int height{600};
    bool running{true};

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    Draw(int w = 800, int h = 600, const char* title = "3D Game Engine Window");
    ~Draw();

    bool isOpen();
    void clear();
    void line(float x1, float y1, float x2, float y2);
    void display();
};

#endif // DRAW_HPP