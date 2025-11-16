#include "stdint.h"

#ifndef CONSTANTS_C
#define CONSTANTS_C

const int32_t WIDTH = 800;
const int32_t HEIGHT = 800;

const int32_t GRID_WIDTH = 150;
const int32_t GRID_HEIGHT = 150;

const float TIME_STEP = 1.0 / 60.0;

const int TILE_WIDTH = ((float)WIDTH / (float)GRID_WIDTH);
const int TILE_HEIGHT = ((float)HEIGHT / (float)GRID_HEIGHT);

const size_t roomCount = 30;

const size_t minRoomWidth = 10;
const size_t minRoomHeight = 10;

const size_t maxRoomWidth = 20;
const size_t maxRoomHeight = 15;

#endif // CONSTANTS_C