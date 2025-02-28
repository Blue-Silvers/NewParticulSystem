#pragma once
#include <vector>
#include <iostream>
#include <math.h>
#include <SFML/Graphics.hpp>

struct Particle 
{
	sf::Vector2f position;
	sf::Vector2f lastPosition;
	sf::Vector2f acceleration;
	float radius = 10.0f;
	sf::Color color = sf::Color::Magenta;
	int gridX = 0, 
		gridY = 0, 
		id = 0;
	Particle() = default;
	Particle(sf::Vector2f pPosition, float pRadius, int pGridX, int pGridY, int pId) : position(pPosition)
																					, lastPosition(pPosition)
																					, acceleration(10.0f, 10.0f)
																					, radius(pRadius)
																					, gridX(pGridX)
																					, gridY(pGridY)
																					, id(pId)
	{
		
	}

	void Update(float dt);
	void Accelerate(sf::Vector2f pBonus);
	void SetVelocity(sf::Vector2f pVelocity, float dt);
	void AddVelocity(sf::Vector2f pVelocity, float dt);
	sf::Vector2f GetVelocity();
};
