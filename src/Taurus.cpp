#include <cmath>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "Taurus.h"
using namespace std;

Taurus::Taurus(int vert, int horiz, float outer, float inner) {
    init(vert, horiz, outer, inner);
}

float Taurus::toRadians(float degrees) {
    return (degrees * 3.14159f) / 180.0f;
}

void Taurus::init(int vert, int horiz, float outer, float inner) {

    numVertices = (vert + 1) * (horiz + 1);
    numIndices = vert * horiz * 6;

    // Generate 2d base vertices
    float theta = toRadians(360.0f / vert);
    for (int i = 0; i <= vert; i++) {
        base2D.push_back(glm::vec3(cos(theta * i), sin(theta * i), 0.0f));
    }

    // Calculate scale factor
    scaleFactor = (inner + outer) == 0 ? 0 : outer / (inner + outer);

    // Scale base vertices
    for (int i = 0; i <= vert; i++) {
        base2D[i] *= scaleFactor;
    }

    // Calculate translation
    float translationFactor = 1 - scaleFactor;

    // Translate base vertices
    for (int i = 0; i <= vert; i++) {
        base2D[i].x += translationFactor;
    }

    // Calculate rotation
    float phi = toRadians(360.0f / horiz);

    // Generate vertices
    for (int i = 0; i <= horiz; i++) {
        glm::vec3 center = glm::vec3(translationFactor * cos(phi * i), 0.0f, - translationFactor * sin(phi * i));
        for (int j = 0; j <= vert; j++) {
            float x = base2D[j].x;
            float y = base2D[j].y;
            float z = base2D[j].z;

            glm::vec3 newVert = glm::vec3(x * cos(phi * i) + z * sin(phi * i), y, -x * sin(phi * i) + z * cos(phi * i));

            vertices.push_back(glm::vec3(newVert.x, newVert.y, newVert.z));
            normals.push_back(glm::normalize(newVert - center));
            float texY = ((float)j / (vert + 1.0f)) + 0.5f;
            if (texY > 1.0f) {
                texY -= 1.0f;
            }
            texCoords.push_back(glm::vec2((float)i / horiz, texY));
        }
    }

    // Determine winding order 
    bool cw = false; //rad < 0.5f && rad >= 0.0f || rad <= -0.5f;
    int p = cw ? 1 : 0;
    int q = 1 - p;

    // Generate indices
    for (int i = 0; i < horiz; i++) {
        for (int j = 0; j < vert; j++) {
            indices.push_back(i * (vert + 1) + j + q);
            indices.push_back(i * (vert + 1) + j + p);
            indices.push_back((i + 1) * (vert + 1) + j);
            indices.push_back(i * (vert + 1) + j + 1);
            indices.push_back((i + 1) * (vert + 1) + j + p);
            indices.push_back((i + 1) * (vert + 1) + j + q);
        }
    }

}

int Taurus::getNumVertices() {
    return numVertices;
}

int Taurus::getNumIndices() {
    return numIndices;
}

std::vector<glm::vec3> Taurus::getVertices() {
    return vertices;
}

std::vector<glm::vec3> Taurus::getNormals() {
    return normals;
}

std::vector<int> Taurus::getIndices() {
    return indices;
}

std::vector<glm::vec2> Taurus::getTexCoords() {
    return texCoords;
}