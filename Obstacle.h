#pragma once
#include <GL/glut.h>

class Obstacle {
public:
    Obstacle(float x, int z, float s, float w, float r, float g, float b) {
        this->xPos = x;
        this->zPos = z;
        this->speed = s; // <-- This was the error, changed from 'this.speed'
        this->width = w;
        this->r = r;
        this->g = g;
        this->b = b;
    }

    virtual ~Obstacle() {}

    void update();
    virtual void draw() = 0;

    // --- NEW: Public getters for collision detection ---
    float getX() const { return xPos; }
    float getWidth() const { return width; }

protected:
    float xPos, speed, width;
    float r, g, b;
    int zPos;
};