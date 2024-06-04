#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream> // For file handling

class ComicReader {
public:
    ComicReader(const std::string& comicFilePath) : window(sf::VideoMode(800, 600), "Comic Reader"), currentPageIndex(0), isFullscreen(false) {
        comicPages = extractImagesFromComic(comicFilePath);

        // Set initial view
        updateView();

        // Main loop
        while (window.isOpen()) {
            handleEvents();

            // Clear the window
            window.clear(sf::Color::White);

            // Apply view
            window.setView(view);

            // Draw the current comic pages
            drawPages();

            // Update page number text
            updatePageNumberText();

            // Display the window contents
            window.display();
        }
    }

private:
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::Resized:
                    updateView();
                    break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Right) {
                        isPanning = true;
                        panStart = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    }
                    break;
                case sf::Event::MouseButtonReleased:
                    if (event.mouseButton.button == sf::Mouse::Right) {
                        isPanning = false;
                    }
                    break;
                case sf::Event::MouseMoved:
                    if (isPanning) {
                        sf::Vector2f panEnd = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                        sf::Vector2f offset = panStart - panEnd;
                        view.move(offset);
                        panStart = panEnd;
                    }
                    break;
                case sf::Event::KeyPressed:
                    handleKeyPress(event.key.code);
                    break;
                default:
                    break;
            }
        }
    }

    void handleKeyPress(sf::Keyboard::Key key) {
        switch (key) {
            case sf::Keyboard::Left:
                currentPageIndex = (currentPageIndex > 0) ? currentPageIndex - 1 : 0;
                break;
            case sf::Keyboard::Right:
                currentPageIndex = (currentPageIndex < comicPages.size() - 1) ? currentPageIndex + 1 : comicPages.size() - 1;
                break;
            case sf::Keyboard::Up:
                view.zoom(0.9f);
                break;
            case sf::Keyboard::Down:
                view.zoom(1.1f);
                break;
            case sf::Keyboard::F11: // Toggle full-screen mode
                isFullscreen = !isFullscreen;
                updateView();
                break;
            case sf::Keyboard::Escape: // Close the window on escape key
                window.close();
                break;
            default:
                break;
        }
    }

    void updateView() {
        sf::Vector2u windowSize;
        if (isFullscreen) {
            sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
            windowSize.x = desktopMode.width;
            windowSize.y = desktopMode.height;
            // Adjust view for two pages side by side
            float pageWidth = windowSize.x / 2.f;
            float aspectRatioPage = static_cast<float>(comicPages[0].getSize().x) / comicPages[0].getSize().y;
            float pageHeight = pageWidth / aspectRatioPage;
            view.setSize(pageWidth * 2, std::min(pageHeight, static_cast<float>(windowSize.y)));
        } else {
            windowSize = window.getSize();
            // Adjust view for one page
            view.setSize(windowSize.x, windowSize.y);
        }

        // Center view on the current page
        view.setCenter(view.getSize().x / 2.f, view.getSize().y / 2.f);
    }

    void drawPages() {
    // Draw the current comic pages
    if (currentPageIndex < comicPages.size()) {
        sf::Sprite leftPage(comicPages[currentPageIndex]);
        leftPage.setPosition(0, 0);
        window.draw(leftPage);

        if (isFullscreen && currentPageIndex + 1 < comicPages.size()) {
            sf::Sprite rightPage(comicPages[currentPageIndex + 1]);
            rightPage.setPosition(leftPage.getGlobalBounds().width, 0); // Position the right page next to the left page
            window.draw(rightPage);
        }
    }
}



    void updatePageNumberText() {
        sf::Font font;
        if (font.loadFromFile("arial.ttf")) {
            sf::Text pageNumberText;
            pageNumberText.setFont(font);
            pageNumberText.setCharacterSize(20);
            pageNumberText.setFillColor(sf::Color::Black);
            pageNumberText.setPosition(10, 10); // Position of page number text
            pageNumberText.setString("Page: " + std::to_string(currentPageIndex + 1) + " / " + std::to_string(comicPages.size()));
            window.draw(pageNumberText);
        } else {
            std::cerr << "Failed to load font." << std::endl;
        }
    }

    std::vector<sf::Texture> extractImagesFromComic(const std::string& comicFilePath) {
        std::vector<sf::Texture> images;

        // Open the CBZ file
        std::ifstream cbzFile(comicFilePath, std::ios::binary);
        if (!cbzFile.is_open()) {
            std::cerr << "Failed to open CBZ file." << std::endl;
            return images;
        }

        // Read the entire CBZ file into memory
        std::vector<char> cbzData((std::istreambuf_iterator<char>(cbzFile)), std::istreambuf_iterator<char>());

        // Close the CBZ file
        cbzFile.close();

        // Extract images from the CBZ data
        std::string imageData(cbzData.begin(), cbzData.end());

        // Find start of first image
        size_t start = imageData.find("\xff\xd8");
        if (start == std::string::npos) {
            std::cerr << "Failed to find start of image data." << std::endl;
            return images;
        }

        // Iterate through image data, find each image, and load it into SFML texture
        size_t pos = start;
        while (pos != std::string::npos) {
            // Find end of current image
            size_t end = imageData.find("\xff\xd9", pos);
            if (end == std::string::npos) {
                std::cerr << "Failed to find end of image data." << std::endl;
                return images;
            }

            // Create image buffer from current image data
            std::string imageBuffer = imageData.substr(pos, end - pos + 2);

            // Load image buffer into SFML texture
            sf::Texture texture;
            sf::Image image;
            if (image.loadFromMemory(imageBuffer.data(), imageBuffer.size())) {
                texture.loadFromImage(image);
                images.push_back(texture);
            } else {
                std::cerr << "Failed to load image from memory." << std::endl;
            }

            // Move to start of next image
            pos = imageData.find("\xff\xd8", end);
        }

        return images;
    }

    sf::RenderWindow window; // SFML window
    std::vector<sf::Texture> comicPages; // Vector to store comic pages as textures
    unsigned int currentPageIndex; // Index of the current comic page
    sf::View view; // SFML view

    // Variables for panning
    bool isPanning = false;
    sf::Vector2f panStart;

    // Full-screen mode flag
    bool isFullscreen;
};

int main() {
    std::string comicFilePath = "comic.cbz"; // Change this to the path of your comic file
    ComicReader comicReader(comicFilePath);
    return 0;
}
