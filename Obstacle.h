#pragma once
#include <GL/glut.h>

// This is an "abstract base class"
// It defines the "interface" for all obstacles
class Obstacle {
public:
    // Constructor
    Obstacle(float x, int z, float s, float w, float r, float g, float b) {
        this->xPos = x;
        this->zPos = z;
        this->speed = s;
        this->width = w;
        this->r = r;
        this->g = g;
        this->b = b;
    }

    // Virtual destructor (good practice for base classes)
    virtual ~Obstacle() {}

    // Public methods
    void update(); // Logic to move the obstacle

    // "Pure Virtual" function.
    // This means a class that inherits from Obstacle
    // MUST provide its own draw() function.
    virtual void draw() = 0; 

protected:
    // Protected members are accessible by child classes (Car, Bus)
    float xPos, speed, width;
    float r, g, b; // Color
    int zPos;
};