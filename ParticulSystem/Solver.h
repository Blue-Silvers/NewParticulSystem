#pragma once
#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES 
#include <cmath>
#include <SFML/Graphics.hpp>
#include "Particle.h"

class Solver 
{
private:
    sf::Vector2f windowSize = { 840.0f, 840.0f };

    sf::Vector2f boundaryCenter = { 420.0f, 420.0f };
    float boundaryRadius = 100.0f;

    std::vector<Particle*> objects;
    sf::Vector2f gravity = { 0.0f, 1000.0f };
    float stepDt = 1.0f / 60;
    int subSteps = 8;

    int gridSize = 10;
    std::vector<int> grid[700];
    //std::vector<int> grid[350][350];

    void ApplyGravity();
    void ApplyBorder();
    void ApplyBoundary();
    void CheckCollisions();
    void CollideCells(int x1, int y1, int x2, int y2);

    void UpdateObject(float dt);

public:
    Solver() = default;

    Particle& addObject(sf::Vector2f pPosition, float pRadius);

    void Update();
    std::vector<Particle*> GetObjects();

    void MousePull(sf::Vector2f pPosition);
    void MousePush(sf::Vector2f pPosition);

    void SetWindowSize(sf::Vector2f pWindowSize);
    void UpdateGrid();

    void SetBoundary(sf::Vector2f pPosition, float pRadius);
    sf::Vector3f GetBoundary() const;

    float GetStepDt();
    void SetObjectVelocity(Particle& pObject, sf::Vector2f pVelocity);
};