#include "stdint.h"

#ifndef CONSTANTS_C
#define CONSTANTS_C

const int VISIBILITY_MASK = 1 << 0;
const int EXPLORED_MASK = 1 << 1;
const int SOLID_MASK = 1 << 2;
const int SEEN_MASK = 1 << 3;

const int32_t ZOOM = 2;

const int32_t WIDTH = 800;
const int32_t HEIGHT = 600;

const int32_t GRID_WIDTH = 250;
const int32_t GRID_HEIGHT = 250;

const float_t TIME_STEP = 1.0 / 60.0;

const int32_t TILE_WIDTH = 6;
const int32_t TILE_HEIGHT = 6;

const size_t ROOM_COUNT = 30;

const size_t MIN_ROOM_WIDTH = 10;
const size_t MIN_ROOM_HEIGHT = 10;

const size_t MAX_ROOM_WIDTH = 20;
const size_t MAX_ROOM_HEIGHT = 15;

#endif // CONSTANTS_C