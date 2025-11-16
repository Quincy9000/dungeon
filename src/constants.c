#include "stdint.h"

#ifndef CONSTANTS_C
#define CONSTANTS_C

const int32_t ZOOM = 3;

const int32_t WIDTH = 800;
const int32_t HEIGHT = 600;

const int32_t GRID_WIDTH = 200;
const int32_t GRID_HEIGHT = 200;

const float_t TIME_STEP = 1.0 / 60.0;

const int32_t TILE_WIDTH = 5;
const int32_t TILE_HEIGHT = 5;

const size_t ROOM_COUNT = 30;

const size_t MIN_ROOM_WIDTH = 10;
const size_t MIN_ROOM_HEIGHT = 10;

const size_t MAX_ROOM_WIDTH = 20;
const size_t MAX_ROOM_HEIGHT = 15;

#endif // CONSTANTS_C