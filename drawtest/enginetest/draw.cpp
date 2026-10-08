#include "draw.hpp"

LRESULT CALLBACK Draw::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DESTROY || uMsg == WM_CLOSE) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

Draw::Draw(int w, int h, const char* title) : width(w), height(h) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "GameEngineWindowClass";
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClass(&wc);

    RECT rect = { 0, 0, width, height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, FALSE);

    hwnd = CreateWindowEx(
        0, "GameEngineWindowClass", title,
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    ShowWindow(hwnd, SW_SHOW);

    hdcReal = GetDC(hwnd);
    hdcMem = CreateCompatibleDC(hdcReal);
    hbmMem = CreateCompatibleBitmap(hdcReal, width, height);
    hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

    // Witte pen instellen voor de lijnen
    HPEN whitePen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
    SelectObject(hdcMem, whitePen);
}

Draw::~Draw() {
    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdcReal);
    DestroyWindow(hwnd);
}

bool Draw::isOpen() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            running = false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return running;
}

void Draw::clear() {
    RECT rect = { 0, 0, width, height };
    HBRUSH blackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    FillRect(hdcMem, &rect, blackBrush);
}

void Draw::line(float x1, float y1, float x2, float y2) {
    MoveToEx(hdcMem, static_cast<int>(x1), static_cast<int>(y1), NULL);
    LineTo(hdcMem, static_cast<int>(x2), static_cast<int>(y2));
}

void Draw::display() {
    BitBlt(hdcReal, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);
}