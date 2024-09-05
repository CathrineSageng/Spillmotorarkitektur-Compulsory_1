#include<iostream>
#include "glm/mat4x3.hpp"

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<vector>

#include "Shader.h"
#include "ShaderFileLoader.h"
#include "Camera.h"
#include "Box.h"
#include "Ball.h"
using namespace std;

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

glm::vec3 ball01(0.3f, -0.19f, 0.3f);
glm::vec3 ball02(0.1f, -0.19f, 0.3f);
glm::vec3 ball03(-0.1f, -0.19f, 0.3f);
glm::vec3 ball04(-0.3f, -0.19f, 0.3f);
glm::vec3 ball05(0.3f, -0.19f, 0.1f);
glm::vec3 ball06(0.1f, -0.19f, 0.1f);
glm::vec3 ball07(-0.1f, -0.19f, 0.1f);
glm::vec3 ball08(-0.3f, -0.19f, 0.1f);
glm::vec3 ball09(0.3f, -0.19f, -0.1f);
glm::vec3 ball010(0.1f, -0.19f, -0.1f);
glm::vec3 ball011(-0.1f, -0.19f, -0.1f);
glm::vec3 ball012(-0.3f, -0.19f, -0.1f);
glm::vec3 ball013(0.3f, -0.19f, -0.3f);
glm::vec3 ball014(0.1f, -0.19f, -0.3f);
glm::vec3 ball015(-0.1f, -0.19f, -0.3f);
glm::vec3 ball016(-0.3f, -0.19f, -0.3f);

glm::vec3 ballVelocity01(0.5f, 0.0f, 0.5f);
glm::vec3 ballVelocity02(-0.5f, 0.0f, 0.4f);
glm::vec3 ballVelocity03(0.4f, 0.0f, -0.3f);
glm::vec3 ballVelocity04(0.6f, 0.0f, -0.7f);
glm::vec3 ballVelocity05(0.4f, 0.0f, 0.8f);
glm::vec3 ballVelocity06(0.4f, 0.0f, -0.3f);
glm::vec3 ballVelocity07(0.6f, 0.0f, -0.3f);
glm::vec3 ballVelocity08(0.4f, 0.0f, 0.8f);
glm::vec3 ballVelocity09(0.4f, 0.0f, -0.3f);
glm::vec3 ballVelocity10(0.9f, 0.0f, 0.3f);
glm::vec3 ballVelocity11(0.4f, 0.0f, -0.3f);
glm::vec3 ballVelocity12(-0.1f, 0.0f, -0.3f);
glm::vec3 ballVelocity13(0.4f, 0.0f, -0.3f);
glm::vec3 ballVelocity14(-0.2f, 0.0f, 0.3f);
glm::vec3 ballVelocity15(0.4f, 0.0f, -0.3f);
glm::vec3 ballVelocity16(-0.4f, 0.0f, 0.3f);
// Add velocities for other balls as needed

float boxMin = -0.46f;
float boxMax = 0.46f;

bool ballsMoving = false;

std::vector<glm::vec3> ballPositions = {
    ball01, ball02, ball03, ball04, ball05, ball06, ball07, ball08,
    ball09, ball010, ball011, ball012, ball013, ball014, ball015, ball016
};

std::vector<glm::vec3> ballVelocities(ballPositions.size(), glm::vec3(0.0f));  // Start with zero velocities

float ballRadius = 0.05f;  // Assuming all balls have the same radius

bool checkCollision(glm::vec3 posA, glm::vec3 posB, float radiusA, float radiusB)
{
    float distance = glm::distance(posA, posB);
    return distance < (radiusA + radiusB);
}

