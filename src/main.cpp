#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <filesystem>

// Define our distinct application views
enum class AppScene {
    StartupMenu,     // Scene 1: Main options / Title Screen
    CarouselBrowser, // Scene 2: Horizontal Cover Flow layout
    PageReader       // Scene 3: Your original active page reading layout
};

// Refactored ComicReader class to handle only reader-state data & drawing
class ComicPageEngine {
public:
    std::vector<sf::Texture> comicPages;
    unsigned int currentPageIndex = 0;
    sf::View view;
    bool isFullscreen = false;
    bool isPanning = false;
    sf::Vector2f panStart;

    void loadComic(const std::string& comicFilePath) {
        comicPages = extractImagesFromComic(comicFilePath);
    }

    void handleKeyPress(sf::Keyboard::Key key, const sf::Vector2u& windowSize) {
        switch (key) {
            case sf::Keyboard::Left:
                currentPageIndex = (currentPageIndex > 0) ? currentPageIndex - 1 : 0;
                break;
            case sf::Keyboard::Right:
                if (!comicPages.empty()) {
                    currentPageIndex = (currentPageIndex < comicPages.size() - 1) ? currentPageIndex + 1 : comicPages.size() - 1;
                }
                break;
            case sf::Keyboard::Up:
                view.zoom(0.9f);
                break;
            case sf::Keyboard::Down:
                view.zoom(1.1f);
                break;
            case sf::Keyboard::F11:
                isFullscreen = !isFullscreen;
                updateView(windowSize);
                break;
            default:
                break;
        }
    }

    void updateView(const sf::Vector2u& windowSize) {
        if (comicPages.empty()) {
            view.setSize(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));
            view.setCenter(windowSize.x / 2.f, windowSize.y / 2.f);
            return;
        }

        if (isFullscreen) {
            sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
            float pageWidth = desktopMode.width / 2.f;
            float aspectRatioPage = static_cast<float>(comicPages[0].getSize().x) / comicPages[0].getSize().y;
            float pageHeight = pageWidth / aspectRatioPage;
            view.setSize(pageWidth * 2, std::min(pageHeight, static_cast<float>(desktopMode.height)));
        } else {
            view.setSize(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));
        }
        view.setCenter(view.getSize().x / 2.f, view.getSize().y / 2.f);
    }

    void drawActivePages(sf::RenderWindow& window) {
        if (currentPageIndex < comicPages.size()) {
            sf::Sprite leftPage(comicPages[currentPageIndex]);
            leftPage.setPosition(0, 0);
            window.draw(leftPage);

            if (isFullscreen && currentPageIndex + 1 < comicPages.size()) {
                sf::Sprite rightPage(comicPages[currentPageIndex + 1]);
                rightPage.setPosition(leftPage.getGlobalBounds().width, 0);
                window.draw(rightPage);
            }
        }
    }

    void drawPageNumberText(sf::RenderWindow& window) {
        sf::Font font;
        if (font.loadFromFile("arial.ttf")) {
            sf::Text pageNumberText;
            pageNumberText.setFont(font);
            pageNumberText.setCharacterSize(20);
            pageNumberText.setFillColor(sf::Color::White); // Changed to white to pop off black backgrounds
            pageNumberText.setPosition(20, 20);
            pageNumberText.setString("Page: " + std::to_string(currentPageIndex + 1) + " / " + std::to_string(comicPages.size()));
            window.draw(pageNumberText);
        }
    }

private:
    std::vector<sf::Texture> extractImagesFromComic(const std::string& comicFilePath) {
        std::vector<sf::Texture> images;
        std::ifstream cbzFile(comicFilePath, std::ios::binary);
        if (!cbzFile.is_open()) {
            std::cerr << "Failed to open CBZ file: " << comicFilePath << std::endl;
            return images;
        }

        std::vector<char> cbzData((std::istreambuf_iterator<char>(cbzFile)), std::istreambuf_iterator<char>());
        cbzFile.close();

        std::string imageData(cbzData.begin(), cbzData.end());
        size_t start = imageData.find("\xff\xd8");
        if (start == std::string::npos) return images;

        size_t pos = start;
        while (pos != std::string::npos) {
            size_t end = imageData.find("\xff\xd9", pos);
            if (end == std::string::npos) break;

            std::string imageBuffer = imageData.substr(pos, end - pos + 2);
            sf::Texture texture;
            sf::Image image;
            if (image.loadFromMemory(imageBuffer.data(), imageBuffer.size())) {
                texture.loadFromImage(image);
                images.push_back(texture);
            }

            pos = imageData.find("\xff\xd8", end);
        }
        return images;
    }
};

