#include "shapes.hpp"
#include <cmath>

Shape::~Shape() {
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);
}

// gpu buffer setup for vertices and indices voor OpenGL rendering
void Shape::setupGpuBuffers(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    indexCount = static_cast<unsigned int>(indices.size());

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Attribute 0: Positie (X, Y, Z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
    glEnableVertexAttribArray(0);

    // Attribute 1: Normaalvector (NX, NY, NZ)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));
    glEnableVertexAttribArray(1);

    // Attribute 2: Textuurcoordinaten (U, V)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}
// tekent de shape met de huidige OpenGL context en shaderprogramma
void Shape::draw() const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
// GEOMETRIE GENERATIE (CUBE, PIRAMIDE, SPHERE, PLANE)

// KUBUS (Middelpunt 0,0,0 - Ribbelengte 1)
void Cube::generateGeometry() {
    std::vector<Vertex> vertices = {
        // Voorkant
        {-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f},
        { 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f},
        { 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f},
        {-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f},
        // Achterkant
        {-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f},
        {-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f},
        { 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f},
        { 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f},
        // Bovenkant
        {-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f},
        {-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f},
        { 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f},
        { 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f},
        // Onderkant
        {-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f},
        { 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f},
        { 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f},
        {-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f},
        // Rechterkant
        { 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
        { 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f},
        { 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f},
        { 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f},
        // Linkerkant
        {-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f},
        {-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
        {-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f},
        {-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f}
    };
    // Indexen voor de kubus (6 zijden, 2 driehoeken per zijde)
    std::vector<unsigned int> indices = {
         0,  1,  2,  2,  3,  0,
         4,  5,  6,  6,  7,  4,
         8,  9, 10, 10, 11,  8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    setupGpuBuffers(vertices, indices);
}

// PIRAMIDE (Grondvlak 1x1 op Y=-0.5, Hoogte 1 op Y=0.5)
void Pyramid::generateGeometry() {
    std::vector<Vertex> vertices = {
        // Grondvlak
        {-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f},
        { 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f},
        { 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f},
        {-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f},
        // Zijvlakken (Top op 0.0, 0.5, 0.0)
        {-0.5f, -0.5f,  0.5f,  0.0f,  0.894f,  0.447f, 0.0f, 0.0f},
        { 0.5f, -0.5f,  0.5f,  0.0f,  0.894f,  0.447f, 1.0f, 0.0f},
        { 0.0f,  0.5f,  0.0f,  0.0f,  0.894f,  0.447f, 0.5f, 1.0f},

        { 0.5f, -0.5f,  0.5f,  0.447f, 0.894f,  0.0f,  0.0f, 0.0f},
        { 0.5f, -0.5f, -0.5f,  0.447f, 0.894f,  0.0f,  1.0f, 0.0f},
        { 0.0f,  0.5f,  0.0f,  0.447f, 0.894f,  0.0f,  0.5f, 1.0f},

        { 0.5f, -0.5f, -0.5f,  0.0f,  0.894f, -0.447f, 0.0f, 0.0f},
        {-0.5f, -0.5f, -0.5f,  0.0f,  0.894f, -0.447f, 1.0f, 0.0f},
        { 0.0f,  0.5f,  0.0f,  0.0f,  0.894f, -0.447f, 0.5f, 1.0f},

        {-0.5f, -0.5f, -0.5f, -0.447f, 0.894f,  0.0f,  0.0f, 0.0f},
        {-0.5f, -0.5f,  0.5f, -0.447f, 0.894f,  0.0f,  1.0f, 0.0f},
        { 0.0f,  0.5f,  0.0f, -0.447f, 0.894f,  0.0f,  0.5f, 1.0f}
    };
    // Indexen voor de piramide (1 grondvlak + 4 zijvlakken)
    std::vector<unsigned int> indices = {
        0, 2, 1, 0, 3, 2,
        4, 5, 6,
        7, 8, 9,
        10, 11, 12,
        13, 14, 15
    };

    setupGpuBuffers(vertices, indices);
}

// CILINDER (Middelpunt 0,0,0 - Hoogte 1, Diameter 1)
void Cylinder::generateGeometry() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const int segments = 32;
    const float radius = 0.5f;
    const float halfHeight = 0.5f;

    // Mantel (zijkant)
    for (int i = 0; i <= segments; ++i) {
        float angle = i * 2.0f * 3.1415926535f / segments;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        float u = static_cast<float>(i) / segments;

        // Onderste cirkelpunt
        vertices.push_back({x, -halfHeight, z, std::cos(angle), 0.0f, std::sin(angle), u, 0.0f});
        // Bovenste cirkelpunt
        vertices.push_back({x,  halfHeight, z, std::cos(angle), 0.0f, std::sin(angle), u, 1.0f});
    }
    // Indexen voor de mantel
    for (int i = 0; i < segments; ++i) {
        indices.push_back(i * 2);
        indices.push_back(i * 2 + 1);
        indices.push_back((i + 1) * 2 + 1);

        indices.push_back(i * 2);
        indices.push_back((i + 1) * 2 + 1);
        indices.push_back((i + 1) * 2);
    }

    setupGpuBuffers(vertices, indices);
}

// BOL (Middelpunt 0,0,0 - Diameter 1)
void Sphere::generateGeometry() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    // Parameters voor de bol
    const unsigned int X_SEGMENTS = 30;
    const unsigned int Y_SEGMENTS = 30;
    const float radius = 0.5f;
    // Genereer de bolpunten
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = radius * std::cos(xSegment * 2.0f * 3.1415926535f) * std::sin(ySegment * 3.1415926535f);
            float yPos = radius * std::cos(ySegment * 3.1415926535f);
            float zPos = radius * std::sin(xSegment * 2.0f * 3.1415926535f) * std::sin(ySegment * 3.1415926535f);

            // Bij een bol is de normaalvector simpelweg de genormaliseerde positie
            float nx = xPos / radius;
            float ny = yPos / radius;
            float nz = zPos / radius;
            // push_back van de vertex met positie, normaal en UV-coördinaten
            vertices.push_back({xPos, yPos, zPos, nx, ny, nz, xSegment, ySegment});
        }
    }
    // Genereer de indexen voor de bol
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
        for (unsigned int x = 0; x < X_SEGMENTS; ++x) {
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x + 1);

            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x + 1);
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x + 1);
        }
    }
    
    setupGpuBuffers(vertices, indices);
}

