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
        // Main body
        glTranslatef(xPos, 0.5f, (float)zPos + 0.5f);
        glColor3f(r, g, b);
        glPushMatrix();
        glScalef(width, 0.6f, 1.0f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Cabin / windows
        glPushMatrix();
        glTranslatef(0.0f, 0.25f, 0.0f);
        glColor3f(0.6f, 0.8f, 1.0f); // Light blue windows
        glScalef(width * 0.6f, 0.3f, 0.6f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Wheels (simple spheres)
        glColor3f(0.05f, 0.05f, 0.05f);
        float wheelY = 0.15f;
        float halfW = width / 2.0f - 0.2f;
        float zOff = 0.35f;

        // Front-left
        glPushMatrix();
        glTranslatef(-halfW, wheelY, -zOff);
        glScalef(0.2f, 0.2f, 0.2f);
        glutSolidSphere(1.0f, 8, 8);
        glPopMatrix();

        // Front-right
        glPushMatrix();
        glTranslatef(halfW, wheelY, -zOff);
        glScalef(0.2f, 0.2f, 0.2f);
        glutSolidSphere(1.0f, 8, 8);
        glPopMatrix();

        // Rear-left
        glPushMatrix();
        glTranslatef(-halfW, wheelY, zOff);
        glScalef(0.2f, 0.2f, 0.2f);
        glutSolidSphere(1.0f, 8, 8);
        glPopMatrix();

        // Rear-right
        glPushMatrix();
        glTranslatef(halfW, wheelY, zOff);
        glScalef(0.2f, 0.2f, 0.2f);
        glutSolidSphere(1.0f, 8, 8);
        glPopMatrix();

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
        // Main body
        glTranslatef(xPos, 0.9f, (float)zPos + 0.5f);
        glColor3f(r, g, b);
        glPushMatrix();
        glScalef(width, 1.0f, 1.0f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Windows row
        glPushMatrix();
        glTranslatef(0.0f, 0.15f, 0.0f);
        glColor3f(0.6f, 0.8f, 1.0f);
        // Draw several window blocks along width
        int windows = 4;
        float gap = (width - 0.6f) / (float)windows;
        float start = -width / 2.0f + 0.3f;
        for (int i = 0; i < windows; ++i) {
            glPushMatrix();
            glTranslatef(start + i * gap, 0.0f, -0.01f);
            glScalef(0.4f, 0.3f, 0.1f);
            glutSolidCube(1.0f);
            glPopMatrix();
        }
        glPopMatrix();

        // Wheels (spheres)
        glColor3f(0.02f, 0.02f, 0.02f);
        float wheelY = 0.2f;
        float halfW = width / 2.0f - 0.3f;
        float zOff = 0.5f;

        // Front-left
        glPushMatrix();
        glTranslatef(-halfW, wheelY, -zOff);
        glScalef(0.25f, 0.25f, 0.25f);
        glutSolidSphere(1.0f, 10, 10);
        glPopMatrix();

        // Front-right
        glPushMatrix();
        glTranslatef(halfW, wheelY, -zOff);
        glScalef(0.25f, 0.25f, 0.25f);
        glutSolidSphere(1.0f, 10, 10);
        glPopMatrix();

        // Rear-left
        glPushMatrix();
        glTranslatef(-halfW, wheelY, zOff);
        glScalef(0.25f, 0.25f, 0.25f);
        glutSolidSphere(1.0f, 10, 10);
        glPopMatrix();

        // Rear-right
        glPushMatrix();
        glTranslatef(halfW, wheelY, zOff);
        glScalef(0.25f, 0.25f, 0.25f);
        glutSolidSphere(1.0f, 10, 10);
        glPopMatrix();

        glPopMatrix();
    }
};