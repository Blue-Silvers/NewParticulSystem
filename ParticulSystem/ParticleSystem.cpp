#include "Renderer.h"
#include "Solver.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

static sf::Color GetColor(float t) {
    const float r = sin(t);
    const float g = sin(t + 0.33f * 2.0f * M_PI);
    const float b = sin(t + 0.66f * 2.0f * M_PI);
    return { static_cast<uint8_t>(255.0f * r * r),
             static_cast<uint8_t>(255.0f * g * g),
             static_cast<uint8_t>(255.0f * b * b) };
}

//float GetRandom()
//{
//    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
//}

int main()
{
    //create window
    constexpr int32_t windowWidth = 840;
    constexpr int32_t windowHeight = 840;

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode({ windowWidth, windowHeight }), "More particles !!", sf::State::Windowed, settings);
    const uint32_t frameRate = 60;
    window.setFramerateLimit(frameRate);
    Renderer renderer{ window };

    Solver solver;
    //solver.SetBoundary({ windowWidth / 2, windowHeight / 2 }, (windowWidth - 20.0f) / 2);
    solver.SetWindowSize({ windowWidth , windowHeight });
    /*auto& object = solver.addObject({ 420.0f,100.0f }, 10.0f);*/
    const int maxObjects = 300;
    const float spawnDelay = 0.05f;
    const float minRadius = 10;
    //const float maxRadius = 10;
    const float maxAngle = M_PI * 0.5f;
    sf::Vector2f spawnPosition = { windowWidth / 2, 100.0f };
    float startVelocity = 50.0f;
    sf::Clock respawnClock, timer;

    //Game loop
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) // use sf::Keyboard::Key:: for detect input
            {
                window.close();
            }
        }

        //Spawn particle
        if (solver.GetObjects().size() < maxObjects && respawnClock.getElapsedTime().asSeconds() >= spawnDelay)
        {
            
            float t = timer.getElapsedTime().asSeconds();

            float radius = minRadius /*+ (maxRadius - minRadius) * GetRandom()*/;
            auto& object = solver.addObject(spawnPosition, radius);

            object.color = GetColor(t);
            float angle = M_PI*0.5f + maxAngle * sin(3 * t);
            solver.SetObjectVelocity(object, startVelocity * sf::Vector2f{ cos(angle), sin(angle) });
            respawnClock.restart();
        }

        //Detect mouse input
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) 
        {
            float ratio = windowWidth / window.getSize().x; //correction for scalled window
            sf::Vector2f position = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)) * ratio;
            solver.MousePull(position);
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
        {
            float ratio = windowWidth / window.getSize().x; //correction for scalled window
            sf::Vector2f position = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)) * ratio;
            solver.MousePush(position);
        }

        solver.Update();
        window.clear(sf::Color::White);
        renderer.render(solver);
        window.display();
    }

    return 0;
}