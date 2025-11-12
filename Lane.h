#pragma once

#include <vector>
#include "Obstacle.h" // We will create this next

// Define the types of lanes we can have
enum LaneType {
    LANE_GRASS,
    LANE_ROAD,
    LANE_WATER // We can add this later!
};

class Lane {
public:
    // Constructor: Creates a lane of a specific type at a z-position
    Lane(LaneType type, int zPos);
    
    // Destructor: Cleans up the obstacles
    ~Lane();

    void draw();
    void update(); // Updates all obstacles in this lane

private:
    LaneType type;
    int zPosition;

    // Each lane holds a list of its own obstacles
    std::vector<Obstacle*> obstacles;
};