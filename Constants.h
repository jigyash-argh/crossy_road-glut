#pragma once

// Half-width of the world in X direction. Obstacles and lanes span from -WORLD_HALF_WIDTH to +WORLD_HALF_WIDTH
static const float WORLD_HALF_WIDTH = 20.0f;

// Lane draw width (use same scale as world half width)
static const float LANE_WIDTH = WORLD_HALF_WIDTH;
