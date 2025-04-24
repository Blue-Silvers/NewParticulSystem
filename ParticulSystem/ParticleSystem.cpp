#pragma once
#include "ballsPool.h"
#include "Fractal.h"

ballsPool newBallsPool;
Fractal fractal;

int main()
{
    newBallsPool.launchGame();
    //fractal.launchGame();

    return 0;
}