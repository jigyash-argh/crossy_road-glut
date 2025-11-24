#include "Lane.h"
#include "Vehicle.h"
#include <GL/glut.h>
#include <cstdlib>

const float LANE_WIDTH = 20.0f;

/**
 * @brief MODIFIED: Constructor implementation
 * Accepts and uses the speedMultiplier
 */
Lane::Lane(LaneType type, int zPos, float speedMultiplier) {
    this->type = type;
    this->zPosition = zPos;

    if (this->type == LANE_ROAD) {
        int numObstacles = (rand() % 3) + 1;
        
        // --- Calculate speed using the multiplier ---
        float baseSpeed = (rand() % 100 / 50.0f) + 0.5f; // Base speed
        float finalSpeed = baseSpeed * speedMultiplier;  // Apply difficulty
        
        if (rand() % 2 == 0) {
            finalSpeed *= -1.0f;
        }

        for (int i = 0; i < numObstacles; i++) {
            float xPos = (float)(rand() % (int)(LANE_WIDTH * 2)) - LANE_WIDTH;
            if (rand() % 4 == 0) {
                obstacles.push_back(new Bus(xPos, zPosition, finalSpeed));
            } else {
                obstacles.push_back(new Car(xPos, zPosition, finalSpeed));
            }
        }
    }
    else if (this->type == LANE_GRASS) {
        // Create a few decorative trees to make the grass lanes look nicer
        int numTrees = (rand() % 4) + 1; // 1..4 trees
        for (int i = 0; i < numTrees; ++i) {
            float tx = (float)(rand() % (int)(LANE_WIDTH * 2)) - LANE_WIDTH;
            treePositions.push_back(tx);
        }
    }
    // TODO: Add logic to create Logs for LANE_WATER
}

Lane::~Lane() {
    for (Obstacle* obs : obstacles) {
        delete obs;
    }
    obstacles.clear();
}

void Lane::draw() {
    glPushMatrix();
    
    if (type == LANE_GRASS) {
        glColor3f(0.0f, 0.6f, 0.2f); // Green
    } else if (type == LANE_ROAD) {
        glColor3f(0.2f, 0.2f, 0.2f); // Dark Gray
    } else if (type == LANE_WATER) {
        glColor3f(0.0f, 0.2f, 0.8f); // Blue
    }

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-LANE_WIDTH, 0.0f, (float)zPosition);
        glVertex3f(LANE_WIDTH, 0.0f, (float)zPosition);
        glVertex3f(LANE_WIDTH, 0.0f, (float)zPosition + 1.0f);
        glVertex3f(-LANE_WIDTH, 0.0f, (float)zPosition + 1.0f);
    glEnd();
    
    glPopMatrix();

    // Draw decorative trees for grass lanes
    if (type == LANE_GRASS) {
        for (float tx : treePositions) {
            glPushMatrix();
            // Trunk
            glTranslatef(tx, 0.5f, (float)zPosition + 0.5f);
            glColor3f(0.45f, 0.25f, 0.07f); // Brown trunk
            glPushMatrix();
            glScalef(0.2f, 1.0f, 0.2f);
            glutSolidCube(1.0f);
            glPopMatrix();

            // Foliage (cone)
            glTranslatef(0.0f, 0.9f, 0.0f);
            glColor3f(0.0f, 0.5f, 0.15f);
            // Use cone for a tree-like shape
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glutSolidCone(0.6f, 1.0f, 8, 8);
            glPopMatrix();
        }
    }

    for (Obstacle* obs : obstacles) {
        obs->draw();
    }
}

void Lane::update() {
    for (Obstacle* obs : obstacles) {
        obs->update();
    }
}

/**
 * @brief AABB Collision Check
 */
bool Lane::checkCollision(float playerX) {
    // Player's bounding box
    float playerMinX = playerX - 0.5f;
    float playerMaxX = playerX + 0.5f;

    for (Obstacle* obs : obstacles) {
        // Obstacle's bounding box
        float obsX = obs->getX();
        float obsWidth = obs->getWidth();
        float obsMinX = obsX - obsWidth / 2.0f;
        float obsMaxX = obsX + obsWidth / 2.0f;

        // AABB check:
        // Return true if they overlap
        if (playerMaxX > obsMinX && playerMinX < obsMaxX) {
            return true; // Collision!
        }
    }

    // No collision found with any obstacle in this lane
    return false;
}