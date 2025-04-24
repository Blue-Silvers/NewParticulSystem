#pragma once
#include <thread>
#include <mutex>
#include <array>
#include <chrono>
#include <iostream>
#include <SFML/Graphics.hpp>

#include "VertexArrayGrid.h"
#include "Palette.h"
#include "ThreadPool.h"
#include "NumberGenerator.h"

struct RenderState
{
    VertexArrayGrid grid;
    float zoom = 1.0;
    sf::Vector2f offset = { 0.0, 0.0 };

    RenderState(uint32_t width, uint32_t height)
        : grid(width, height)
    {
    }
};

struct AsyncRenderer
{
    float ComputeIterations(sf::Vector2f pos)
    {
        uint32_t i{ 0 };
        float zr{ pos.x };
        float zi{ pos.y };
        float mod = zr * zr + zi * zi;
        while (mod < 4.0f && i < 1000 /*max_iteration*/) {
            const float tmp = zr;
            zr = zr * zr - zi * zi + -0.8f /*julia_r*/;
            zi = 2.0f * zi * tmp + 0.156f /*julia_i*/;
            mod = zr * zr + zi * zi;
            ++i;
        }
        return static_cast<float>(i) - static_cast<float>(log2(std::max(1.0f, log2(mod))));
    }

    RenderState states[2];

    float requested_zoom = 1.0;
    sf::Vector2f requested_center = {};

    float render_zoom = 1.0;
    sf::Vector2f render_center = {};

    uint32_t state_idx = 0;
    uint32_t texture_idx = 0;

    std::array<sf::Vector2f, 32> anti_aliasing_offsets;
    sf::RenderTexture textures[2];

    tp::ThreadPool thread_pool;

    Palette palette;

    float fade_time = 2.0f /*fade_time*/;

    AsyncRenderer(uint32_t width, uint32_t height, float zoom_)
        : thread_pool{ 16 }
        , states{ RenderState(width, height), RenderState(width, height) }
    {
        requested_zoom = zoom_;
        palette.addColorPoint(0.0f, sf::Color{ 25, 24, 23 });
        palette.addColorPoint(0.03f, sf::Color{ 120, 90, 70 });
        palette.addColorPoint(0.05f, sf::Color{ 130, 24, 23 });
        palette.addColorPoint(0.25f, sf::Color{ 250, 179, 100 });
        palette.addColorPoint(0.5f, sf::Color{ 43, 65, 98 });
        palette.addColorPoint(0.85f, sf::Color{ 11, 110, 79 });
        palette.addColorPoint(0.95f, sf::Color{ 150, 110, 79 });
        palette.addColorPoint(1.0f, sf::Color{ 255, 255, 255 });

        // Precompute Monte Carlo offsets
        for (auto& o : anti_aliasing_offsets) {
            o.x = RNGf::getUnder(1.0f);
            o.y = RNGf::getUnder(1.0f);
        }
        // If only one sample is used, no offset
        anti_aliasing_offsets[0] = {};

        for (uint32_t i{ 2 }; i--;) {
            //textures[i].create(width, height);
            textures[i].setSmooth(true);
        }
    }

    ~AsyncRenderer()
    {
        thread_pool.waitForCompletion();
    }

    void generate()
    {
        // Get the back buffer
        RenderState& state = states[!state_idx];
        VertexArrayGrid& grid = state.grid;
        render_zoom = requested_zoom;
        render_center = requested_center;

        // Generate colors
        const sf::Vector2f win_center = static_cast<float>(0.5) * sf::Vector2f{static_cast<float>(grid.size.x),
            static_cast<float>(grid.size.y)};
        const uint32_t slice_height = grid.size.y / thread_pool.m_thread_count;
        thread_pool.m_queue.m_remaining_tasks = thread_pool.m_thread_count;
        for (uint32_t k{ 0 }; k < thread_pool.m_thread_count; ++k) {
            thread_pool.addTaskNoIncrement([=] {
                constexpr float    sample_coef = 1.0f / static_cast<float>(16 /*samples_count*/);
                auto& grid = states[!state_idx].grid;
                for (uint32_t x{ 0 }; x < grid.size.x; ++x) {
                    for (uint32_t y{ k * slice_height }; y < (k + 1) * slice_height; ++y) {
                        const float xf = (static_cast<float>(x) - win_center.x) / render_zoom + render_center.x;
                        const float yf = (static_cast<float>(y) - win_center.y) / render_zoom + render_center.y;
                        // AA color accumulator
                        sf::Vector3f color_vec;
                        // Monte Carlo color integration
                        for (uint32_t i{ 16 /*samples_count*/ }; i--;) {
                            const sf::Vector2f off = anti_aliasing_offsets[i] / render_zoom;
                            const float iter = ComputeIterations({ xf + off.x, yf + off.y });
                            const float iter_ratio = iter / static_cast<float>(1000/*max_iteration*/);
                            color_vec += palette.getColorVec(iter_ratio);
                        }
                        // Update color
                        grid.setCellColor(x, y, Palette::vec3ToColor(color_vec * sample_coef));
                    }
                }
                });
        }
    }

    void render(float zoom, sf::Vector2f center, sf::RenderTarget& target)
    {
        // Check if background rendering is ready
        if (fade_time >= 2.0f /*fade_time*/ && thread_pool.isDone())
        {
            // Update state
            auto& state = states[!state_idx];
            state.zoom = render_zoom;
            state.offset = render_center;
            // Swap buffers
            state_idx = !state_idx;
            // Update texture
            textures[!texture_idx].draw(states[state_idx].grid.va);
            textures[!texture_idx].display();
            texture_idx = !texture_idx;
            fade_time = 0.0f;
            // Start next render
            generate();
        }

        requested_center = center;
        requested_zoom = zoom;
        const auto scale = getStateScale(state_idx);
        sf::Sprite sprite_new(textures[texture_idx].getTexture());
        const auto bounds = sprite_new.getGlobalBounds();
        const sf::Vector2f origin = sf::Vector2f{ bounds.size.x, bounds.size.y } *0.5f;
        sprite_new.setOrigin(origin);
        sprite_new.setPosition(origin + getStateOffset(state_idx));
        sprite_new.setScale({ scale, scale });

        const auto scale_old = getStateScale(!state_idx);
        sf::Sprite sprite_old(textures[!texture_idx].getTexture());
        sprite_old.setOrigin(origin);
        sprite_old.setPosition(origin + getStateOffset(!state_idx));
        sprite_old.setScale({ scale_old, scale_old });
        const auto alpha = static_cast<uint8_t>(std::max(0.0f, 1.0f - fade_time / 2.0f /*fade_time*/) * 255.0f);
        sprite_old.setColor(sf::Color{ 255, 255, 255, alpha });

        target.draw(sprite_new);
        target.draw(sprite_old);

        fade_time += 0.016f;
    }

    float getStateScale(uint32_t idx) const
    {
        return static_cast<float>(requested_zoom / states[idx].zoom);
    }

    sf::Vector2<float> getStateOffset(uint32_t idx) const
    {
        return static_cast<sf::Vector2f>((states[idx].offset - requested_center) * states[idx].zoom) * getStateScale(idx);
    }
};