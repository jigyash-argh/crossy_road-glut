#include "Lane.h"
#include "Vehicle.h" // Includes Car and Bus
#include <GL/glut.h>
#include <cstdlib> // For rand()

// Width of the lane in the X-direction
const float LANE_WIDTH = 20.0f;

Lane::Lane(LaneType type, int zPos) {
    this->type = type;
    this->zPosition = zPos;

    // --- Procedurally add obstacles ---
    if (this->type == LANE_ROAD) {
        // Add 1 to 3 obstacles per road lane
        int numObstacles = (rand() % 3) + 1;
        
        // All obstacles in a lane move in the same direction
        float speed = (rand() % 100 / 50.0f) + 1.0f; // Speed between 1.0 and 3.0
        if (rand() % 2 == 0) {
            speed *= -1.0f; // 50% chance to move left
        }

        for (int i = 0; i < numObstacles; i++) {
            // Spread them out along the x-axis
            float xPos = (float)(rand() % (int)(LANE_WIDTH * 2)) - LANE_WIDTH;

            // 1 in 4 chance of being a Bus
            if (rand() % 4 == 0) {
                obstacles.push_back(new Bus(xPos, zPosition, speed));
            } else {
                obstacles.push_back(new Car(xPos, zPosition, speed));
            }
        }
    }
}

Lane::~Lane() {
    // Clean up all Obstacle objects in the vector
    for (Obstacle* obs : obstacles) {
        delete obs;
    }
    obstacles.clear();
}

void Lane::draw() {
    // Draw the lane itself (the ground quad)
    glPushMatrix();
    
    // Set color based on lane type
    if (type == LANE_GRASS) {
        glColor3f(0.0f, 0.6f, 0.2f); // Green
    } else if (type == LANE_ROAD) {
        glColor3f(0.2f, 0.2f, 0.2f); // Dark Gray
    }
    // TODO: Add LANE_WATER color

    // Draw a 1-unit wide quad for this lane
    glBegin(GL_QUADS);
        // Use normals for correct lighting
        glNormal3f(0.0f, 1.0f, 0.0f); 
        glVertex3f(-LANE_WIDTH, 0.0f, (float)zPosition);
        glVertex3f(LANE_WIDTH, 0.0f, (float)zPosition);
        glVertex3f(LANE_WIDTH, 0.0f, (float)zPosition + 1.0f);
        glVertex3f(-LANE_WIDTH, 0.0f, (float)zPosition + 1.0f);
    glEnd();
    
    glPopMatrix();

    // --- Draw all obstacles in this lane ---
    for (Obstacle* obs : obstacles) {
        obs->draw();
    }
}

void Lane::update() {
    // --- Update all obstacles in this lane ---
    for (Obstacle* obs : obstacles) {
        obs->update();
    }
}