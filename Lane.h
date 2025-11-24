#pragma once

#include <vector>
#include "Obstacle.h" 

// Define the types of lanes we can have
enum LaneType {
    LANE_GRASS,
    LANE_ROAD,
    LANE_WATER
};

class Lane {
public:
    // --- MODIFIED: Constructor now takes a speed multiplier ---
    Lane(LaneType type, int zPos, float speedMultiplier);
    ~Lane();

    void draw();
    void update();

    // --- Public getter functions ---
    int getZ() const { return zPosition; }
    LaneType getLaneType() const { return type; }
    
    /**
     * @brief Checks if the player at playerX collides with any obstacle
     * @return true if collision, false if safe
     */
    bool checkCollision(float playerX);

private:
    LaneType type;
    int zPosition;

    std::vector<Obstacle*> obstacles;
    // Positions for decorative trees on grass lanes
    std::vector<float> treePositions;
};