void resolveCollision(glm::vec3& posA, glm::vec3& velA, glm::vec3& posB, glm::vec3& velB, float radius, float massA, float massB)
{
    glm::vec3 collisionNormal = glm::normalize(posB - posA);
    glm::vec3 relativeVelocity = velA - velB;

    // Calculate velocity along the normal
    float velocityAlongNormal = glm::dot(relativeVelocity, collisionNormal);

    // Do not resolve if velocities are separating
    if (velocityAlongNormal > 0)
        return;

    // Calculate restitution (elastic collision)
    float e = 1.0f;  // Elastic collision (no energy loss)

    // Impulse scalar
    float j = -(1 + e) * velocityAlongNormal / (1 / massA + 1 / massB);

    // Apply impulse
    glm::vec3 impulse = j * collisionNormal;
    velA -= impulse / massA;
    velB += impulse / massB;
}

void updateBallCollisions(std::vector<glm::vec3>& ballPositions, std::vector<glm::vec3>& ballVelocities, float radius)
{
    // Loop over all pairs of balls to check for collisions
    for (int i = 0; i < ballPositions.size(); ++i)
    {
        for (int j = i + 1; j < ballPositions.size(); ++j)
        {
            if (checkCollision(ballPositions[i], ballPositions[j], radius, radius))
            {
                // If a collision is detected, resolve it
                resolveCollision(ballPositions[i], ballVelocities[i], ballPositions[j], ballVelocities[j], radius, 1.0f, 1.0f); // Assume equal mass (1.0f)
            }
        }
    }
}

// Camera settings
//This is the starting position of the of the camera 
Camera camera(glm::vec3(0.0f, 1.0f, 2.0f));
//Keeps track of the last position of the mouse cursor 
GLfloat lastX = SCR_WIDTH / 2.0f;
GLfloat lastY = SCR_HEIGHT / 2.0f;
//Avoids sudden jumps in the camera orientation when the mouse is first detected. 
bool firstMouse = true;

// Time between current frame and last frame
float deltaTime = 0.0f;
//Stores the timestamp of previous frame. 
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);



std::string vfs = ShaderLoader::LoadShaderFromFile("vs.vs");
std::string fs = ShaderLoader::LoadShaderFromFile("fs.fs");


