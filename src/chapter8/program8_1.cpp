#include "program8_1.h"
#include "Utils.h"
#include "Torus.h"
#include "ImportedModel.h"

using namespace std;

#define numVAOs 1
#define numVBOs 7

glm::vec3 cameraPoint;
float cameraRotAngle;

float cameraX, cameraY, cameraZ;
float sphereLocX, sphereLocY, sphereLocZ;
GLuint renderingProgram1, renderingProgram2;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

GLuint mvLoc, projLoc, nLoc;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mAmbLoc, mDiffLoc, mSpecLoc, mShiLoc, sLoc;
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
float* goldMatAmb = Utils::goldAmbient();
float* goldMatDif = Utils::goldDiffuse();
float* goldMatSpe = Utils::goldSpecular();
float goldMatShi = Utils::goldShininess();
// bronze material for the torus
float* bronzeMatAmb = Utils::bronzeAmbient();
float* bronzeMatDif = Utils::bronzeDiffuse();
float* bronzeMatSpe = Utils::bronzeSpecular();
float bronzeMatShi = Utils::bronzeShininess();
// variables used in display() for transfering light to shaders
float curAmb[4], curDif[4], curSpe[4], matAmb[4], matDif[4], matSpe[4];
float curShi, matShi;

// shadow-related variables
int screenSizeX, screenSizeY;
GLuint shadowTex, shadowBuffer;
glm::mat4 lightVmatrix;
glm::mat4 lightPmatrix;
glm::mat4 shadowMVP1;
glm::mat4 shadowMVP2;
glm::mat4 b;

int width, height;
float aspect;
double tf;

ImportedModel pyramid("assets/objects/pyr.obj"); // define the pyramid
Torus myTorus(0.6f, 0.4f, 48); // define the torus
int numPyramidVertices, numTorusVertices, numTorusIndices;

GLuint brickTexture, iceTexture, customTexture, earthTexture;

glm::vec3 torusLoc(1.6f, 0.0f, -0.3f);
glm::vec3 pyrLoc(-1.0f, 0.1f, 0.3f);
glm::vec3 cameraLoc(0.0f, 0.2f, 6.0f);
glm::vec3 lightLoc(-3.8f, 2.2f, 1.1f);
glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);

void setupVertices(void) { 

    // For Torus
    std::vector<int> ind = myTorus.getIndices();
    std::vector<glm::vec3> vert = myTorus.getVertices();
    std::vector<glm::vec2> tex = myTorus.getTexCoords();
    std::vector<glm::vec3> norm = myTorus.getNormals();
    std::vector<float> pvalues;
    std::vector<float> tvalues;
    std::vector<float> nvalues;

    int numTorusVertices = myTorus.getNumVertices();
    numTorusIndices = myTorus.getNumIndices();

    for (int i = 0; i < numTorusVertices; i++) {
        pvalues.push_back(vert[i].x);
        pvalues.push_back(vert[i].y);
        pvalues.push_back(vert[i].z);
        tvalues.push_back(tex[i].s);
        tvalues.push_back(tex[i].t);
        nvalues.push_back(norm[i].x);
        nvalues.push_back(norm[i].y);
        nvalues.push_back(norm[i].z);
    }

    // For Pyramid
    std::vector<glm::vec3> vertP = pyramid.getVertices();
    std::vector<glm::vec2> texP = pyramid.getTextureCoords();
    std::vector<glm::vec3> normP = pyramid.getNormals();
    std::vector<float> pvaluesP;
    std::vector<float> tvaluesP;
    std::vector<float> nvaluesP;

    numPyramidVertices = pyramid.getNumVertices();

    for (int i = 0; i < numPyramidVertices; i++) {
        pvaluesP.push_back(vertP[i].x);
        pvaluesP.push_back(vertP[i].y);
        pvaluesP.push_back(vertP[i].z);
        tvaluesP.push_back(texP[i].s);
        tvaluesP.push_back(texP[i].t);
        nvaluesP.push_back(normP[i].x);
        nvaluesP.push_back(normP[i].y);
        nvaluesP.push_back(normP[i].z);
    }

    // Bind buffers
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo); // generate VBOs as before, plus one for indices

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // torus vertex positions
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); // torus texture coordinates
    glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]); // torus normal vectors
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]); // torus indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]); // pyramid vertex positions
    glBufferData(GL_ARRAY_BUFFER, pvaluesP.size() * 4, &pvaluesP[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]); // pyramid texture coordinates
    glBufferData(GL_ARRAY_BUFFER, tvaluesP.size() * 4, &tvaluesP[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]); // pyramid normal vectors
    glBufferData(GL_ARRAY_BUFFER, nvaluesP.size() * 4, &nvaluesP[0], GL_STATIC_DRAW);
}

