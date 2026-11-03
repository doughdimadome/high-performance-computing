#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <GL/glew.h>
#include <vector>
#include <memory>
#include "library.hpp"
#include "camera_light.hpp"

// Interleaved vertex opbouw (Positie, Normaal, UV-coördinaat)
struct Vertex {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};


// ABSTRACTE BASISKLASSE: Shape
// Handleiding-eisen: Polymorfisme met virtual functions en RAII

class Shape {
protected:
    GLuint vao = 0, vbo = 0, ebo = 0;
    unsigned int indexCount = 0;

    void setupGpuBuffers(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

public:
    Vec3 position{0.0f, 0.0f, 0.0f};
    Vec3 rotation{0.0f, 0.0f, 0.0f};
    Vec3 scale{1.0f, 1.0f, 1.0f};
    Material material;

    virtual ~Shape();
    virtual void generateGeometry() = 0; // Pure virtual methode

    // Berekent de Model Matrix: M = T * Ry * Rx * Rz * S
    Matrix4 getModelMatrix() const {
        Translate t(position.x, position.y, position.z);
        RotateX rx(rotation.x);
        RotateY ry(rotation.y);
        RotateZ rz(rotation.z);
        Scale s(scale.x, scale.y, scale.z);
        return t * ry * rx * rz * s;
    }

    void draw() const;
};


// AFGELEIDE KLASSEN
// Handleiding-eisen: Kubus, Piramide, Cilinder, Bol

class Cube : public Shape { public: void generateGeometry() override; };
class Pyramid : public Shape { public: void generateGeometry() override; };
class Cylinder : public Shape { public: void generateGeometry() override; };
class Sphere : public Shape { public: void generateGeometry() override; };


// DESIGN PATTERN: Factory Pattern
// Handleiding-eisen: Relevante design patterns toegepast
enum class ShapeType { CUBE, PYRAMID, CYLINDER, SPHERE };

class ShapeFactory {
public:
    static std::unique_ptr<Shape> createShape(ShapeType type);
};

#endif // SHAPES_HPP