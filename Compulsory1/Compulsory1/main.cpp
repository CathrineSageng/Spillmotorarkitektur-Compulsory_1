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

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

using namespace std;

// Global variables
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

float boxMin = -0.5f;
float boxMax = 0.5f;

float ballSpeed = 0.5f;

bool ballsMoving = false;

float ballRadius = 0.05f;

//Position of the lightning (texture)
glm::vec3 sunPos(0.0f, 2.0f, 0.0f);

//Position of th 16 balls in the box 
glm::vec3 ball1(0.3f, -0.45f, 0.3f);
glm::vec3 ball2(0.1f, -0.45f, 0.3f);
glm::vec3 ball03(-0.1f, -0.45f, 0.3f);
glm::vec3 ball04(-0.3f, -0.45f, 0.3f);
glm::vec3 ball05(0.3f, -0.45f, 0.1f);
glm::vec3 ball06(0.1f, -0.45f, 0.1f);
glm::vec3 ball07(-0.1f, -0.45f, 0.1f);
glm::vec3 ball08(-0.3f, -0.45f, 0.1f);
glm::vec3 ball09(0.3f, -0.45f, -0.1f);
glm::vec3 ball010(0.1f, -0.45f, -0.1f);
glm::vec3 ball011(-0.1f, -0.45f, -0.1f);
glm::vec3 ball012(-0.3f, -0.45f, -0.1f);
glm::vec3 ball013(0.3f, -0.45f, -0.3f);
glm::vec3 ball014(0.1f, -0.45f, -0.3f);
glm::vec3 ball015(-0.1f, -0.45f, -0.3f);
glm::vec3 ball016(-0.3f, -0.45f, -0.3f);

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

vector<glm::vec3> ballVelocities = {
    ballVelocity01, ballVelocity02, ballVelocity03, ballVelocity04, ballVelocity05,
    ballVelocity06, ballVelocity07, ballVelocity08,
    ballVelocity09, ballVelocity10, ballVelocity11, ballVelocity12, ballVelocity13,
    ballVelocity14, ballVelocity15, ballVelocity16
};