void Cone::generateGeometry() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    // Parameters voor de kegel
    const int segments = 32;
    const float radius = 0.5f;
    const float halfHeight = 0.5f;
    // Apex van de kegel
    Vertex apex = {0.0f, halfHeight, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f};
    // Genereer de basis van de kegel
    for (int i = 0; i <= segments; ++i) {
        float angle = i * 2.0f * 3.1415926535f / segments;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        float u = static_cast<float>(i) / segments;
        // Normaalvector voor de zijkant van de kegel
        float nx = std::cos(angle);
        float ny = 0.5f;
        float nz = std::sin(angle);
        // Voeg de vertex toe voor de basis van de kegel
        vertices.push_back({x, -halfHeight, z, nx, ny, nz, u, 0.0f});
    }
    // Voeg de apex toe aan de vertices
    vertices.push_back(apex);
    unsigned int apexIndex = static_cast<unsigned int>(vertices.size() - 1);
    // Genereer de indexen voor de zijkant van de kegel
    for (int i = 0; i < segments; ++i) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(apexIndex);
    }

    setupGpuBuffers(vertices, indices);
}

// FACTORY PATTERN IMPLEMENTATIE

std::unique_ptr<Shape> ShapeFactory::createShape(ShapeType type) {
    std::unique_ptr<Shape> shape;

    switch (type) {
        case ShapeType::CUBE:
            shape = std::make_unique<Cube>();
            break;
        case ShapeType::PYRAMID:
            shape = std::make_unique<Pyramid>();
            break;
        case ShapeType::CYLINDER:
            shape = std::make_unique<Cylinder>();
            break;
        case ShapeType::SPHERE:
            shape = std::make_unique<Sphere>();
            break;
        case ShapeType::CONE:
            shape = std::make_unique<Cone>();
            break;
    }

    if (shape) {
        shape->generateGeometry();
    }

    return shape;
}