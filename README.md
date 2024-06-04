# Comic Book Reader

## Description
A simple comic book reader application developed in C++ using the SFML library. This application allows users to load and read comic book files, providing features like page navigation and zooming.

## Installation
1. **Clone the repository**:
    ```sh
    git https://github.com/robfernan/Comic_Reader
    cd comic-reader
    ```
2. **Install dependencies**:
    - Ensure you have SFML installed on your system. 
    - You can install it via `pacman` if your on Arch, Manjaro, or other Arch distributions:
      ```sh
      sudo pacman -S sfml
      ```

3. **Build the project**:
    ```sh
    g++ -o comic src/main.cpp -lsfml-graphics -lsfml-window -lsfml-system
    ```

## Usage
1. **Add your comic files**:
    - Place your comic book pages in the `comics/sample_comic/` directory. 
    - The application currently supports .cbz comic files
2. **Run the application**:
    ```sh
    ./comic
    ```

3. **Navigate through the pages**:
    - Use the right arrow key to move to the next page.
    - Use the left arrow key to move to the previous page.
    - Use the up arrow key to zoom in
    - Use the down arrow key to zoom out

## Features
- Load and display comic book files from the `comics` directory.
- Navigate through pages using arrow keys.

