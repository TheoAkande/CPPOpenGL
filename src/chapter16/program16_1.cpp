#include "Utils.h"
#include "program16_1.h"

using namespace std;

GLuint buffer[3];
GLuint simpleComputeShader;
int v1[] = { 10, 12, 16, 18, 50, 17 }; 
int v2[] = { 30, 14, 80, 20, 51, 12 };
int res[6]; // this is the array in which the result will be placed

void init() {
    simpleComputeShader = Utils::createShaderProgram("shaders/computeShader16_1.glsl");
    glGenBuffers(3, buffer); // note that each buffer is an SSBO (Shader Storage Buffer) of size 6
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 6 * sizeof(int), v1, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 6 * sizeof(int), v2, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer[2]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 6 * sizeof(int), NULL, GL_STATIC_READ);
}

void computeSum() {
    glUseProgram(simpleComputeShader);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer[0]); // first input matrix
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffer[1]); // second input matrix
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buffer[2]); // buffer to hold output matrix
    glDispatchCompute(6, 1, 1); // invokes the compute shader 6 times – the invocations can run in parallel
    glMemoryBarrier(GL_ALL_BARRIER_BITS); // ensure the compute shader finishes before proceeding
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer[2]); // retrieve the result buffer into array 'res'
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(res), res);
}

int program16_1(void) {
    if (!glfwInit()) { 
        exit(EXIT_FAILURE); 
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(1200, 1000, "Chapter16 - program1", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) { 
        exit(EXIT_FAILURE); 
    }
    init();
    computeSum(); // since we don't use the GL window, we call this "computeSum" instead of "display"
    // display the input matrices, and the computed output matrix retrieved from the output SSBO
    std::cout << v1[0] << " " << v1[1] << " " << v1[2] << " " << v1[3] << " " << v1[4] << " " << v1[5] << std::endl;
    std::cout << v2[0] << " " << v2[1] << " " << v2[2] << " " << v2[3] << " " << v2[4] << " " << v2[5] << std::endl;
    std::cout << res[0] << " " << res[1] << " " << res[2] << " " << res[3] << " " << res[4] << " " << res[5] << std::endl;

    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // GLFWwindow* window = glfwCreateWindow(1200, 1000, "Chapter6 - program3", NULL, NULL);
    // glfwMakeContextCurrent(window);
    // if (glewInit() != GLEW_OK) { 
    //     exit(EXIT_FAILURE); 
    // }
    // glfwSwapInterval(1);
    // init(window);
    // while (!glfwWindowShouldClose(window)) {
    //     display(window, glfwGetTime());
    //     glfwSwapBuffers(window);
    //     glfwPollEvents();
    // }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}