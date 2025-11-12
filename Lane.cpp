#include "Lane.h"
#include "Vehicle.h"
#include <GL/glut.h>
#include <cstdlib>

const float LANE_WIDTH = 20.0f;

Lane::Lane(LaneType type, int zPos) {
    this->type = type;
    this->zPosition = zPos;

    if (this->type == LANE_ROAD) {
        int numObstacles = (rand() % 3) + 1;
        float speed = (rand() % 100 / 50.0f) + 1.0f;
        if (rand() % 2 == 0) {
            speed *= -1.0f;
        }

        for (int i = 0; i < numObstacles; i++) {
            float xPos = (float)(rand() % (int)(LANE_WIDTH * 2)) - LANE_WIDTH;
            if (rand() % 4 == 0) {
                obstacles.push_back(new Bus(xPos, zPosition, speed));
            } else {
                obstacles.push_back(new Car(xPos, zPosition, speed));
            }
        }
    }
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
        glColor3f(0.0f, 0.6f, 0.2f);
    } else if (type == LANE_ROAD) {
        glColor3f(0.2f, 0.2f, 0.2f);
    } else if (type == LANE_WATER) {
        glColor3f(0.0f, 0.2f, 0.8f);
    }

    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-LANE_WIDTH, 0.0f, (float)zPosition);
        glVertex3f(LANE_WIDTH, 0.0f, (float)zPosition);
        glVertex3f(LANE_WIDTH, 0.0f, (float)zPosition + 1.0f);
        glVertex3f(-LANE_WIDTH, 0.0f, (float)zPosition + 1.0f);
    glEnd();
    
    glPopMatrix();

    for (Obstacle* obs : obstacles) {
        obs->draw();
    }
}

void Lane::update() {
    for (Obstacle* obs : obstacles) {
        obs->update();
    }
}

bool Lane::checkCollision(float playerX) {
    float playerMinX = playerX - 0.5f;
    float playerMaxX = playerX + 0.5f;

    for (Obstacle* obs : obstacles) {

        float obsX = obs->getX();
        float obsWidth = obs->getWidth();
        float obsMinX = obsX - obsWidth / 2.0f;
        float obsMaxX = obsX + obsWidth / 2.0f;
        if (playerMaxX > obsMinX && playerMinX < obsMaxX) {
            return true; 
        }
    }

    return false;
}