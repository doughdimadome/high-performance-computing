#ifndef CAMERA_LIGHT_HPP
#define CAMERA_LIGHT_HPP

#include "library.hpp"

struct Material {
    Vec3 ambient{0.05f, 0.2f, 0.05f};
    Vec3 diffuse{0.1f, 0.7f, 0.1f};
    Vec3 specular{1.0f, 1.0f, 1.0f};
    float shininess{32.0f};
    float opacity{1.0f};
};

class Camera {
public:
    Vec3 position{0.0f, 1.0f, 5.2f};
    float pitch{0.15f}; // Rotatie om X-as (omhoog/omlaag)
    float yaw{0.0f};    // Rotatie om Y-as (links/rechts)
    float speed{3.5f};  // Loop-snelheid in eenheden per seconde

    Matrix4 getViewMatrix() const {
        RotateX rx(pitch);
        RotateY ry(yaw);
        Translate t(-position.x, -position.y, -position.z);
        return rx * ry * t;
    }

    
    // BEWEGINGSFUNCTIES BINNEN DE CAMERA KLASSE
    
    void moveForward(float dt) {
        position.x -= std::sin(yaw) * speed * dt;
        position.z -= std::cos(yaw) * speed * dt;
    }

    void moveBackward(float dt) {
        position.x += std::sin(yaw) * speed * dt;
        position.z += std::cos(yaw) * speed * dt;
    }

    void moveLeft(float dt) {
        position.x -= std::cos(yaw) * speed * dt;
        position.z += std::sin(yaw) * speed * dt;
    }

    void moveRight(float dt) {
        position.x += std::cos(yaw) * speed * dt;
        position.z -= std::sin(yaw) * speed * dt;
    }

    void moveUp(float dt)   { position.y += speed * dt; }
    void moveDown(float dt) { position.y -= speed * dt; }
};

class Light {
public:
    Vec3 position{1.2f, 1.8f, 2.0f};
    Vec3 color{1.0f, 1.0f, 1.0f};
};

#endif // CAMERA_LIGHT_HPP