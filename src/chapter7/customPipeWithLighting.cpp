#include "customPipeWithLighting.h"
#include "Utils.h"
#include "Pipe.h"

using namespace std;

#define numVAOs 1
#define numVBOs 6

glm::vec3 cameraPoint, cameraLoc;
float cameraRotAngle;

float cameraX, cameraY, cameraZ;
float sphereLocX, sphereLocY, sphereLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

GLuint mvLoc, projLoc, nLoc;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mAmbLoc, mDiffLoc, mSpecLoc, mShiLoc;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat;
glm::vec3 currentLightPos, lightPosV; // light position as Vector3f, in both model and view space
float lightPos[3]; // light position as float array
// initial light location
glm::vec3 initialLightLoc = glm::vec3(5.0f, 2.0f, 2.0f);
// white light properties
float globalAmbient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
// gold material properties
float* matAmb = Utils::goldAmbient();
float* matDif = Utils::goldDiffuse();
float* matSpe = Utils::goldSpecular();
float matShi = Utils::goldShininess();

int width, height;
float aspect;
double tf;

Pipe myPipe(vector<glm::vec3>( {
    glm::vec3(-5.0f, 0.0f, 0.0f),
    glm::vec3(-4.0f, 1.0f, 0.0f),
    glm::vec3(-3.0f, 0.0f, 0.0f)
}), 25, 0.5f);

GLuint brickTexture, iceTexture, customTexture, earthTexture;


void setupVertices(void) { 

    std::vector<glm::vec3> vert = myPipe.getVertices();
    std::vector<glm::vec2> tex = myPipe.getTexCoords();
    std::vector<glm::vec3> norm = myPipe.getNormals();
    std::vector<int> pipeInd = myPipe.getPipeIndices();
    std::vector<int> startInd = myPipe.getStartIndices();
    std::vector<int> endInd = myPipe.getEndIndices();

    std::vector<float> pvalues; // vertex positions
    std::vector<float> tvalues; // texture coordinates
    std::vector<float> nvalues; // normal vectors

    int numVertices = myPipe.getNumVertices();

    for (int i = 0; i < numVertices; i++) {
        pvalues.push_back(vert[i].x);
        pvalues.push_back(vert[i].y);
        pvalues.push_back(vert[i].z);
        tvalues.push_back(tex[i].s);
        tvalues.push_back(tex[i].t);
        nvalues.push_back(norm[i].x);
        nvalues.push_back(norm[i].y);
        nvalues.push_back(norm[i].z);
    }

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo); // generate VBOs as before, plus one for indices

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // vertex positions
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); // texture coordinates
    glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]); // normal vectors
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]); // pipe indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, pipeInd.size() * 4, &pipeInd[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[4]); // start indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, startInd.size() * 4, &startInd[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[5]); // end indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, endInd.size() * 4, &endInd[0], GL_STATIC_DRAW);
}

void installLights(glm::mat4 vMatrix) {
    // convert light’s position to view space, and save it in a float array
    lightPosV = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
    lightPos[0] = lightPosV.x;
    lightPos[1] = lightPosV.y;
    lightPos[2] = lightPosV.z;
    // get the locations of the light and material fields in the shader
    globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
    ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
    diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
    specLoc = glGetUniformLocation(renderingProgram, "light.specular");
    posLoc = glGetUniformLocation(renderingProgram, "light.position");
    mAmbLoc = glGetUniformLocation(renderingProgram, "material.ambient");
    mDiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
    mSpecLoc = glGetUniformLocation(renderingProgram, "material.specular");
    mShiLoc = glGetUniformLocation(renderingProgram, "material.shininess");
    // set the uniform light and material values in the shader
    glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
    glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
    glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
    glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
    glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);
    glProgramUniform4fv(renderingProgram, mAmbLoc, 1, matAmb);
    glProgramUniform4fv(renderingProgram, mDiffLoc, 1, matDif);
    glProgramUniform4fv(renderingProgram, mSpecLoc, 1, matSpe);
    glProgramUniform1f(renderingProgram, mShiLoc, matShi);
}

void init(GLFWwindow* window) {
    std::vector<glm::vec3> midPoints;
    for (int i = 200; i > 0; i--) {
        midPoints.push_back(glm::vec3(-(float)i / 10.0f, sin(i / 10.0f), 0.0f));
    }
    for (int i = 0; i < 200; i++) {
        midPoints.push_back(glm::vec3((float)i / 10.0f, 0.0f, sin(i / 10.0f)));
    }
    myPipe = Pipe(midPoints, 25, 1.0f);

    renderingProgram = Utils::createShaderProgram("shaders/vert7_3.glsl", "shaders/frag7_3.glsl");
    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 8.0f;

    cameraLoc = glm::vec3(0.0f, 0.0f, 8.0f);
    cameraPoint = glm::vec3(0.0f, 0.0f, 0.0f);
    cameraRotAngle = 0.0f;
    sphereLocX = 0.0f; sphereLocY = 0.0f; sphereLocZ = 0.0f;
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
    nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");
    // build perspective matrix
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees
    // build view matrix, model matrix, and model-view matrix
    // the view matrix is computed once and used for both objects
    Utils::calculateVMat(&vMat, &cameraLoc, &cameraPoint, cameraRotAngle);
    // draw the cube (use buffer #0)
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(sphereLocX, sphereLocY, sphereLocZ));
    mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));

    currentLightPos = glm::vec3(initialLightLoc.x, initialLightLoc.y, initialLightLoc.z);
    installLights(vMat);

    mvMat = vMat * mMat;

    // build the inverse-transpose of the MV matrix, for transforming normal vectors
    invTrMat = glm::transpose(glm::inverse(mvMat));

    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

     // Draw Pipe

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glDrawElements(GL_TRIANGLES, myPipe.getNumIndicesPipe(), GL_UNSIGNED_INT, 0);

    // Draw Start Face

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[4]);
    glDrawElements(GL_TRIANGLE_FAN, myPipe.getNumIndicesFace(), GL_UNSIGNED_INT, 0);

    //Draw end face

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[5]);
    glDrawElements(GL_TRIANGLE_FAN, myPipe.getNumIndicesFace(), GL_UNSIGNED_INT, 0);
}

int customPipe(void) {
    if (!glfwInit()) { 
        exit(EXIT_FAILURE); 
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(1200, 1000, "Custom Pipe with lighting", NULL, NULL);
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