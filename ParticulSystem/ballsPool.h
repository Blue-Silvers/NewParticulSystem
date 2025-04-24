#pragma once
#include "Renderer.h"
#include "Solver.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

class ballsPool
{
private:

public:
	ballsPool();
	static sf::Color GetColor(float t);
	float GetRandom();
	void launchGame();
};