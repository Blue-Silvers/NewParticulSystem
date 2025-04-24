#pragma once
#include <iostream>
#include "AsyncRenderer.h"
#define _USE_MATH_DEFINES
#include <math.h>

class Fractal
{
private:
	const uint32_t  samples_count = 16;
	const float julia_r = -0.8;
	const float julia_i = 0.156;
	const float fade_time = 2.0f;
	const uint32_t max_iteration = 1000;

public:
	Fractal();
	void launchGame();
	void Event( bool zoom_in,
				bool zoom_out,
				bool left,
				bool right,
				bool up,
				bool down);
};

