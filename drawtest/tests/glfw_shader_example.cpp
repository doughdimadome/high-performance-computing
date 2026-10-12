// compile on MacOs:
// c++ -I /opt/homebrew/include -L /opt/homebrew/lib -o glfw_shader_example -lglfw -lglew -framework OpenGL -DGL_SILENCE_DEPRECATION glfw_shader_example.cpp

// compile on Windows:
// g++ -o glfw_shader_example.exe glfw_shader_example.cpp -lglfw3 -lopengl32 -lglew32

#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Exit when a key is pressed
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    glfwTerminate();
    exit(0);
}

int main() {
    // Initialize the GLFW library
    if (!glfwInit()) {
        exit(-1);
    }

    // Create a windowed mode window and its OpenGL context
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(840, 640, "Shader example", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(-1);
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Init GLEW
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        exit(-1);
    }

    // Set the callback function for keypresses
    glfwSetKeyCallback(window, keyCallback);

    // Compile vertex shader
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const char *vsSource =
        "#version 330 core\n\
        in vec3 pos; \
        void main() { \
            gl_Position = vec4(pos, 1.0); \
        }";
    glShaderSource(vs, 1, &vsSource, NULL);
    glCompileShader(vs);

    // Compile fragment shader
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fsSource = 
        "#version 330 core\n \
        out vec4 fragColor; \
        void main() { \
            fragColor = vec4(0.5, 0.5, 1.0, 1.0); \
        }";
    glShaderSource(fs, 1, &fsSource, NULL);
    glCompileShader(fs);

    // Combine the two shaders into a program
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glUseProgram(program);

    // Vertices for triangle
    GLfloat vertices[] = {
        -1,  0, 0,
         0,  1, 0,
         1, -1, 0
    };

    // Create and bind a Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // Create and bind a Vertex Buffer Object
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    // Fill the VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Positions using 3 floats per vertex
    GLint posLocation = glGetAttribLocation(program, "pos");
    glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posLocation);

    // Set the background color
    glClearColor(0.8, 0.8, 0.8, 1.0); // r, g, b, alpha

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Clear canvas
        glClear(GL_COLOR_BUFFER_BIT);

        // Tell opengl to draw the vertices in the arrayBuffer as a line
        glDrawArrays(GL_LINE_LOOP, 0, 3);

        // Swap buffers
        glfwSwapBuffers(window);

        // Wait for keypress
        glfwWaitEvents();
    }
    
    // Terminate the GLFW library
    glfwTerminate();
    return 0;
}