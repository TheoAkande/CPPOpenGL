#include "program6_3.h"
#include "Utils.h"
#include "ImportedModel.h"

using namespace std;

#define numVAOs 1
#define numVBOs 3

glm::vec3 cameraPoint, cameraLoc;
float cameraRotAngle;

float cameraX, cameraY, cameraZ;
float teacupLocX, teacupLocY, teacupLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

GLuint mvLoc, projLoc;
int width, height;
float aspect;
double tf;
glm::mat4 pMat, vMat, mMat, mvMat, tMat, rMat;

ImportedModel myModel("assets/objects/formattedPot.obj");

GLuint brickTexture, iceTexture, customTexture, earthTexture;


void setupVertices(void) { 
    std::vector<glm::vec3> vert = myModel.getVertices();
    std::vector<glm::vec2> tex = myModel.getTextureCoords();
    std::vector<glm::vec3> norm = myModel.getNormals();
    int numObjVertices = myModel.getNumVertices();
    std::vector<float> pvalues; // vertex positions
    std::vector<float> tvalues; // texture coordinates
    std::vector<float> nvalues; // normal vectors
    for (int i = 0; i < numObjVertices; i++) {
        pvalues.push_back((vert[i]).x);
        pvalues.push_back((vert[i]).y);
        pvalues.push_back((vert[i]).z);
        tvalues.push_back((tex[i]).s);
        tvalues.push_back((tex[i]).t);
        nvalues.push_back((norm[i]).x);
        nvalues.push_back((norm[i]).y);
        nvalues.push_back((norm[i]).z);
    }
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);
    // VBO for vertex locations
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
    // VBO for texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
    // VBO for normal vectors
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
}

void init(GLFWwindow* window) {
    renderingProgram = Utils::createShaderProgram("shaders/vert6_3.glsl", "shaders/frag6_3.glsl");
    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 8.0f;

    cameraLoc = glm::vec3(0.0f, 0.0f, 8.0f);
    cameraPoint = glm::vec3(0.0f, 0.0f, 0.0f);
    cameraRotAngle = 0.0f;
    teacupLocX = 0.0f; teacupLocY = 0.0f; teacupLocZ = 0.0f;
    setupVertices();

    brickTexture = Utils::loadTexture("assets/textures/brick1.jpg");
    iceTexture = Utils::loadTexture("assets/textures/ice.jpg");
    customTexture = Utils::loadTexture("assets/textures/5050.jpg");
    earthTexture = Utils::loadTexture("assets/textures/earth.jpg");
}

void display(GLFWwindow* window, double currentTime) {

    cameraLoc.x = 5.0f * cos(currentTime);
    cameraLoc.z = 5.0f * sin(currentTime);
    cameraLoc.y = 2 * sin(currentTime);

    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(renderingProgram);
    // get the uniform variables for the MV and projection matrices
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    // build perspective matrix
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees
    // build view matrix, model matrix, and model-view matrix
    // the view matrix is computed once and used for both objects
    Utils::calculateVMat(&vMat, &cameraLoc, &cameraPoint, cameraRotAngle);
    // draw the cube (use buffer #0)
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(teacupLocX, teacupLocY, teacupLocZ));
    mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
    mvMat = vMat * mMat;
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, customTexture);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());

}

int program6_3(void) {
    if (!glfwInit()) { 
        exit(EXIT_FAILURE); 
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(1200, 1000, "Chapter6 - program3", NULL, NULL);
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