#include "Fractal.h"

Fractal::Fractal()
{
}

void Fractal::launchGame()
{
    const sf::Vector2u windowSize{ 1000, 750 };
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode({ windowSize.x, windowSize.y }), "Fractal", sf::State::Windowed, settings);
    const uint32_t frameRate = 60;
    window.setFramerateLimit(frameRate);
    window.setMouseCursorVisible(false);
    window.setKeyRepeatEnabled(false);

    const auto zoom_factor = static_cast<float>(1.005);
    const auto speed = static_cast<float>(1.0);
    auto zoom = static_cast<float>(window.getSize().y) / 2;
    sf::Vector2f center{ 0.0, 0.0 };

    AsyncRenderer renderer{ windowSize.x, windowSize.y, zoom };

    bool zoom_in = false;
    bool zoom_out = false;
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;


    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) // use sf::Keyboard::Key:: for detect input
            {
                window.close();
            }
        }

        Event(zoom_in, zoom_out, left, right, up, down);

        const float offset = speed / zoom;
        zoom = zoom_in ? zoom * zoom_factor : (zoom_out ? zoom / zoom_factor : zoom);

        center.x += left ? -offset : (right ? offset : 0);
        center.y += up ? -offset : (down ? offset : 0);

        window.clear(sf::Color::Black);
        renderer.render(zoom, center, window);
        window.display();
    }
}

void Fractal::Event(bool zoom_in, bool zoom_out, bool left, bool right, bool up, bool down)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        zoom_in = true;
    }
    else
    {
        zoom_in = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
    {
        zoom_out = true;
    }
    else
    {
        zoom_out = false;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        left = true;
    }
    else
    {
        left = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        right = true;
    }
    else
    {
        right = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    {
        up = true;
    }
    else
    {
        up = false;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
    {
        down = true;
    }
    else
    {
        down = false;
    }
}