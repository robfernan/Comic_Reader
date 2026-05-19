#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Comic Reader - Background Demo", sf::Style::Default);
    window.setFramerateLimit(60);

    sf::Clock clock;

    sf::Shader shader;
    if (!shader.loadFromFile("shaders/background.frag", sf::Shader::Fragment)) {
        std::cerr << "Failed to load shader: shaders/background.frag" << std::endl;
        return 1;
    }

    // Load base artwork to composite under the procedural flames
    sf::Texture baseTex;
    sf::Sprite baseSprite;
    bool haveBase = false;
    if (baseTex.loadFromFile("assets/digitalComicsUi.jpg")) {
        baseTex.setSmooth(true);
        baseSprite.setTexture(baseTex);
        haveBase = true;
    } else {
        std::cerr << "Warning: failed to load base artwork at assets/digitalComicsUi.jpg. Continuing with procedural background only." << std::endl;
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::Resized) {
                sf::View v = window.getView();
                v.setSize(event.size.width, event.size.height);
                v.setCenter(event.size.width/2.f, event.size.height/2.f);
                window.setView(v);
            }
        }

        float t = clock.getElapsedTime().asSeconds();
        sf::Vector2u sz = window.getSize();

        shader.setUniform("u_time", t);
        shader.setUniform("u_resolution", sf::Glsl::Vec2(static_cast<float>(sz.x), static_cast<float>(sz.y)));
        // If baseTex was set into the shader, it will be sampled in the fragment shader

        window.clear();

        // Draw base artwork first (if available)
        if (haveBase) {
            // scale base sprite to fill window while preserving aspect
            sf::Vector2u tSz = baseTex.getSize();
            float sx = static_cast<float>(sz.x) / tSz.x;
            float sy = static_cast<float>(sz.y) / tSz.y;
            float s = std::max(sx, sy);
            baseSprite.setScale(s, s);
            // center it
            sf::FloatRect br = baseSprite.getGlobalBounds();
            baseSprite.setPosition((sz.x - br.width)/2.f, (sz.y - br.height)/2.f);
            window.draw(baseSprite);
        }

        sf::RectangleShape full(sf::Vector2f(static_cast<float>(sz.x), static_cast<float>(sz.y)));
        full.setPosition(0.f, 0.f);
        // Draw flame overlay with additive blending to composite above base
        sf::RenderStates states;
        states.shader = &shader;
        states.blendMode = sf::BlendAdd;
        window.draw(full, states);

        window.display();
    }

    return 0;
}
