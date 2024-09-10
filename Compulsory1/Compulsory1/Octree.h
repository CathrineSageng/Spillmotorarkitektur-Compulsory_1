#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <glm/glm.hpp>
#include <utility>

class Octree {
public:
    // Constructor for the octree
    Octree(glm::vec3 minBounds, glm::vec3 maxBounds, int depth = 0, int maxDepth = 4, int maxObjects = 4);

    // Destructor to clean up children nodes
    ~Octree();

    // Insert a ball into the octree
    void insert(int ballIndex, const std::vector<glm::vec3>& ballPositions, float ballRadius);

    // Get all potential collision pairs
    void getPotentialCollisions(std::vector<std::pair<int, int>>& collisionPairs, const std::vector<glm::vec3>& ballPositions, float ballRadius);

private:
    glm::vec3 minBounds;  // Minimum bounds of this node
    glm::vec3 maxBounds;  // Maximum bounds of this node
    int maxDepth;         // Maximum depth for the tree
    int maxObjects;       // Maximum objects allowed before splitting
    std::vector<int> balls;  // List of ball indices in this node
    Octree* children[8];  // Array of child nodes

    // Helper functions
    bool isLeaf();        // Check if this is a leaf node
    void subdivide(const std::vector<glm::vec3>& ballPositions, float ballRadius);     // Subdivide this node into 8 children
    int getChildIndex(glm::vec3 pos);  // Get the appropriate child node index for a position
};

#endif // OCTREE_H


