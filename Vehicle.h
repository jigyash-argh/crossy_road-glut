#pragma once

#include "Obstacle.h"

// --- Car Class ---
// Inherits from Obstacle
class Car : public Obstacle {
public:
    // Constructor calls the base Obstacle constructor
    Car(float x, int z, float s) 
        : Obstacle(x, z, s, 1.5f, 1.0f, 0.0f, 0.0f) { // Width=1.5, Color=Red
    }

    // Provide the "draw" function required by the base class
    virtual void draw() override {
        glPushMatrix();
        glTranslatef(xPos, 0.5f, (float)zPos + 0.5f);
        glScalef(width, 1.0f, 1.0f); // Scale to be wider
        glColor3f(r, g, b); // Use color from base class
        glutSolidCube(1.0f);
        glPopMatrix();
    }
};

// --- Bus Class ---
// Also inherits from Obstacle
class Bus : public Obstacle {
public:
    // Constructor calls the base Obstacle constructor
    Bus(float x, int z, float s) 
        : Obstacle(x, z, s, 3.0f, 1.0f, 0.8f, 0.0f) { // Width=3.0, Color=Yellow
    }

    // Provide the "draw" function
    virtual void draw() override {
        glPushMatrix();
        glTranslatef(xPos, 0.75f, (float)zPos + 0.5f);
        // Make the bus wider AND taller
        glScalef(width, 1.5f, 1.0f); 
        glColor3f(r, g, b);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
};