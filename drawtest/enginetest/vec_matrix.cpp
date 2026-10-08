#include "library.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>


// VECTOR IMPLEMENTATIE


void print(const Vec &vec) {
    std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")\n";
}

void perspective_divide(Vec &vec) {
    if (std::abs(vec.w) > 1e-6f) {
        vec.x /= vec.w;
        vec.y /= vec.w;
        vec.z /= vec.w;
        vec.w = 1.0f;
    }
}


// MATRIX IMPLEMENTATIE


void print(const Matrix &matrix) {
    for (int i = 0; i < 4; ++i) {
        std::cout << "[ ";
        for (int j = 0; j < 4; ++j) {
            std::cout << std::setw(8) << std::setprecision(3) << matrix.m[i][j] << " ";
        }
        std::cout << "]\n";
    }
}

void multiply(Matrix &matrix, float factor) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            matrix.m[i][j] *= factor;
        }
    }
}

void multiply(Matrix &matrix, const Matrix &matrix1) {
    Matrix res{};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            res.m[i][j] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                res.m[i][j] += matrix.m[i][k] * matrix1.m[k][j];
            }
        }
    }
    matrix = res;
}

Vec multiply(const Matrix &matrix, const Vec &vec) {
    Vec res;
    res.x = matrix.m[0][0] * vec.x + matrix.m[0][1] * vec.y + matrix.m[0][2] * vec.z + matrix.m[0][3] * vec.w;
    res.y = matrix.m[1][0] * vec.x + matrix.m[1][1] * vec.y + matrix.m[1][2] * vec.z + matrix.m[1][3] * vec.w;
    res.z = matrix.m[2][0] * vec.x + matrix.m[2][1] * vec.y + matrix.m[2][2] * vec.z + matrix.m[2][3] * vec.w;
    res.w = matrix.m[3][0] * vec.x + matrix.m[3][1] * vec.y + matrix.m[3][2] * vec.z + matrix.m[3][3] * vec.w;
    return res;
}

// TRANSFORMATIES & PROJECTIES

Matrix create_scale(float sx, float sy, float sz) {
    Matrix mat{};
    mat.m[0][0] = sx;
    mat.m[1][1] = sy;
    mat.m[2][2] = sz;
    return mat;
}

Matrix create_translation(float tx, float ty, float tz) {
    Matrix mat{};
    mat.m[0][3] = tx;
    mat.m[1][3] = ty;
    mat.m[2][3] = tz;
    return mat;
}

Matrix create_rotation_x(float angle_rad) {
    Matrix mat{};
    float c = std::cos(angle_rad);
    float s = std::sin(angle_rad);
    mat.m[1][1] = c;  mat.m[1][2] = -s;
    mat.m[2][1] = s;  mat.m[2][2] = c;
    return mat;
}

Matrix create_rotation_y(float angle_rad) {
    Matrix mat{};
    float c = std::cos(angle_rad);
    float s = std::sin(angle_rad);
    mat.m[0][0] = c;   mat.m[0][2] = s;
    mat.m[2][0] = -s;  mat.m[2][2] = c;
    return mat;
}

Matrix create_projection_ortho() {
    Matrix mat{};
    mat.m[2][2] = 0.0f;
    return mat;
}

Matrix create_projection_parallel() {
    Matrix mat{};
    mat.m[0][2] = 0.5f;
    mat.m[1][2] = 0.5f;
    mat.m[2][2] = 0.0f;
    return mat;
}

Matrix create_projection_iso() {
    Matrix mat{};
    mat.m[0][0] = 1.0f / std::sqrt(2.0f);
    mat.m[0][2] = 1.0f / std::sqrt(2.0f);
    mat.m[1][0] = 1.0f / std::sqrt(6.0f);
    mat.m[1][1] = std::sqrt(2.0f / 3.0f);
    mat.m[1][2] = -1.0f / std::sqrt(6.0f);
    mat.m[2][2] = 0.0f;
    return mat;
}

Matrix create_projection_perspective(float zp) {
    Matrix mat{};
    mat.m[0][0] = zp;
    mat.m[1][1] = zp;
    mat.m[2][2] = zp;
    mat.m[3][2] = 1.0f;
    mat.m[3][3] = 0.0f;
    return mat;
}