#include "Octree.h"
#include <iostream>
#include <glm/glm.hpp>

// Constructor
Octree::Octree(glm::vec3 minBounds, glm::vec3 maxBounds, int depth, int maxDepth, int maxObjects)
    : minBounds(minBounds), maxBounds(maxBounds), maxDepth(maxDepth), maxObjects(maxObjects) {
    for (int i = 0; i < 8; ++i)
        children[i] = nullptr;
}

// Destructor to clean up children
Octree::~Octree() {
    for (int i = 0; i < 8; ++i) {
        delete children[i];
    }
}

// Check if the node is a leaf node (i.e., it has no children)
bool Octree::isLeaf() {
    for (int i = 0; i < 8; ++i)
        if (children[i] != nullptr) return false;
    return true;
}

// Insert a ball into the octree
void Octree::insert(int ballIndex, const std::vector<glm::vec3>& ballPositions, float ballRadius) {
    glm::vec3 pos = ballPositions[ballIndex];

    // If this node is a leaf and has space, store the ball
    if (isLeaf() && (balls.size() < maxObjects || maxDepth == 0)) {
        balls.push_back(ballIndex);
    }
    else {
        // If this node is full and it's not at max depth, subdivide if necessary
        if (isLeaf() && maxDepth > 0) {
            subdivide(ballPositions, ballRadius);  // Pass ballPositions and ballRadius here
        }

        // Find the correct child for this ball and insert into the child
        int childIndex = getChildIndex(pos);
        children[childIndex]->insert(ballIndex, ballPositions, ballRadius);  // Pass ballPositions and ballRadius as parameters
    }
}

// Subdivide the current node into 8 children
void Octree::subdivide(const std::vector<glm::vec3>& ballPositions, float ballRadius) {
    glm::vec3 mid = (minBounds + maxBounds) * 0.5f;

    for (int i = 0; i < 8; ++i) {
        glm::vec3 newMin = minBounds;
        glm::vec3 newMax = maxBounds;

        // Determine the new bounds based on the child's index
        if (i & 1) newMin.x = mid.x; else newMax.x = mid.x;
        if (i & 2) newMin.y = mid.y; else newMax.y = mid.y;
        if (i & 4) newMin.z = mid.z; else newMax.z = mid.z;

        children[i] = new Octree(newMin, newMax, maxDepth - 1, maxDepth, maxObjects);
    }

    // Re-insert the current balls into the appropriate children
    for (int i = 0; i < balls.size(); ++i) {
        int childIndex = getChildIndex(ballPositions[balls[i]]);
        children[childIndex]->insert(balls[i], ballPositions, ballRadius);  // Ensure ballPositions and ballRadius are passed here
    }
    balls.clear();  // Clear the current node's balls, they are now in the children
}

// Get the child index based on a ball's position
int Octree::getChildIndex(glm::vec3 pos) {
    glm::vec3 mid = (minBounds + maxBounds) * 0.5f;
    int index = 0;
    if (pos.x > mid.x) index |= 1;
    if (pos.y > mid.y) index |= 2;
    if (pos.z > mid.z) index |= 4;
    return index;
}

// Get potential collisions (pairs of balls that are in the same or neighboring nodes)
void Octree::getPotentialCollisions(std::vector<std::pair<int, int>>& collisionPairs, const std::vector<glm::vec3>& ballPositions, float ballRadius) {
    // Check for collisions between balls in the same node
    for (int i = 0; i < balls.size(); ++i) {
        for (int j = i + 1; j < balls.size(); ++j) {
            collisionPairs.push_back(std::make_pair(balls[i], balls[j]));
        }
    }

    // If this is not a leaf, check the child nodes
    if (!isLeaf()) {
        for (int i = 0; i < 8; ++i) {
            if (children[i]) {
                children[i]->getPotentialCollisions(collisionPairs, ballPositions, ballRadius);  // Pass ballPositions and ballRadius
            }
        }
    }
}