// --- MOCK UI DRAWING FUNCTIONS FOR MENU & CAROUSEL LAYOUTS ---
void drawStartupMenuUI(sf::RenderWindow& window) {
    sf::Font font;
    // Try multiple locations for font assets
    auto loadFontFallback = [&](sf::Font& f)->bool{
        if (f.loadFromFile("assets/arial.ttf")) return true;
        if (f.loadFromFile("../assets/arial.ttf")) return true;
        if (f.loadFromFile("arial.ttf")) return true;
        if (f.loadFromFile("../arial.ttf")) return true;
        return false;
    };
    if (loadFontFallback(font)) {
        sf::Text titleText("DIGITAL COMICS", font, 36);
        titleText.setFillColor(sf::Color::White);
        titleText.setPosition(50, 60);
        window.draw(titleText);

        std::vector<std::string> options = {"Browse Collection", "Recently Added", "Bookmarks", "Options"};
        for (size_t i = 0; i < options.size(); ++i) {
            // Pill highlights mimicking the active selection
            if (i == 0) {
                sf::RectangleShape selectionPill(sf::Vector2f(320, 40));
                selectionPill.setFillColor(sf::Color(217, 75, 24, 200)); // Core orange color theme
                selectionPill.setPosition(50, 150 + i * 55);
                window.draw(selectionPill);
            }
            sf::Text optText(options[i], font, 20);
            optText.setFillColor(sf::Color::White);
            optText.setPosition(65, 158 + i * 55);
            window.draw(optText);
        }
    }
}

void drawCarouselUI(sf::RenderWindow& window) {
    sf::Font font;
    auto loadFontFallback = [&](sf::Font& f)->bool{
        if (f.loadFromFile("assets/arial.ttf")) return true;
        if (f.loadFromFile("../assets/arial.ttf")) return true;
        if (f.loadFromFile("arial.ttf")) return true;
        if (f.loadFromFile("../arial.ttf")) return true;
        return false;
    };
    if (loadFontFallback(font)) {
        sf::Text headerText("All Comics", font, 24);
        headerText.setFillColor(sf::Color::White);
        headerText.setPosition(window.getSize().x / 2.f - headerText.getGlobalBounds().width / 2.f, 20);
        window.draw(headerText);

        // Render dummy frame cards to test screen sorting spacing
        for (int i = -2; i <= 2; ++i) {
            float centerX = window.getSize().x / 2.f + (i * 150);
            float centerY = window.getSize().y / 2.f;
            
            sf::RectangleShape card(sf::Vector2f(120, 180));
            card.setOrigin(60, 90);
            card.setPosition(centerX, centerY);
            
            if (i == 0) {
                card.setFillColor(sf::Color(80, 80, 80));
                card.setOutlineThickness(4.f);
                card.setOutlineColor(sf::Color(217, 75, 24)); // Focus orange line frame
                card.setScale(1.1f, 1.1f);
            } else {
                card.setFillColor(sf::Color(40, 40, 40, 180));
            }
            window.draw(card);
        }
    }
}

