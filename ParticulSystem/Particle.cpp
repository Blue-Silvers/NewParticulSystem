#include "Particle.h"

void Particle::Update(float dt)
{
	sf::Vector2f movement = position - lastPosition;
	lastPosition = position;
	position = position + movement + acceleration * (dt * dt);
	acceleration = {}; //reset accelerration

	gridX = position.x / 10;
	gridY = position.y / 10;
}

void Particle::Accelerate(sf::Vector2f pBonus)
{
	acceleration += pBonus;
}

void Particle::SetVelocity(sf::Vector2f pVelocity, float dt)
{
	lastPosition = position - (pVelocity * dt);
}

void Particle::AddVelocity(sf::Vector2f pVelocity, float dt)
{
	lastPosition -= (pVelocity * dt);
}

sf::Vector2f Particle::GetVelocity()
{
	return position - lastPosition;
}
