#ifndef CAMERA_LIGHT_HPP
#define CAMERA_LIGHT_HPP

#include "library.hpp"


// MATERIAALEIGENSCHAPPEN
// Handleiding-eisen: RGB, opaciteit, ambient, diffuse & speculair
struct Material {
    Vec3 ambient{0.05f, 0.2f, 0.05f};   // Omgevingslicht coëfficiënt
    Vec3 diffuse{0.1f, 0.7f, 0.1f};     // Basiskleur / Lambertiaanse reflectie
    Vec3 specular{1.0f, 1.0f, 1.0f};   // Glanskleur
    float shininess{32.0f};             // Scherpte van de glansvlek (exponent)
    float opacity{1.0f};                // Transparantie (1.0 = dekkend, 0.0 = onzichtbaar)
};


// CAMERA KLASSE
// Handleiding-eisen: Positie en oriëntatie instelbaar

class Camera {
public:
    Vec3 position{0.0f, 1.0f, 5.2f};
    float pitch{0.15f}; // Kijkhoek omhoog/omlaag
    float yaw{0.0f};   // Kijkhoek links/rechts

    // De View Matrix beweegt de hele wereld omgekeerd aan de camera
    Matrix4 getViewMatrix() const {
        RotateX rx(pitch);
        RotateY ry(yaw);
        Translate t(-position.x, -position.y, -position.z);
        return rx * ry * t;
    }
};


// LICHTBRON KLASSE
// Handleiding-eisen: Positie en kleur gespecificeerd

class Light {
public:
    Vec3 position{1.2f, 1.8f, 2.0f};
    Vec3 color{1.0f, 1.0f, 1.0f};
};

#endif // CAMERA_LIGHT_HPP