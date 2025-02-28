#include "Solver.h"
#include <string>
#include <SFML/Graphics.hpp>

class Renderer 
{
private:
	sf::RenderTarget& target;

public:
	Renderer(sf::RenderTarget& pTarget) : target(pTarget) 
	{
	}

    void render(Solver& solver) const {
        //Background
        target.clear(sf::Color::Black);

        //Drawn boundary
        /*const sf::Vector3f boundary = solver.GetBoundary();
        sf::CircleShape boundaryBackground{ boundary.z };
        boundaryBackground.setOrigin({ boundary.z , boundary.z });
        boundaryBackground.setFillColor(sf::Color::Black);
        boundaryBackground.setPosition({ boundary.x , boundary.x });
        boundaryBackground.setPointCount(128);
        target.draw(boundaryBackground);*/

        // Draw objects
        sf::CircleShape circle{ 1.0f };
        circle.setPointCount(32);
        //circle.setOrigin({ 1.0f, 1.0f });
        const auto& objects = solver.GetObjects();
        for (const auto& obj : objects) {
            circle.setPosition(obj->position);
            circle.setScale({ obj->radius, obj->radius });
            circle.setFillColor(obj->color);
            target.draw(circle);
        }
    }
};