int main() {
    // Force standard 16:9 widescreen PSP viewport dimension configurations
    sf::RenderWindow window(sf::VideoMode(960, 544), "PSP Comic Reader Engine");
    window.setFramerateLimit(60);

    // Load custom ribbon background shader with dual-path fallback
    sf::Shader auroraShader;
    if (!auroraShader.loadFromFile("src/shaders/aurora.frag", sf::Shader::Fragment)) {
        if (!auroraShader.loadFromFile("../src/shaders/aurora.frag", sf::Shader::Fragment)) {
            std::cerr << "CRITICAL: Could not find aurora.frag in local or parent directories!" << std::endl;
            return -1;
        }
    }

    // Allocate full screen background asset structures
    sf::RenderTexture bgTexture;
    bgTexture.create(window.getSize().x, window.getSize().y);
    sf::Sprite bgSprite(bgTexture.getTexture());

    sf::Clock clock;
    AppScene currentScene = AppScene::StartupMenu;

    // Load comic book states with asset discovery
    ComicPageEngine readerEngine;
    // helper to find a .cbz in common locations
    auto find_first_cbz = [&]() -> std::string {
        // check explicit local file
        if (std::ifstream("comic.cbz")) return std::string("comic.cbz");
        if (std::ifstream("../comic.cbz")) return std::string("../comic.cbz");
        // search assets/ in project or parent
        std::vector<std::string> searchPaths = {"assets", "../assets"};
        for (auto &p : searchPaths) {
            std::filesystem::path dir(p);
            if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
                for (auto &entry : std::filesystem::directory_iterator(dir)) {
                    if (!entry.is_regular_file()) continue;
                    auto ext = entry.path().extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                    if (ext == ".cbz") return entry.path().string();
                }
            }
        }
        return std::string();
    };

    std::string comicPath = find_first_cbz();
    if (!comicPath.empty()) {
        readerEngine.loadComic(comicPath);
    } else {
        std::cerr << "CRITICAL: Could not find comic.cbz in project root or assets/" << std::endl;
        // continue without pages; UI will still run
    }
    readerEngine.updateView(window.getSize());

    // Main Game State Loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::Resized) {
                readerEngine.updateView(window.getSize());
            }

            // Universal Controller/Key Input handler
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                // TAB KEY: Switches scenes natively
                else if (event.key.code == sf::Keyboard::Tab) {
                    if (currentScene == AppScene::StartupMenu) currentScene = AppScene::CarouselBrowser;
                    else if (currentScene == AppScene::CarouselBrowser) currentScene = AppScene::PageReader;
                    else currentScene = AppScene::StartupMenu;
                }
                else {
                    // Send keys directly down to the reader if it is active
                    if (currentScene == AppScene::PageReader) {
                        readerEngine.handleKeyPress(event.key.code, window.getSize());
                    }
                }
            }

            // Route panning logic exclusively when viewing comic book pages
            if (currentScene == AppScene::PageReader) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                    readerEngine.isPanning = true;
                    readerEngine.panStart = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), readerEngine.view);
                }
                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
                    readerEngine.isPanning = false;
                }
                if (event.type == sf::Event::MouseMoved && readerEngine.isPanning) {
                    sf::Vector2f panEnd = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), readerEngine.view);
                    sf::Vector2f offset = readerEngine.panStart - panEnd;
                    readerEngine.view.move(offset);
                    readerEngine.panStart = panEnd;
                }
            }
        }

        // --- BACKGROUND SHADER SETUP ---
        auroraShader.setUniform("u_time", clock.getElapsedTime().asSeconds());
        auroraShader.setUniform("u_resolution", sf::Vector2f(window.getSize()));

        // --- RENDERING PIPELINE PIPES ---
        window.clear(sf::Color::Black); // Wipes backbuffer to clean black void

        if (currentScene == AppScene::StartupMenu) {
            window.setView(window.getDefaultView());
            window.draw(bgSprite, &auroraShader); // Background shader
            drawStartupMenuUI(window);            // Left side text options
        } 
        else if (currentScene == AppScene::CarouselBrowser) {
            window.setView(window.getDefaultView());
            window.draw(bgSprite, &auroraShader); // Background shader
            drawCarouselUI(window);               // Overlapping cover flow cards
        } 
        else if (currentScene == AppScene::PageReader) {
            // Apply custom camera parameters exclusively for rendering pages
            window.setView(readerEngine.view);
            readerEngine.drawActivePages(window);
            
            // Switch back to overlay layer coordinates to draw text indicators on top statically
            window.setView(window.getDefaultView());
            readerEngine.drawPageNumberText(window);
        }

        window.display();
    }

    return 0;
}