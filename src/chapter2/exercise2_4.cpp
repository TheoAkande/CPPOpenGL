#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>

#include "Utils.h"
#include "program2_6.h"

using namespace std;

#define numVAOs 1
GLuint renderingProgram;
GLuint vao[numVAOs];
float inc = 1.0f;
float x = 0.0f;
float lastTime = 0.0f;
float deltaTime;

void init(GLFWwindow* window) {
    renderingProgram = Utils::createShaderProgram("shaders/vert2_7.glsl", "shaders/frag2_7.glsl");
    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);
}

void display(GLFWwindow* window, double currentTime) {
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT); 

    glUseProgram(renderingProgram);

    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    x += inc * deltaTime;
    if (x > 1.0f) inc = -1.0f;
    if (x < -1.0f) inc = 1.0f;

    GLuint offsetLoc = glGetUniformLocation(renderingProgram, "offset");
    glProgramUniform1f(renderingProgram, offsetLoc, x);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int exercise2_4(void) {
    if (!glfwInit()) { 
        exit(EXIT_FAILURE); 
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter2 - exercise4", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) { 
        exit(EXIT_FAILURE); 
    }
    glfwSwapInterval(1);
    init(window);
    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}