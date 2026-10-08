#ifndef MATHLIB_HPP
#define MATHLIB_HPP

// 4D VECTOR DEFINITIE (HOMOGENE COÖRDINATEN)

struct Vec4 {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    float w{1.0f}; // Homogene component (standaard 1 voor punten)

    // @brief Geeft een pointer naar de interne data voor array-style access
    // @return Constante float pointer naar x
    const float* get_vec() const {
        return &x;
    }
};

using Vec = Vec4;

// @brief Print een Vec4 overzichtelijk naar de console
// @param vec De te printen vector
void print(const Vec4 &vec);

// @brief Voert de perspectiefdeling uit op de vector (deelt x, y, z door w)
// @param vec De vector die aangepast wordt
void perspective_divide(Vec4 &vec);

// 4x4 MATRIX DEFINITIE

struct Matrix4 {
    float m[4][4]{
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
};

using Matrix = Matrix4;

// @brief Print de 4x4 matrix overzichtelijk naar de console
// @param matrix De te printen matrix
void print(const Matrix4 &matrix);

// @brief Vermenigvuldigt twee 4x4 matrices in-place (matrix = matrix * matrix1)
// @param matrix De eerste matrix die wordt overschreven
// @param matrix1 De tweede matrix
void multiply(Matrix4 &matrix, const Matrix4 &matrix1);

// @brief Vermenigvuldigt een 4x4 matrix met een 4D vector
// @param matrix De transformatiematrix
// @param vec De te transformeren vector
// @return Een nieuwe getransformeerde Vec4
Vec4 multiply(const Matrix4 &matrix, const Vec4 &vec);

// TRANSFORMATIE & PROJECTIE MATRICES

// @brief Maakt een 4x4 schaalmatrix
// @param sx Schaalfactor X
// @param sy Schaalfactor Y
// @param sz Schaalfactor Z
// @return De 4x4 schaalmatrix
Matrix4 create_scale(float sx, float sy, float sz);

// @brief Maakt een 4x4 translatiematrix
// @param tx Translatie X
// @param ty Translatie Y
// @param tz Translatie Z
// @return De 4x4 translatiematrix
Matrix4 create_translation(float tx, float ty, float tz);

// @brief Maakt een 4x4 rotatiematrix rond de X-as
// @param angle_rad Hoek in radialen
// @return De 4x4 rotatiematrix
Matrix4 create_rotation_x(float angle_rad);

// @brief Maakt een 4x4 rotatiematrix rond de Y-as
// @param angle_rad Hoek in radialen
// @return De 4x4 rotatiematrix
Matrix4 create_rotation_y(float angle_rad);

// @brief Maakt een 4x4 orthografische projectiematrix
// @return De orthografische projectiematrix
Matrix4 create_projection_ortho();

// @brief Maakt een 4x4 parallel projectiematrix
// @return De parallelle projectiematrix
Matrix4 create_projection_parallel();

// @brief Maakt een 4x4 isometrische projectiematrix
// @return De isometrische projectiematrix
Matrix4 create_projection_iso();

// @brief Maakt een 4x4 perspectief projectiematrix
// @param zp De Z-coördinaat van het projectievlak
// @return De perspectief projectiematrix
Matrix4 create_projection_perspective(float zp);

#endif // MATHLIB_HPP