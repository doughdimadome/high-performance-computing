#include <stdlib.h>
#include "draw.h"

Draw::Draw(int width, int height, const char *windowTitle, void func(Draw &d)) {
    // Initialize the GLFW library
    glfwInit();

    // Create a window
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Synchronize buffer swap interval with refresh rate
    glfwSwapInterval(1);

    // Set the callback function for keypresses
    glfwSetKeyCallback(window, keyCallBack);

    // Define orthographic projection
    glOrtho(0, width, 0, height, -1, 1);

    // Set point size
    glPointSize(4);

    // Set line width
    glLineWidth(2);
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {

        // Clear the window
        glClear(GL_COLOR_BUFFER_BIT);

        // Set default color
        glColor3f(1, 1, 1);

        // Call drawing function
        func(*this);

        // Show buffer
        glfwSwapBuffers(window);

        // Check for events (window closing)
        glfwPollEvents();
    }

    // Terminate the GLFW library
    glfwTerminate();
}

void Draw::point(float x, float y) {
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
}

void Draw::line(float x1, float y1, float x2, float y2) {
    // Draw a line
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void Draw::color(float r, float g, float b) {
    glColor3f(r, g, b);
}

double Draw::getTime() {
    return glfwGetTime();
}

// Exit when a key is pressed
void Draw::keyCallBack(GLFWwindow *window, int key, int scancode, int action, int mods) {
    glfwTerminate();
    exit(0);
}
