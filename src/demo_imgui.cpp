#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Comic Reader - Ribbon Demo (ImGui)");
    window.setFramerateLimit(60);

    ImGui::SFML::Init(window);

    sf::Clock deltaClock;
    sf::Clock runtimeClock;

    sf::Shader shader;
    if (!shader.loadFromFile("shaders/aurora.frag", sf::Shader::Fragment)) {
        std::cerr << "Failed to load shader" << std::endl;
        return 1;
    }

    // default parameters exposed to UI
    float speed = 0.3f;
    float intensity = 0.9f;
    float layerScale = 3.0f;
    float glow = 0.8f;
    float softness = 0.5f;
    int layers = 6;
    // Default recommended colors (desaturated, rich tones)
    ImVec4 colorA = ImVec4(0.29f, 0.05f, 0.05f, 1.0f); // #4A0D0D deep wine red
    ImVec4 colorB = ImVec4(0.85f, 0.29f, 0.09f, 1.0f); // #D94B18 intense core orange

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
        // Presets
        if (ImGui::Button("Preset: Subtle")) {
            speed = 0.15f; intensity = 0.6f; layerScale = 2.5f; glow = 0.6f; softness = 0.65f; layers = 4;
            colorA = ImVec4(0.45f, 0.06f, 0.03f, 1.0f); colorB = ImVec4(0.9f, 0.28f, 0.06f, 1.0f);
        }
        ImGui::SameLine();
        if (ImGui::Button("Preset: Vivid")) {
            speed = 0.45f; intensity = 1.4f; layerScale = 3.8f; glow = 1.1f; softness = 0.28f; layers = 8;
            colorA = ImVec4(1.0f, 0.12f, 0.03f, 1.0f); colorB = ImVec4(1.0f, 0.45f, 0.06f, 1.0f);
        }
        ImGui::SameLine();
        if (ImGui::Button("Preset: Cinematic")) {
            speed = 0.22f; intensity = 0.95f; layerScale = 3.0f; glow = 0.9f; softness = 0.5f; layers = 6;
            colorA = ImVec4(0.7f, 0.08f, 0.03f, 1.0f); colorB = ImVec4(1.0f, 0.36f, 0.06f, 1.0f);
        }

        ImGui::Separator();
        ImGui::SliderFloat("Speed", &speed, 0.0f, 2.0f);
        ImGui::SliderFloat("Intensity", &intensity, 0.0f, 2.0f);
        ImGui::SliderFloat("Layer Scale", &layerScale, 0.5f, 6.0f);
        ImGui::SliderFloat("Glow", &glow, 0.0f, 2.0f);
        ImGui::SliderFloat("Softness", &softness, 0.0f, 1.0f);
        ImGui::SliderInt("Layers", &layers, 1, 12);
        ImGui::ColorEdit3("Color A", (float*)&colorA);
        ImGui::ColorEdit3("Color B", (float*)&colorB);
        ImGui::End();

        // update shader uniforms
        float t = runtimeClock.getElapsedTime().asSeconds();
        sf::Vector2u sz = window.getSize();
        shader.setUniform("u_time", t);
        shader.setUniform("u_resolution", sf::Glsl::Vec2((float)sz.x, (float)sz.y));
        shader.setUniform("u_speed", speed);
        shader.setUniform("u_intensity", intensity);
        shader.setUniform("u_layerScale", layerScale);
        shader.setUniform("u_softness", softness);
        shader.setUniform("u_layers", layers);
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