void installLights(GLuint renderingProgram, glm::mat4 vMatrix) {
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
    glProgramUniform4fv(renderingProgram, mAmbLoc, 1, curAmb);
    glProgramUniform4fv(renderingProgram, mDiffLoc, 1, curDif);
    glProgramUniform4fv(renderingProgram, mSpecLoc, 1, curSpe);
    glProgramUniform1f(renderingProgram, mShiLoc, curShi);
}

void setupShadowBuffers(GLFWwindow* window) {
    glfwGetFramebufferSize(window, &width, &height);
    screenSizeX = width;
    screenSizeY = height;
    // create the custom frame buffer
    glGenFramebuffers(1, &shadowBuffer);
    // create the shadow texture and configure it to hold depth information.
    // these steps are similar to those in Program 5.2
    glGenTextures(1, &shadowTex);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
    screenSizeX, screenSizeY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
    GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
}

void init(GLFWwindow* window) {
    renderingProgram1 = Utils::createShaderProgram("shaders/vert8_1_1.glsl", "shaders/frag8_1_1.glsl");
    renderingProgram2 = Utils::createShaderProgram("shaders/vert8_1_2.glsl", "shaders/frag8_1_2.glsl");
    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 8.0f;

    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees

    cameraLoc = glm::vec3(0.0f, 0.0f, 8.0f);
    cameraPoint = glm::vec3(0.0f, 0.0f, 0.0f);
    cameraRotAngle = 0.0f;
    sphereLocX = 0.0f; sphereLocY = 0.0f; sphereLocZ = 0.0f;
    setupVertices();
    setupShadowBuffers(window);
    b = glm::mat4(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f
    );

    brickTexture = Utils::loadTexture("assets/textures/brick1.jpg");
    iceTexture = Utils::loadTexture("assets/textures/ice.jpg");
    customTexture = Utils::loadTexture("assets/textures/5050.jpg");
    earthTexture = Utils::loadTexture("assets/textures/earth.jpg");
}

float toRadians(float degrees) {
    return degrees * (3.141592f / 180.0f);
}

void passOne(void) {
    // renderingProgram1 includes the pass one vertex and fragment shaders
    glUseProgram(renderingProgram1);
    // the following blocks of code render the torus to establish the depth buffer from the light point of view
    mMat = glm::translate(glm::mat4(1.0f), torusLoc);
    // slight rotation for viewability
    mMat = glm::rotate(mMat, toRadians(25.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // we are drawing from the light’s point of view, so we use the light’s P and V matrices
    shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
    sLoc = glGetUniformLocation(renderingProgram1, "shadowMVP");
    glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));
    // we only need to set up torus vertices buffer – we don’t need its textures or normals for pass one.
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glClear(GL_DEPTH_BUFFER_BIT);
    // glEnable(GL_CULL_FACE);
    // glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]); // vbo[3] contains torus indices
    glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0);
    // repeat for the pyramid (but don’t clear the GL_DEPTH_BUFFER_BIT)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    // glCullFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // The pyramid is not indexed, so we use glDrawArrays() rather than glDrawElements()
    glDrawArrays(GL_TRIANGLES, 0, numPyramidVertices);
}

