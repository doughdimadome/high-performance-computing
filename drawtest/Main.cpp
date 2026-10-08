#include <iostream>
#include "library.hpp"

int main() {
    std::cout << "=== OEFENING 1 & 2: 3D TRANSFORMATIES ===\n\n";

    Vec v{ .x = 2.0f, .y = 3.0f, .z = 4.0f, .w = 1.0f };

    // 1. Schaling (2 in x, 3 in y, 0.5 in z)
    Matrix S = create_scale(2.0f, 3.0f, 0.5f);
    Vec v_scaled = multiply(S, v);
    std::cout << "1. Schaling v: "; print(v_scaled);

    // 2. Translatie (3, 4, 5)
    Matrix T = create_translation(3.0f, 4.0f, 5.0f);
    Vec v_translated = multiply(T, v);
    std::cout << "2. Translatie v: "; print(v_translated);

    // 3. Rotatie 90 deg (1.5708 rad) rond Y-as
    Matrix R = create_rotation_y(1.57079632679f);
    Vec v_rotated = multiply(R, v);
    std::cout << "3. Rotatie 90 deg Y-as v: "; print(v_rotated);

    std::cout << "\n=== OEFENING 4: KUBUS TRANSFORMATIEMATRIX ===\n\n";

    Matrix P = create_projection_parallel();
    Matrix S_cube = create_scale(100.0f, 100.0f, 1.0f);
    Matrix T_cube = create_translation(400.0f, 300.0f, 0.0f);

    Matrix transform = T_cube;
    multiply(transform, S_cube);
    multiply(transform, P);

    std::cout << "Gecombineerde Transformatiematrix voor Kubus:\n";
    print(transform);

    Vec vertex{ .x = 1.0f, .y = 1.0f, .z = 1.0f, .w = 1.0f };
    Vec projected_vertex = multiply(transform, vertex);
    perspective_divide(projected_vertex);

    std::cout << "\nGeprojecteerd punt op scherm: ";
    print(projected_vertex);

    return 0;
}