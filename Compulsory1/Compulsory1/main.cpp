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
#include "Octree.h"

using namespace std;

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;
float boxMin = -0.45f;
float boxMax = 0.45f;
float ballSpeed = 0.5f;

bool ballsMoving = false;

float ballRadius = 0.05f;

glm::vec3 ball1(0.3f, -0.19f, 0.3f);
glm::vec3 ball2(0.1f, -0.19f, 0.3f);
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

std::vector<glm::vec3> ballPositions = {
    ball1, ball2, ball03, ball04, ball05, ball06, ball07, ball08,
    ball09, ball010, ball011, ball012, ball013, ball014, ball015, ball016
};

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

std::vector<glm::vec3> ballVelocities = {
    ballVelocity01, ballVelocity02, ballVelocity03, ballVelocity04, ballVelocity05,
    ballVelocity06, ballVelocity07, ballVelocity08,
    ballVelocity09, ballVelocity10, ballVelocity11, ballVelocity12, ballVelocity13,
    ballVelocity14, ballVelocity15, ballVelocity16
};

std::vector<glm::vec3> ballColors = {
    glm::vec3(1.0f, 0.0f, 0.0f), // Red
    glm::vec3(0.0f, 1.0f, 0.0f), // Green
    glm::vec3(0.0f, 0.0f, 1.0f), // Blue
    glm::vec3(1.0f, 1.0f, 0.0f), // Yellow
    glm::vec3(1.0f, 0.0f, 1.0f), // Magenta
    glm::vec3(0.0f, 1.0f, 1.0f), // Cyan
    glm::vec3(0.5f, 0.5f, 0.5f), // Grey
    glm::vec3(1.0f, 0.5f, 0.0f), // Orange
    glm::vec3(0.5f, 0.0f, 0.5f), // Purple
    glm::vec3(0.0f, 0.5f, 0.5f),  // Teal
    glm::vec3(0.0f, 0.0f, 0.0f)  // Black
};

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
bool checkCollision(glm::vec3 posA, glm::vec3 posB, float radiusA, float radiusB);
void resolveCollision(glm::vec3& p1, glm::vec3& v1, glm::vec3& p2, glm::vec3& v2, float radius);

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

    Ball ball(0.05f, 30, 30, glm::vec3(1.0f, 0.41f, 0.71f));  // Hot Pink

    std::vector<Ball> balls;
    for (int i = 0; i < ballPositions.size(); ++i)
    {
        glm::vec3 ballColor = ballColors[i % ballColors.size()]; // Assign a color from the color list
        balls.push_back(Ball(ballRadius, 30, 30, ballColor));    // Create the ball with the color
    }

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

        // If the balls are moving, update their positions and detect collisions
        if (ballsMoving)
        {
            // Step 1: Create the octree to manage the balls within the box boundaries
            Octree octree(glm::vec3(boxMin, boxMin, boxMin), glm::vec3(boxMax, boxMax, boxMax), 0, 4, 4);

            // Step 2: Update ball positions and insert each ball into the octree
            for (int i = 0; i < ballPositions.size(); ++i)
            {
                // Update position with current velocity
                ballPositions[i] += ballVelocities[i] * deltaTime;

                // Ball Boundary Checking and Reversing Velocity
                if (ballPositions[i].x - ballRadius <= boxMin || ballPositions[i].x + ballRadius >= boxMax) {
                    ballVelocities[i].x = -ballVelocities[i].x;
                    ballPositions[i].x = glm::clamp(ballPositions[i].x, boxMin + ballRadius, boxMax - ballRadius);
                }
                if (ballPositions[i].z - ballRadius <= boxMin || ballPositions[i].z + ballRadius >= boxMax) {
                    ballVelocities[i].z = -ballVelocities[i].z;
                    ballPositions[i].z = glm::clamp(ballPositions[i].z, boxMin + ballRadius, boxMax - ballRadius);
                }

                // Normalize the velocity and maintain constant speed
                ballVelocities[i] = glm::normalize(ballVelocities[i]) * ballSpeed;

                // Insert the ball into the octree
                octree.insert(i, ballPositions, ballRadius);
            }

            // Step 3: Get all potential collision pairs from the octree
            std::vector<std::pair<int, int>> potentialCollisions;
            octree.getPotentialCollisions(potentialCollisions, ballPositions, ballRadius);

            // Step 4: Check for actual collisions between the potential pairs and resolve them
            for (const auto& pair : potentialCollisions) {
                int i = pair.first;
                int j = pair.second;

                if (checkCollision(ballPositions[i], ballPositions[j], ballRadius, ballRadius)) {
                    resolveCollision(ballPositions[i], ballVelocities[i], ballPositions[j], ballVelocities[j], ballRadius);
                }
            }
        }

        // Render all balls
        for (int i = 0; i < ballPositions.size(); ++i)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, ballPositions[i]);
            ourShader.setMat4("model", model);
            balls[i].DrawBall();
        }

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

bool checkCollision(glm::vec3 posA, glm::vec3 posB, float radiusA, float radiusB)
{
    float distance = glm::distance(posA, posB);
    return distance < (radiusA + radiusB);
}

void resolveCollision(glm::vec3& p1, glm::vec3& v1, glm::vec3& p2, glm::vec3& v2, float radius)
{
    // Mass of the balls (assuming equal mass)
    float m1 = 1.0f;
    float m2 = 1.0f;

    // Normal vector
    glm::vec3 normal = p1 - p2;
    float distance = glm::length(normal);
    normal = glm::normalize(normal);

    // Ensure there's no overlap
    float overlap = radius * 2 - distance;
    if (overlap > 0)
    {
        glm::vec3 correction = normal * (overlap / 2.0f);
        p1 += correction;
        p2 -= correction;
    }

    // Relative velocity
    glm::vec3 relativeVelocity = v1 - v2;

    // Calculate velocity along the normal
    float velocityAlongNormal = glm::dot(relativeVelocity, normal);

    // If the balls are moving apart, skip
    if (velocityAlongNormal > 0)
        return;

    // Collision resolution using impulse
    float e = 1.0f;  // Coefficient of restitution (elastic collision)
    float j = -(1 + e) * velocityAlongNormal / (1 / m1 + 1 / m2);

    // Apply the impulse to the velocities
    glm::vec3 impulse = j * normal;
    v1 += impulse / m1;
    v2 -= impulse / m2;
}


