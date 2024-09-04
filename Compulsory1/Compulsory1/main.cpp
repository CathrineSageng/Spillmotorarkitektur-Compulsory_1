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

glm::vec3 ball01(0.3f, 0.0f, -0.3f);
glm::vec3 ball02(0.3f, 0.0f, 0.0f);
glm::vec3 ball03(-0.3f, 0.0f, 0.0f);
glm::vec3 ball04(0.0f, 0.0f, 0.3f);
glm::vec3 ball05(0.0f, 0.0f, -0.3f);
glm::vec3 ball06(-0.3f, 0.0f, 0.3f);
glm::vec3 ball07(0.3f, 0.0f, 0.3f);
glm::vec3 ball08(-0.3f, 0.0f, -0.3f);

// Camera settings
//This is the starting position of the of the camera 
Camera camera(glm::vec3(0.0f, 1.0f, 1.5f));
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



std::string vfs = ShaderLoader::LoadShaderFromFile("Triangle.vs");
std::string fs = ShaderLoader::LoadShaderFromFile("Triangle.fs");


int main()
{
    std::cout << "Vfs " << vfs.c_str() << std::endl;
    std::cout << "fs " << fs.c_str() << std::endl;
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Exam", NULL, NULL);
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


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // render the triangle
        ourShader.use();

        /// View/projection transformations
        //Projection simulates the way objects appear smaller as they move further away. 
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //POsitions and orients the scene from the camera's point of view. 
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // World transformation
        //Transformation applied to each individual object in the scene
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.2f, 0.0f)); // Translate the box upwards
        ourShader.setMat4("model", model);

        glBindVertexArray(box.getVAO());
        glDrawElements(GL_TRIANGLES, box.getIndexCount(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        //Ball1
        model = glm::mat4(1.0f);
        model = glm::translate(model, ball01);
        ourShader.setMat4("model", model);
        ball1.DrawBall();

        //Ball2
        model = glm::mat4(1.0f);
        model = glm::translate(model, ball02);
        ourShader.setMat4("model", model);
        ball2.DrawBall();

        //Ball3
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

        /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/

         // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
         // -------------------------------------------------------------------------------
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

