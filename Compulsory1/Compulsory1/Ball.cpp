#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "Ball.h"


Ball::Ball(float radius, int sectors, int stacks, glm::vec3 color)
{
    VAO = 0;
    VBO = 0;
    EBO = 0;

    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = M_PI / 2 - i * M_PI / stacks;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * 2 * M_PI / sectors;

            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Assign the same color to every vertex of this ball
            colors.push_back(color.r);
            colors.push_back(color.g);
            colors.push_back(color.b);
        }
    }

    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    // Create VAO, VBO, EBO for vertices and colors
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat) + colors.size() * sizeof(GLfloat), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(GLfloat), vertices.data());
    glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), colors.size() * sizeof(GLfloat), colors.data());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(vertices.size() * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Ball::DrawBall()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}