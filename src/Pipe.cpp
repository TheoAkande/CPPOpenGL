#include <cmath>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Pipe.h"
using namespace std;

Pipe::Pipe(std::vector<glm::vec3> midPoints, int prec, float radius) {
    init(midPoints, prec, radius);
}

float Pipe::toRadians(float degrees) {
    return (degrees * 3.14159f) / 180.0f;
}

void Pipe::init(std::vector<glm::vec3> midPoints, int prec, float radius) {

    totalDistance = 0;

    distances.push_back(0);

    for (int i = 0; i < midPoints.size() - 1; i++) {
        totalDistance += (midPoints[i + 1] - midPoints[i]).length();
        distances.push_back(totalDistance);
    }

    numVertices = 2 * (prec + 2) + 2 * (prec + 1) * midPoints.size() - (prec + 1);
    numIndicesFace = prec + 2;
    numIndicesPipe = 12 * (midPoints.size() - 1) * prec;

    float maxValue = 0;
    for (int i = 0; i < midPoints.size(); i++) {
        if (abs(midPoints[i].x) > maxValue) {
            maxValue = abs(midPoints[i].x);
        }
        if (abs(midPoints[i].y) > maxValue) {
            maxValue = abs(midPoints[i].y);
        }
        if (abs(midPoints[i].z) > maxValue) {
            maxValue = abs(midPoints[i].z);
        }
    }
    if (radius > maxValue) {
        maxValue = radius;
    }
    float radiusScale = radius / maxValue;

    // Calculate face normals
    int numFaceNormals = midPoints.size();
    for (int i = 0; i < numFaceNormals - 1; i++) {
        faceNormals.push_back(glm::normalize(midPoints.at(i + 1) - midPoints.at(i)));
    }
    faceNormals.push_back(faceNormals.back());

    // Calculate start face
    vertices.push_back(midPoints[0] / maxValue);

    // nan vec
    glm::vec3 nanVec = glm::cross(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    // Get the perpendicular vector to the first face normal
    glm::vec3 perp = glm::normalize(glm::cross(faceNormals[0], glm::vec3(1.0f, 0.0f, 0.0f)));
    if (faceNormals[0].y == 0.0f && faceNormals[0].z == 0.0f) {
        perp = glm::vec3(0.0f, 1.0f, 0.0f);
    }
    // Calculate the points
    for (int i = 0; i <= prec; i++) {
        float amt = toRadians(i * 360.0f / prec);
        glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, faceNormals[0]);
        glm::vec3 newVert = glm::vec3(rMat * glm::vec4(perp, 1.0f));
        vertices.push_back(midPoints[0] / maxValue + newVert * radiusScale);
    }
    // Add normals
    for (int i = 0; i <= prec + 1; i++) {
        normals.push_back(-faceNormals[0]);
    }

    //Calculate the end face
    vertices.push_back(midPoints[midPoints.size() - 1] / maxValue);
    // Get the perpendicular vector to the last face normal
    perp = glm::normalize(glm::cross(faceNormals[numFaceNormals - 1], glm::vec3(1.0f, 0.0f, 0.0f)));
    if (faceNormals[numFaceNormals - 1].y == 0.0f && faceNormals[numFaceNormals - 1].z == 0.0f) {
        perp = glm::vec3(0.0f, 1.0f, 0.0f);
    }
    // Calculate the points
    for (int i = 0; i <= prec; i++) {
        float amt = toRadians(i * 360.0f / prec);
        glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, faceNormals[numFaceNormals - 1]);
        glm::vec3 newVert = glm::vec3(rMat * glm::vec4(perp, 1.0f));
        vertices.push_back(midPoints[midPoints.size() - 1] / maxValue + newVert * radiusScale);
    }
    // Add normals
    for (int i = 0; i <= prec + 1; i++) {
        normals.push_back(faceNormals[numFaceNormals - 1]);
    }

    // Calculate the indices
    for (int i = 0; i <= prec + 1; i++) {
        startIndices.push_back(i);
        endIndices.push_back(i + prec + 2);
    }

    // Face texture coord
    texCoords.push_back(glm::vec2(0.5f, 0.5f));
    for (int i = 0; i <= prec; i++) {
        texCoords.push_back(glm::vec2(0.5f + 0.5f * cos(toRadians(i * 360.0f / prec)), 0.5f + 0.5f * sin(toRadians(i * 360.0f / prec))));
    }
    texCoords.push_back(glm::vec2(0.5f, 0.5f));
    for (int i = 0; i <= prec; i++) {
        texCoords.push_back(glm::vec2(0.5f + 0.5f * cos(toRadians(i * 360.0f / prec)), 0.5f + 0.5f * sin(toRadians(i * 360.0f / prec))));
    }



    // Calculate Pipe vertices
    for (int i = 0; i < midPoints.size(); i++) {
        glm::vec3 perp = glm::normalize(glm::cross(faceNormals[i], glm::vec3(1.0f, 0.0f, 0.0f)));
        // cout << "Perpendicular vector: " << perp.x << " " << perp.y << " " << perp.z << endl;
        if (faceNormals[i].y == 0.0f && faceNormals[i].z == 0.0f) {
            perp = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        float texCoordX = distances[i] / totalDistance;
        float texCoordNext = i < midPoints.size() - 1 ? distances[i + 1] / totalDistance : 1.0f;

        for (int j = 0; j <= prec; j++) {
            float amt = toRadians(j * 360.0f / prec);
            if (faceNormals[i].y < 0.0f) {
                amt = -amt;
            }
            glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, faceNormals[i]);
            glm::vec3 newVert = glm::vec3(rMat * glm::vec4(perp, 1.0f));
            vertices.push_back(midPoints[i] / maxValue + newVert * radiusScale);
            normals.push_back(newVert);
            normals.push_back(newVert);

            if (i < midPoints.size() - 1) {
                vertices.push_back(midPoints[i + 1] / maxValue + newVert * radiusScale);
            } else {
                vertices.push_back(midPoints[i] / maxValue + newVert * radiusScale);
            }

            texCoords.push_back(glm::vec2(texCoordX, (float)j / prec));
            texCoords.push_back(glm::vec2(texCoordNext, (float)j / prec));
        }
    }

    int pipeStart = 2 * (prec + 2);

    // Calculate Pipe indices
    for (int i = 0; i < midPoints.size() - 1; i++) {
        for (int j = 0; j < prec; j++) {

            pipeIndices.push_back(pipeStart + 2 * (prec + 1) * i + 2 * j);
            pipeIndices.push_back(pipeStart + 2 * (prec + 1) * i + 2 * j + 1);
            pipeIndices.push_back(pipeStart + 2 * (prec + 1) * i + 2 * j + 2);

            pipeIndices.push_back(pipeStart + 2 * (prec + 1) * i + 2 * j + 3);
            pipeIndices.push_back(pipeStart + 2 * (prec + 1) * i + 2 * j + 2);
            pipeIndices.push_back(pipeStart + 2 * (prec + 1) * i + 2 * j + 1);

            if (i < midPoints.size() - 1) {
                pipeIndices.push_back(pipeStart + 2 * (prec + 1) * i + 2 *j + 1);
                pipeIndices.push_back(pipeStart + 2 * (prec + 1) * (i + 1) + 2 * j);
                pipeIndices.push_back(pipeStart + 2 * (prec + 1) * i + 2 * j + 3);

                pipeIndices.push_back(pipeStart + 2 * (prec + 1) * i + 2 * j + 3);
                pipeIndices.push_back(pipeStart + 2 * (prec + 1) * (i + 1) + 2 * j);
                pipeIndices.push_back(pipeStart + 2 * (prec + 1) * (i + 1) + 2 * j + 2);
            }

        }
    }

    numVertices = vertices.size();
    numIndicesPipe = pipeIndices.size();
}

int Pipe::getNumVertices() {
    return numVertices;
}

int Pipe::getNumIndicesPipe() {
    return numIndicesPipe;
}

int Pipe::getNumIndicesFace() {
    return numIndicesFace;
}

std::vector<int> Pipe::getPipeIndices() {
    return pipeIndices;
}

std::vector<int> Pipe::getStartIndices() {
    return startIndices;
}

std::vector<int> Pipe::getEndIndices() {
    return endIndices;
}

std::vector<glm::vec3> Pipe::getVertices() {
    return vertices;
}

std::vector<glm::vec2> Pipe::getTexCoords() {
    return texCoords;
}

std::vector<glm::vec3> Pipe::getNormals() {
    return normals;
}