void passTwo(void) {
    glUseProgram(renderingProgram2); // pass two vertex and fragment shaders
    // draw the torus – this time we need to include lighting, materials, normals, etc.
    // We also need to provide MVP tranforms for BOTH camera space and light space.
    mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
    nLoc = glGetUniformLocation(renderingProgram2, "norm_matrix");
    sLoc = glGetUniformLocation(renderingProgram2, "shadowMVP");
    // the torus is bronze
    curAmb[0] = bronzeMatAmb[0]; curAmb[1] = bronzeMatAmb[1]; curAmb[2] = bronzeMatAmb[2];
    curDif[0] = bronzeMatDif[0]; curDif[1] = bronzeMatDif[1]; curDif[2] = bronzeMatDif[2];
    curSpe[0] = bronzeMatSpe[0]; curSpe[1] = bronzeMatSpe[1]; curSpe[2] = bronzeMatSpe[2];
    curShi = bronzeMatShi;
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraLoc.x, -cameraLoc.y, -cameraLoc.z));
    currentLightPos = glm::vec3(lightLoc);
    installLights(renderingProgram2, vMat);
    mMat = glm::translate(glm::mat4(1.0f), torusLoc);
    // slight rotation for viewability
    mMat = glm::rotate(mMat, toRadians(25.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // build the MV matrix for the torus from the camera’s point of view
    mvMat = vMat * mMat;
    invTrMat = glm::transpose(glm::inverse(mvMat));
    // build the MVP matrix for the torus from the light’s point of view
    shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;
    // put the MV and PROJ matrices into the corresponding uniforms
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
    glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));
    // set up torus vertices and normals buffers (and texture coordinates buffer if used)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // torus vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]); // torus normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glClear(GL_DEPTH_BUFFER_BIT);
    // glEnable(GL_CULL_FACE);
    // glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]); // vbo[3] contains torus indices
    glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0);
    
    // the torus is bronze
    curAmb[0] = goldMatAmb[0]; curAmb[1] = goldMatAmb[1]; curAmb[2] = goldMatAmb[2];
    curDif[0] = goldMatDif[0]; curDif[1] = goldMatDif[1]; curDif[2] = goldMatDif[2];
    curSpe[0] = goldMatSpe[0]; curSpe[1] = goldMatSpe[1]; curSpe[2] = goldMatSpe[2];
    curShi = goldMatShi;
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraLoc.x, -cameraLoc.y, -cameraLoc.z));
    currentLightPos = glm::vec3(lightLoc);
    installLights(renderingProgram2, vMat);
    mMat = glm::translate(glm::mat4(1.0f), pyrLoc);
    // slight rotation for viewability
    mMat = glm::rotate(mMat, toRadians(-25.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // build the MV matrix for the torus from the camera’s point of view
    mvMat = vMat * mMat;
    invTrMat = glm::transpose(glm::inverse(mvMat));
    // build the MVP matrix for the torus from the light’s point of view
    shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;
    // put the MV and PROJ matrices into the corresponding uniforms
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
    glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));
    // set up torus vertices and normals buffers (and texture coordinates buffer if used)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]); // pyr vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]); // pyr normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    glClear(GL_DEPTH_BUFFER_BIT);
    // glEnable(GL_CULL_FACE);
    // glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, numPyramidVertices);
}

void display(GLFWwindow* window, double currentTime) {

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    // set up view and perspective matrix from the light point of view, for pass 1
    lightVmatrix = glm::lookAt(currentLightPos, origin, up); // vector from light to origin
    lightPmatrix = glm::perspective(toRadians(60.0f), aspect, 0.1f, 1000.0f);
    // make the custom frame buffer current, and associate it with the shadow texture
    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTex, 0);
    // disable drawing colors, but enable the depth computation
    glDrawBuffer(GL_FRONT);
    glEnable(GL_DEPTH_TEST);
    passOne();
    // restore the default display buffer, and re-enable drawing
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glDrawBuffer(GL_FRONT); // re-enables drawing colors
    passTwo();
}

int program8_1(void) {

    if (!glfwInit()) { 
        exit(EXIT_FAILURE); 
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(1200, 1000, "Chapter8 - program1", NULL, NULL);
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