int main()
{
    std::cout << "vfs " << vfs.c_str() << std::endl;
    std::cout << "fs " << fs.c_str() << std::endl;
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Compulsory 1", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("vs.vs", "fs.fs"); // you can name your shader files however you like

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    //The Box
    Box box;
    box.loadBox("Box.txt");

    //Balls
    Ball ball1(0.05f, 30, 30);
    Ball ball2(0.05f, 30, 30);
    Ball ball3(0.05f, 30, 30);
    Ball ball4(0.05f, 30, 30);
    Ball ball5(0.05f, 30, 30);
    Ball ball6(0.05f, 30, 30);
    Ball ball7(0.05f, 30, 30);
    Ball ball8(0.05f, 30, 30);
    Ball ball9(0.05f, 30, 30);
    Ball ball10(0.05f, 30, 30);
    Ball ball11(0.05f, 30, 30);
    Ball ball12(0.05f, 30, 30);
    Ball ball13(0.05f, 30, 30);
    Ball ball14(0.05f, 30, 30);
    Ball ball15(0.05f, 30, 30);
    Ball ball16(0.05f, 30, 30);

while (!glfwWindowShouldClose(window))
{
    // Time calculation for movement
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ourShader.use();

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    ourShader.setMat4("projection", projection);
    ourShader.setMat4("view", view);

    // World transformation for the box
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.2f, 0.0f)); // Translate the box upwards
    ourShader.setMat4("model", model);

    glBindVertexArray(box.getVAO());
    glDrawElements(GL_TRIANGLES, box.getIndexCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    if (ballsMoving)
    {
        // Update ball positions with velocities
        for (int i = 0; i < ballPositions.size(); ++i)
        {
            ballPositions[i] += ballVelocities[i] * deltaTime;
        }

        // Check for ball-ball collisions and resolve
        updateBallCollisions(ballPositions, ballVelocities, ballRadius);

        // Ball Movement
        ball01 += ballVelocity01 * deltaTime;
        ball02 += ballVelocity02 * deltaTime;
        ball03 += ballVelocity03 * deltaTime;
        ball04 += ballVelocity04 * deltaTime;
        ball05 += ballVelocity05 * deltaTime;
        ball06 += ballVelocity06 * deltaTime;
        ball07 += ballVelocity07 * deltaTime;
        ball08 += ballVelocity08 * deltaTime;
        ball09 += ballVelocity09 * deltaTime;
        ball010 += ballVelocity10 * deltaTime;
        ball011 += ballVelocity11 * deltaTime;
        ball012 += ballVelocity12 * deltaTime;
        ball013 += ballVelocity13 * deltaTime;
        ball014 += ballVelocity14 * deltaTime;
        ball015 += ballVelocity15 * deltaTime;
        ball016 += ballVelocity16 * deltaTime;

        // Update other ball positions based on velocity

        // Ball Boundary Checking and Reversing Velocity
        if (ball01.x <= boxMin || ball01.x >= boxMax) ballVelocity01.x = -ballVelocity01.x;
        if (ball01.z <= boxMin || ball01.z >= boxMax) ballVelocity01.z = -ballVelocity01.z;

        if (ball02.x <= boxMin || ball02.x >= boxMax) ballVelocity02.x = -ballVelocity02.x;
        if (ball02.z <= boxMin || ball02.z >= boxMax) ballVelocity02.z = -ballVelocity02.z;

        if (ball03.x <= boxMin || ball03.x >= boxMax) ballVelocity03.x = -ballVelocity03.x;
        if (ball03.z <= boxMin || ball03.z >= boxMax) ballVelocity03.z = -ballVelocity03.z;

        if (ball04.x <= boxMin || ball04.x >= boxMax) ballVelocity04.x = -ballVelocity04.x;
        if (ball04.z <= boxMin || ball04.z >= boxMax) ballVelocity04.z = -ballVelocity04.z;

        if (ball05.x <= boxMin || ball05.x >= boxMax) ballVelocity05.x = -ballVelocity05.x;
        if (ball05.z <= boxMin || ball05.z >= boxMax) ballVelocity05.z = -ballVelocity05.z;

        if (ball06.x <= boxMin || ball06.x >= boxMax) ballVelocity06.x = -ballVelocity06.x;
        if (ball06.z <= boxMin || ball06.z >= boxMax) ballVelocity06.z = -ballVelocity06.z;

        if (ball07.x <= boxMin || ball07.x >= boxMax) ballVelocity07.x = -ballVelocity07.x;
        if (ball07.z <= boxMin || ball07.z >= boxMax) ballVelocity07.z = -ballVelocity07.z;

        if (ball08.x <= boxMin || ball08.x >= boxMax) ballVelocity08.x = -ballVelocity08.x;
        if (ball08.z <= boxMin || ball08.z >= boxMax) ballVelocity08.z = -ballVelocity08.z;

        if (ball09.x <= boxMin || ball09.x >= boxMax) ballVelocity09.x = -ballVelocity09.x;
        if (ball09.z <= boxMin || ball09.z >= boxMax) ballVelocity09.z = -ballVelocity09.z;

        if (ball010.x <= boxMin || ball010.x >= boxMax) ballVelocity10.x = -ballVelocity10.x;
        if (ball010.z <= boxMin || ball010.z >= boxMax) ballVelocity10.z = -ballVelocity10.z;

        if (ball011.x <= boxMin || ball011.x >= boxMax) ballVelocity11.x = -ballVelocity11.x;
        if (ball011.z <= boxMin || ball011.z >= boxMax) ballVelocity11.z = -ballVelocity11.z;

        if (ball012.x <= boxMin || ball012.x >= boxMax) ballVelocity12.x = -ballVelocity12.x;
        if (ball012.z <= boxMin || ball012.z >= boxMax) ballVelocity12.z = -ballVelocity12.z;

        if (ball013.x <= boxMin || ball013.x >= boxMax) ballVelocity13.x = -ballVelocity13.x;
        if (ball013.z <= boxMin || ball013.z >= boxMax) ballVelocity13.z = -ballVelocity13.z;

        if (ball014.x <= boxMin || ball014.x >= boxMax) ballVelocity14.x = -ballVelocity14.x;
        if (ball014.z <= boxMin || ball014.z >= boxMax) ballVelocity14.z = -ballVelocity14.z;

        if (ball015.x <= boxMin || ball015.x >= boxMax) ballVelocity15.x = -ballVelocity15.x;
        if (ball015.z <= boxMin || ball015.z >= boxMax) ballVelocity15.z = -ballVelocity15.z;

        if (ball016.x <= boxMin || ball016.x >= boxMax) ballVelocity16.x = -ballVelocity16.x;
        if (ball016.z <= boxMin || ball016.z >= boxMax) ballVelocity16.z = -ballVelocity16.z;

    }

    // Ball1
    model = glm::mat4(1.0f);
    model = glm::translate(model, ball01);
    ourShader.setMat4("model", model);
    ball1.DrawBall();

    // Ball2
    model = glm::mat4(1.0f);
    model = glm::translate(model, ball02);
    ourShader.setMat4("model", model);
    ball2.DrawBall();

    // Ball3
    model = glm::mat4(1.0f);
    model = glm::translate(model, ball03);
    ourShader.setMat4("model", model);
    ball3.DrawBall();

    //Ball4
    model = glm::mat4(1.0f);
    model = glm::translate(model, ball04);
    ourShader.setMat4("model", model);
    ball4.DrawBall();

    //Ball5
    model = glm::mat4(1.0f);
    model = glm::translate(model, ball05);
    ourShader.setMat4("model", model);
    ball5.DrawBall();

    //Ball6
    model = glm::mat4(1.0f);
    model = glm::translate(model, ball06);
    ourShader.setMat4("model", model);
    ball6.DrawBall();

    //Ball7
    model = glm::mat4(1.0f);
    model = glm::translate(model, ball07);
    ourShader.setMat4("model", model);
    ball7.DrawBall();

    //Ball8
    model = glm::mat4(1.0f);
    model = glm::translate(model, ball08);
    ourShader.setMat4("model", model);
    ball8.DrawBall();

    //Ball9
    model = glm::mat4(1.0f);
    model = glm::translate(model, ball09);
    ourShader.setMat4("model", model);
    ball9.DrawBall();

    //Ball10
    model = glm::mat4(1.0f);
    model = glm::translate(model, ball010);
    ourShader.setMat4("model", model);
    ball10.DrawBall();

    //Ball11
    model = glm::mat4(1.0f);
    model = glm::translate(model, ball011);
    ourShader.setMat4("model", model);
    ball11.DrawBall();

    //Ball12
    model = glm::mat4(1.0f);
    model = glm::translate(model, ball012);
    ourShader.setMat4("model", model);
    ball12.DrawBall();

    //Ball13
    model = glm::mat4(1.0f);
    model = glm::translate(model, ball013);
    ourShader.setMat4("model", model);
    ball13.DrawBall();

    //Ball14
    model = glm::mat4(1.0f);
    model = glm::translate(model, ball014);
    ourShader.setMat4("model", model);
    ball14.DrawBall();

    //Ball15
    model = glm::mat4(1.0f);
    model = glm::translate(model, ball015);
    ourShader.setMat4("model", model);
    ball15.DrawBall();

    //Ball16
    model = glm::mat4(1.0f);
    model = glm::translate(model, ball016);
    ourShader.setMat4("model", model);
    ball16.DrawBall();


    glfwSwapBuffers(window);
    glfwPollEvents();
}
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // Check if the space key is pressed to start ball movement
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        ballsMoving = true;  // Start moving the balls when space is pressed
    }
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

