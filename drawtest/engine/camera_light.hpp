#ifndef CAMERA_LIGHT_HPP
#define CAMERA_LIGHT_HPP

#include "library.hpp"

// Materiaalstructuur voor het Blinn-Phong belichtingsmodel
struct Material {
    Vec3 ambient{0.2f, 0.2f, 0.2f};
    Vec3 diffuse{1.0f, 1.0f, 1.0f};
    Vec3 specular{1.0f, 1.0f, 1.0f};
    float shininess{32.0f};
    float opacity{1.0f};
};

// Camera klasse voor FPS-stijl besturing en View Matrix berekeningen
class Camera {
public:
    Vec3 position{0.0f, 1.0f, 5.2f};
    float pitch{0.0f};
    float yaw{0.0f};
    float speed{3.5f};
    float fov{45.0f}; // Field of view (dynamisch aanpasbaar met zoom)

    // View Matrix genereren op basis van positie en kijkhoek
    Matrix4 getViewMatrix() const {
        RotateX rx(pitch);
        RotateY ry(yaw);
        Translate t(-position.x, -position.y, -position.z);
        return rx * ry * t;
    }

    // Zoom-functie via het muiswiel
    void zoom(float yoffset) {
        fov -= yoffset;
        if (fov < 1.0f) fov = 1.0f;
        if (fov > 90.0f) fov = 90.0f;
    }

    // Richtingsbewegingen (WASD + Spatie/Shift)
    void moveForward(float dt)  { position.x -= std::sin(yaw) * speed * dt; position.z -= std::cos(yaw) * speed * dt; }
    void moveBackward(float dt) { position.x += std::sin(yaw) * speed * dt; position.z += std::cos(yaw) * speed * dt; }
    void moveLeft(float dt)     { position.x -= std::cos(yaw) * speed * dt; position.z += std::sin(yaw) * speed * dt; }
    void moveRight(float dt)    { position.x += std::cos(yaw) * speed * dt; position.z -= std::sin(yaw) * speed * dt; }
    void moveUp(float dt)       { position.y += speed * dt; }
    void moveDown(float dt)     { position.y -= speed * dt; }
};

// Lichtbron klasse
class Light {
public:
    Vec3 position{1.2f, 2.5f, 2.0f};
    Vec3 color{1.0f, 1.0f, 1.0f};
};

#endif // CAMERA_LIGHT_HPP