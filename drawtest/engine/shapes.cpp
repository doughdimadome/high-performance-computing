#include "shapes.hpp"

void Shape::setupGpuBuffers(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    indexCount = indices.size();
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Attribute 0: Positie (3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
    glEnableVertexAttribArray(0);

    // Attribute 1: Normaal (3 floats)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));
    glEnableVertexAttribArray(1);

    // Attribute 2: UV TexCoord (2 floats)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

Shape::~Shape() {
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);
}

void Shape::draw() const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// 1. KUBUS (24 vertices voor unieke per-vlak normalen)
void Cube::generateGeometry() {
    std::vector<Vertex> vertices = {
        // Front (Z+)
        {-0.5f,-0.5f, 0.5f, 0,0,1, 0,0}, { 0.5f,-0.5f, 0.5f, 0,0,1, 1,0}, { 0.5f, 0.5f, 0.5f, 0,0,1, 1,1}, {-0.5f, 0.5f, 0.5f, 0,0,1, 0,1},
        // Back (Z-)
        {-0.5f,-0.5f,-0.5f, 0,0,-1, 1,0}, {-0.5f, 0.5f,-0.5f, 0,0,-1, 1,1}, { 0.5f, 0.5f,-0.5f, 0,0,-1, 0,1}, { 0.5f,-0.5f,-0.5f, 0,0,-1, 0,0},
        // Top (Y+)
        {-0.5f, 0.5f,-0.5f, 0,1,0, 0,1}, {-0.5f, 0.5f, 0.5f, 0,1,0, 0,0}, { 0.5f, 0.5f, 0.5f, 0,1,0, 1,0}, { 0.5f, 0.5f,-0.5f, 0,1,0, 1,1},
        // Bottom (Y-)
        {-0.5f,-0.5f,-0.5f, 0,-1,0, 1,1}, { 0.5f,-0.5f,-0.5f, 0,-1,0, 0,1}, { 0.5f,-0.5f, 0.5f, 0,-1,0, 0,0}, {-0.5f,-0.5f, 0.5f, 0,-1,0, 1,0},
        // Right (X+)
        { 0.5f,-0.5f,-0.5f, 1,0,0, 1,0}, { 0.5f, 0.5f,-0.5f, 1,0,0, 1,1}, { 0.5f, 0.5f, 0.5f, 1,0,0, 0,1}, { 0.5f,-0.5f, 0.5f, 1,0,0, 0,0},
        // Left (X-)
        {-0.5f,-0.5f,-0.5f,-1,0,0, 0,0}, {-0.5f,-0.5f, 0.5f,-1,0,0, 1,0}, {-0.5f, 0.5f, 0.5f,-1,0,0, 1,1}, {-0.5f, 0.5f,-0.5f,-1,0,0, 0,1}
    };
    std::vector<unsigned int> indices = {
        0,1,2, 2,3,0,    4,5,6, 6,7,4,    8,9,10, 10,11,8,
        12,13,14, 14,15,12, 16,17,18, 18,19,16, 20,21,22, 22,23,20
    };
    setupGpuBuffers(vertices, indices);
}

// 2. PIRAMIDE
void Pyramid::generateGeometry() {
    std::vector<Vertex> vertices = {
        // Grondvlak (Y-)
        {-0.5f, -0.5f, -0.5f,  0,-1,0,  0,0}, { 0.5f, -0.5f, -0.5f,  0,-1,0,  1,0},
        {0.5f, -0.5f,  0.5f,  0,-1,0,  1,1}, {-0.5f, -0.5f,  0.5f,  0,-1,0,  0,1},
        
        // Zijvlakken met correcte UV-driehoeksmapping (onder min-Y tot top op Y=0.5)
        {-0.5f, -0.5f,  0.5f,  0, 0.707f,  0.707f,  0.0f, 0.0f},
        { 0.5f, -0.5f,  0.5f,  0, 0.707f,  0.707f,  1.0f, 0.0f},
        { 0.0f,  0.5f,  0.0f,  0, 0.707f,  0.707f,  0.5f, 1.0f},

        { 0.5f, -0.5f,  0.5f,  0.707f, 0.707f, 0,  0.0f, 0.0f},
        { 0.5f, -0.5f, -0.5f,  0.707f, 0.707f, 0,  1.0f, 0.0f},
        { 0.0f,  0.5f,  0.0f,  0.707f, 0.707f, 0,  0.5f, 1.0f},

        { 0.5f, -0.5f, -0.5f,  0, 0.707f, -0.707f,  0.0f, 0.0f},
        {-0.5f, -0.5f, -0.5f,  0, 0.707f, -0.707f,  1.0f, 0.0f},
        { 0.0f,  0.5f,  0.0f,  0, 0.707f, -0.707f,  0.5f, 1.0f},

        {-0.5f, -0.5f, -0.5f, -0.707f, 0.707f, 0,  0.0f, 0.0f},
        {-0.5f, -0.5f,  0.5f, -0.707f, 0.707f, 0,  1.0f, 0.0f},
        { 0.0f,  0.5f,  0.0f, -0.707f, 0.707f, 0,  0.5f, 1.0f}
    };
    std::vector<unsigned int> indices = {
        0,2,1, 0,3,2,
        4,5,6, 7,8,9, 10,11,12, 13,14,15
    };
    setupGpuBuffers(vertices, indices);
}

// 3. CILINDER (Inclusief boven- en onderkap)
void Cylinder::generateGeometry() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    const int sectors = 24;
    const float radius = 0.5f;
    const float height = 1.0f;

    // Zijwand
    for (int i = 0; i <= sectors; ++i) {
        float angle = i * (2.0f * PI / sectors);
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        float u = (float)i / sectors; // Loopt horizontaal van 0.0 naar 1.0 om de cilinder

        // Onderste vertex -> v = 0.0, Bovenste vertex -> v = 1.0
        vertices.push_back({x, -height/2.0f, z, std::cos(angle), 0.0f, std::sin(angle), u, 0.0f});
        vertices.push_back({x,  height/2.0f, z, std::cos(angle), 0.0f, std::sin(angle), u, 1.0f});
    }

    for (int i = 0; i < sectors; ++i) {
        indices.push_back(i * 2);
        indices.push_back(i * 2 + 1);
        indices.push_back((i + 1) * 2 + 1);

        indices.push_back(i * 2);
        indices.push_back((i + 1) * 2 + 1);
        indices.push_back((i + 1) * 2);
    }

    // Top Cap (Bovendeksel)
    unsigned int topCenterIndex = vertices.size();
    vertices.push_back({0.0f, height/2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f});

    for (int i = 0; i <= sectors; ++i) {
        float angle = i * (2.0f * PI / sectors);
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        float u = 0.5f + 0.5f * std::cos(angle);
        float v = 0.5f + 0.5f * std::sin(angle);
        vertices.push_back({x, height/2.0f, z, 0.0f, 1.0f, 0.0f, u, v});
    }

    for (int i = 0; i < sectors; ++i) {
        indices.push_back(topCenterIndex);
        indices.push_back(topCenterIndex + 1 + i + 1);
        indices.push_back(topCenterIndex + 1 + i);
    }

    // Bottom Cap (Onderdeksel)
    unsigned int bottomCenterIndex = vertices.size();
    vertices.push_back({0.0f, -height/2.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.5f, 0.5f});

    for (int i = 0; i <= sectors; ++i) {
        float angle = i * (2.0f * PI / sectors);
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        float u = 0.5f + 0.5f * std::cos(angle);
        float v = 0.5f + 0.5f * std::sin(angle);
        vertices.push_back({x, -height/2.0f, z, 0.0f, -1.0f, 0.0f, u, v});
    }

    for (int i = 0; i < sectors; ++i) {
        indices.push_back(bottomCenterIndex);
        indices.push_back(bottomCenterIndex + 1 + i);
        indices.push_back(bottomCenterIndex + 1 + i + 1);
    }

    setupGpuBuffers(vertices, indices);
}

// 4. BOL
void Sphere::generateGeometry() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    const unsigned int X_SEGMENTS = 20;
    const unsigned int Y_SEGMENTS = 20;
    const float r = 0.5f;

    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;

            float xPos = r * std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = r * std::cos(ySegment * PI);
            float zPos = r * std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            Vec3 norm = normalize({xPos, yPos, zPos});
            
            // u loopt rondom (0 -> 1), v loopt van top naar bodem (1 -> 0)
            vertices.push_back({xPos, yPos, zPos, norm.x, norm.y, norm.z, xSegment, 1.0f - ySegment});
        }
    }

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

// Factory Pattern
std::unique_ptr<Shape> ShapeFactory::createShape(ShapeType type) {
    std::unique_ptr<Shape> shape;
    switch (type) {
        case ShapeType::CUBE:     shape = std::make_unique<Cube>(); break;
        case ShapeType::PYRAMID:  shape = std::make_unique<Pyramid>(); break;
        case ShapeType::CYLINDER: shape = std::make_unique<Cylinder>(); break;
        case ShapeType::SPHERE:   shape = std::make_unique<Sphere>(); break;
    }
    shape->generateGeometry();
    return shape;
}