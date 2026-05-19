#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Comic Reader - Ribbon Demo (ImGui)");
    window.setFramerateLimit(60);

    if (!ImGui::SFML::Init(window)) {
        std::cerr << "Failed to initialize ImGui-SFML" << std::endl;
        return 1;
    }

    sf::Clock deltaClock;

    sf::Shader shader;
    if (!shader.loadFromFile("shaders/background.frag", sf::Shader::Fragment)) {
        std::cerr << "Failed to load shader" << std::endl;
        return 1;
    }

    // default parameters exposed to UI
    float speed = 0.3f;
    float intensity = 0.9f;
    float layerScale = 3.0f;
    float glow = 0.8f;
    ImVec4 colorA = ImVec4(0.8f, 0.1f, 0.05f, 1.0f);
    ImVec4 colorB = ImVec4(1.0f, 0.4f, 0.05f, 1.0f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::Resized) {
                sf::View v = window.getView();
                v.setSize(event.size.width, event.size.height);
                v.setCenter(event.size.width/2.f, event.size.height/2.f);
                window.setView(v);
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // UI
        ImGui::Begin("Ribbon Controls");
        ImGui::SliderFloat("Speed", &speed, 0.0f, 2.0f);
        ImGui::SliderFloat("Intensity", &intensity, 0.0f, 2.0f);
        ImGui::SliderFloat("Layer Scale", &layerScale, 0.5f, 6.0f);
        ImGui::SliderFloat("Glow", &glow, 0.0f, 2.0f);
        ImGui::ColorEdit3("Color A", (float*)&colorA);
        ImGui::ColorEdit3("Color B", (float*)&colorB);
        ImGui::End();

        // update shader uniforms
        float t = deltaClock.getElapsedTime().asSeconds();
        sf::Vector2u sz = window.getSize();
        shader.setUniform("u_time", t);
        shader.setUniform("u_resolution", sf::Glsl::Vec2((float)sz.x, (float)sz.y));
        shader.setUniform("u_speed", speed);
        shader.setUniform("u_intensity", intensity);
        shader.setUniform("u_layerScale", layerScale);
        shader.setUniform("u_glow", glow);
        shader.setUniform("u_colorA", sf::Glsl::Vec3(colorA.x, colorA.y, colorA.z));
        shader.setUniform("u_colorB", sf::Glsl::Vec3(colorB.x, colorB.y, colorB.z));

        window.clear(sf::Color::Black);

        // draw full-quad with shader
        sf::RectangleShape full(sf::Vector2f((float)sz.x, (float)sz.y));
        full.setPosition(0,0);
        sf::RenderStates states;
        states.shader = &shader;
        window.draw(full, states);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
