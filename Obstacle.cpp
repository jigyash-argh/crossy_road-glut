#include "Obstacle.h"

// Width of the world
const float WORLD_WIDTH = 20.0f;

void Obstacle::update() {
    // Move the obstacle based on its speed
    // We use a fixed delta time (16ms / 1000.0f)
    xPos += speed * (16.0f / 1000.0f);

    // --- Wrap around logic ---
    // If obstacle goes off one side, move it to the other
    if (xPos > WORLD_WIDTH + width && speed > 0) {
        xPos = -WORLD_WIDTH - width;
    }
    else if (xPos < -WORLD_WIDTH - width && speed < 0) {
        xPos = WORLD_WIDTH + width;
    }
}