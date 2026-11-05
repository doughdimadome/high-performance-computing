#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <GL/glew.h>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "library.hpp"
#include "camera_light.hpp"

struct Vertex {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

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
    GLuint textureID = 0;

    virtual ~Shape();
    virtual void generateGeometry() = 0;

    // MODEL MATRIX: Nu volledig via OpenGL (GLM)
    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f); // Identiteitsmatrix van GLM
        model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));
        return model;
    }

    void draw() const;
};

class Cube : public Shape { public: void generateGeometry() override; };
class Pyramid : public Shape { public: void generateGeometry() override; };
class Cylinder : public Shape { public: void generateGeometry() override; };
class Sphere : public Shape { public: void generateGeometry() override; };

enum class ShapeType { CUBE, PYRAMID, CYLINDER, SPHERE };

class ShapeFactory {
public:
    static std::unique_ptr<Shape> createShape(ShapeType type);
};

#endif // SHAPES_HPP