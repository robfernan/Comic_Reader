# Comic Book Reader

## Description
A **PSP Digital Comics-inspired** reader for modern Linux desktops. Built in C++ with SFML, this application delivers a **clean, minimal interface** for reading CBZ comic files with smooth navigation and intuitive controls. No clutter—just reading.

**Design Philosophy:** Simple, fast, elegant. Like the original PSP Comics Reader, but modernized for 2026.

## Design Goals
- **Minimal UI:** Navigation-only interface, zero chrome when reading
- **Fast navigation:** Arrow keys, intuitive page turning
- **Smart fitting:** Auto-fit to screen, zoom, pan capabilities  
- **Portable:** Single executable, zero configuration needed
- **Library aware:** Remember reading position, batch file support

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
    - Place your comic book pages in the `comics/` directory. 
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

## 2026 Modernization Roadmap

### Phase 1: Foundation (Current)
- [x] Core SFML rendering working with CBZ files
- [ ] **CMake build system** — Replace manual g++ compilation
- [ ] **Project restructuring** — src/, include/, assets/, build/ separation
- [ ] **.gitignore improvements** — Exclude CBZ files, build artifacts

### Phase 2: Robust CBZ Handling
- [ ] **libzip integration** — Replace manual binary searching with proper ZIP/CBZ parsing
- [ ] **Image caching** — Load extracted images into temp memory efficiently
- [ ] **Error handling** — Graceful fallbacks, user-friendly error messages
- [ ] **Metadata support** — Read EPUB/CBZ metadata when available

### Phase 3: UX Polish (PSP-Style)
- [ ] **Smart fit modes** — Fit-to-width, fit-to-height, 100%, best-fit
- [ ] **Dual-page mode** — Side-by-side reading (like manga)
- [ ] **Bookmarks** — Remember last page per comic
- [ ] **Config file** — Keybindings, UI preferences
- [ ] **Dark mode** — Eye-friendly reading

### Phase 4: Advanced Features
- [ ] **Library browser** — File picker UI, recent files
- [ ] **Batch processing** — Open multiple CBZ files sequentially
- [ ] **Performance** — Async image loading, preload next page
- [ ] **Export** — Save page as PNG

### Tech Stack (Target)
- **Build:** CMake 3.20+
- **Libraries:** SFML 2.5.1, libzip, spdlog (logging)
- **C++ Standard:** C++17
- **Platform:** Linux (Debian/Ubuntu primary)

## Contributing
This is a passion project to bring the elegant PSP Comics Reader experience to modern Linux. Pull requests welcome!

---
*Last updated: May 2026*

## Demo: Background reconstruction

I added a small demo that uses an OpenGL fragment shader to reproduce the PSP-style flame background. It's a starting point for making the UI identical to the original.

Build and run (requires SFML and CMake):

```bash
mkdir -p build && cd build
cmake ..
cmake --build . -- -j
./demo_background
```

The shader file is at `src/shaders/background.frag`. The CMake build copies the `shaders/` folder into the build directory so the demo can find it at runtime.

Next steps: integrate the shader into the main app UI, add title bar and carousel artwork rendering, and tune shader parameters to match the original art perfectly.

## Progress: Ribbon shader + ImGui

- Added an animated procedural ribbon shader (`src/shaders/background.frag`) that generates layered sine+noise ribbons and additive glow to mimic the "digital comics" ribbon effect.
- Added an ImGui-powered demo (`demo_imgui`) so you can tweak parameters live: color A/B, speed, intensity, layer scale, and glow.
- Built and tested on Debian Bookworm (SFML 2.5.1). The project vendors ImGui and ImGui-SFML (version-matched) to ensure builds are reproducible on Debian.

Run the ImGui demo to tune the background:

```bash
cd /home/rf80678/Documents/SFML/Comic_Reader/build
./demo_imgui
```

Goal: reproduce both screens from the PSP Digital Comics UI —
1) the startup menu (left-side list with the selection bar) and
2) the carousel view (centered cover carousel with highlighted item)

Both screens will use the same animated flame ribbon background; next I'll integrate the background into the UI shell and start building the carousel and startup menu overlays.

New shader: aurora-style ribbons

I added a new shader that renders a dark, minimalist aurora-style background with smooth translucent ribbons (`src/shaders/aurora.frag`). It's tuned for deep red / dark orange palettes and soft layered translucency.

To test it quickly (without recompiling), copy the shader into your build shaders folder so the demo can load it:

```bash
cp src/shaders/aurora.frag build/shaders/background.frag
cd build
./demo_imgui
```

Or edit the demo to load `aurora.frag` directly from `src/shaders/` if you prefer live-editing.


