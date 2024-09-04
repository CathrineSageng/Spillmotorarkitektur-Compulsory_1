#ifndef BALL_H
#define BALL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include<iostream>
#include <vector>


class Ball
{
public:
    Ball(float radius, int sectors, int stacks);
    void DrawBall();

private:
    GLuint VAO, VBO, EBO;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
};

#endif