vector<glm::vec3> ballColors =
{
    glm::vec3(1.0f, 1.0f, 1.0f), // White
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
void whenCollisionHappens(glm::vec3& p1, glm::vec3& v1, glm::vec3& p2, glm::vec3& v2, float radius);
unsigned int loadTexture(const char* path);

std::string vfs = ShaderLoader::LoadShaderFromFile("vs.vs");
std::string fs = ShaderLoader::LoadShaderFromFile("fs.fs");

std::string vfsT = ShaderLoader::LoadShaderFromFile("Texture.vs");
std::string fsT = ShaderLoader::LoadShaderFromFile("Texture.fs");

int main()
{
    std::cout << "vfs " << vfs.c_str() << std::endl;
    std::cout << "fs " << fs.c_str() << std::endl;

    std::cout << "vfsT " << vfsT.c_str() << std::endl;
    std::cout << "fsT " << fsT.c_str() << std::endl;

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
    Shader textureShader("Texture.vs", "Texture.fs");

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    //The Box
    Box box;
    box.loadBox("Box.txt");

    vector<Ball> balls;
    for (int i = 0; i < ballPositions.size(); ++i)
    {
        glm::vec3 ballColor = ballColors[i % ballColors.size()]; // Assign a color from the color list
        balls.push_back(Ball(ballRadius, 30, 30, ballColor));    // Create the ball with the color
    }

    //Textures
    unsigned int diffuseMap1 = loadTexture("Textures/ball.jpg");
    unsigned int specularMap = loadTexture("Textures/ball2.jpg");

    unsigned int diffuseMap2 = loadTexture("Textures/box.jpg");
    unsigned int specularMap2 = loadTexture("Textures/box.jpg");

    textureShader.use();
    textureShader.setInt("material.diffuse", 0);
    textureShader.setInt("material.specular", 1);

    while (!glfwWindowShouldClose(window))
    {
        // Time calculation for movement
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        textureShader.use();
        textureShader.setVec3("light.position", sunPos);
        textureShader.setVec3("viewPos", camera.Position);

        // Light properties
        textureShader.setVec3("light.ambient", 0.5f, 0.6f, 0.6f);
        textureShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        textureShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // material properties
        textureShader.setFloat("material.shininess", 64.0f);

        textureShader.setMat4("projection", projection);
        textureShader.setMat4("view", view);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap2);

        // World transformation for the box
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Translate the box upwards
        textureShader.setMat4("model", model);

        glBindVertexArray(box.getVAO());
        glDrawElements(GL_TRIANGLES, box.getIndexCount(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Is updating the positions of the balls, detecting collision, resolving collision using an octree to 
        //optimize the collision. 
        //Pressing space "ballMoving will happend. 
        if (ballsMoving)
        {
            // The Octree is created over the box with the mininum and maximum boundaries. 
            Octree octree(glm::vec3(boxMin, boxMin, boxMin), glm::vec3(boxMax, boxMax, boxMax), 0, 4, 4);

            // Updates the ball positions and inserts each ball into the octree
            for (int i = 0; i < ballPositions.size(); ++i)
            {
                // The position is updated for each ball based on the velocity and deltatime. 
                //deltaTime is important for smooth movement 
                ballPositions[i] += ballVelocities[i] * deltaTime;

                // Checks if the ball has hit the wall of the box. 
                //If the balls goes over the box boundaries in x or z direction, the velocity is reversed. 
                //clamp makes the ball stay in the box boundaries. 
                if (ballPositions[i].x - ballRadius <= boxMin or ballPositions[i].x + ballRadius >= boxMax)
                {
                    ballVelocities[i].x = -ballVelocities[i].x;
                    ballPositions[i].x = glm::clamp(ballPositions[i].x, boxMin + ballRadius, boxMax - ballRadius);
                }
                if (ballPositions[i].z - ballRadius <= boxMin or ballPositions[i].z + ballRadius >= boxMax)
                {
                    ballVelocities[i].z = -ballVelocities[i].z;
                    ballPositions[i].z = glm::clamp(ballPositions[i].z, boxMin + ballRadius, boxMax - ballRadius);
                }

                //After the position and collision with the walls are uodated, the ballVelocity is normalized. 
                //The direction remains the same, but the length is set to 1 and then scaled with the ballSpeed.
                //Ensures that the balls have constant speed 
                ballVelocities[i] = glm::normalize(ballVelocities[i]) * ballSpeed;

                // Each ball is inserted into the octree with index, radius and position. This helps for faster collision detection
                octree.insert(i, ballPositions, ballRadius);
            }

            // The pairs of balls that can potentially collide is stored inside the vector named i and j
            vector<pair<int, int>> potentialCollisions;
            octree.getPotentialCollisions(potentialCollisions, ballPositions, ballRadius);

            // Goes trough each potential collision and checks if the balls are actually colliding. 
            for (const pair<int, int>& pair : potentialCollisions)
            {
                int i = pair.first; //Gets the index of the first ball in the pair
                int j = pair.second; //Gets the index of the second ball in the pair

                //If the balls are colliding the function adjuts the positions and velocity of the two balls 
                if (checkCollision(ballPositions[i], ballPositions[j], ballRadius, ballRadius))
                {
                    whenCollisionHappens(ballPositions[i], ballVelocities[i], ballPositions[j], ballVelocities[j], ballRadius);
                }
            }
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap1);

        // Render all balls
        for (int i = 0; i < ballPositions.size(); ++i)
        {
            // Update the rotation based on velocity
            balls[i].UpdateRotation(ballVelocities[i], deltaTime, ballsMoving);

            model = glm::mat4(1.0f);
            model = glm::translate(model, ballPositions[i]);
            model = model * balls[i].rotationMatrix; // Adding rotation
            textureShader.setMat4("model", model);
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

//This function checks if two balls are colliding, this is based on the position and radius
//posA and posB is the center points of the two balls 
//radiusA and radiusB are the radius of the two balls. 
//glm::distance claculates a straight- line distance between thw centers of the two balls 
//If the distance of the two balls are less than the radius of the two balls, it means that they are 
//overlapping and collinding. 
//It returns true if the balls are colliding. 
bool checkCollision(glm::vec3 posA, glm::vec3 posB, float radiusA, float radiusB)
{
    float distance = glm::distance(posA, posB);
    return distance < (radiusA + radiusB);
}

//This function is used when collision happens. 
//p1 and p2 are the position of the two balls colliding. 
// v1 and v2 are the velocity of the two balls. (these values are passed by reference)
//This function assume that the radius is the same for both of the balls. 
void whenCollisionHappens(glm::vec3& p1, glm::vec3& v1, glm::vec3& p2, glm::vec3& v2, float radius)
{
    // Mass of the balls, the mass are equal 
    float m1 = 1.0f;
    float m2 = 1.0f;

    // The normal vector is the direction of the center of one ball to the center of the second ball.
    glm::vec3 normal = p1 - p2;
    //Calculates the length of the normal. This calculates the distance between the two centers 
    float distance = glm::length(normal);
    //Calculates the length of the vector and making its magnitude to 1 while keeping its direction. 
    normal = glm::normalize(normal);

    // Ensures that the two balls do not overlap. 
    //If the distance between the two cneters are exactly equal to radius*2, they are touching, not overlapping
    //distance is the current distance between two balls. If the distance is less than radius*2, the balls have overlapped.
    // It the distance is less than radius*2 the value will be positive and this is indicating that the talls needs to be moved apart. 
    //
    float overlap = radius * 2 - distance;
    if (overlap > 0)
    {
        //The direction and distance the balls needs to move to seperate them. 
        //Each ball is moving half the overlap and in opposite directions. 
        glm::vec3 seperateTheBalls = normal * (overlap / 2.0f);
        //The first ball is moved away from the second ball
        //It is adding the seperateTheBalls vector. This shifts the direction of the vector 
        p1 += seperateTheBalls;
        //The second ball is moved away from the first ball 
        //It is subtracting the seperateTheBalls vector from its position and shifts the direction 
        p2 -= seperateTheBalls;
    }

    //v1 and v2 are the velocities of two differnt balls before collision.
    // If v1>v2 , ball1 is moving faster than ball 2 and vice versa
    glm::vec3 differenceBetweenTheVelocitiesOftheTwoBalls = v1 - v2;

    // Calculate velocity along the normal. 
    //This is telling how fast the two balls are moving towards each other or away from each other
    float velocityAlongNormal = glm::dot(differenceBetweenTheVelocitiesOftheTwoBalls, normal);

    // If the balls are moving apart, skip
    if (velocityAlongNormal > 0)
        return;

    // How bouncy the collision is. 
    // e=1.0f means the ball is bouncing of each other with no loss of kinectic energy(elatsic collision)
    float e = 1.0f;
    //The momentum that needs to be exhanged between the two balls to resolve the collision
    //-(1+e) reverses the direction of the motion, velocityAlongNormal is the speed the balls are moving towards each other
    // (1/m1)+(1/m2) accounts for how much the velocities of the balls should change, depending on their masses
    float j = -(1 + e) * velocityAlongNormal / (1 / m1 + 1 / m2);

    // The impulse pushes the balls away from each other adjusting their velocities 
    glm::vec3 impulse = j * normal;
    //Applies the impulse to the velocity on ball 1 
    v1 += impulse / m1;
    //Applies the impulse to the velocity on ball 2
    v2 -= impulse / m2;
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}



