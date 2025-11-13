# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A water simulation written in C using the Raylib graphics library. The simulation runs on a tile-based grid where each tile contains a droplet with volume and size properties.

## Build and Run Commands

**Build:**
```bash
make
```
This compiles `src/main.c` into the `watersim` executable using clang with C99 standard.

**Run:**
```bash
./watersim
```

**Regenerate IDE compile flags:**
```bash
make compile_flags.txt
```
This generates the `compile_flags.txt` file for clangd/LSP support.

## Dependencies

- **Raylib**: Graphics library installed at `~/opt/include` and `~/opt/lib`
- **macOS Frameworks**: CoreVideo, IOKit, Cocoa, GLUT, OpenGL

The project assumes Raylib is installed in a non-standard location (`~/opt`), so custom include and library paths are configured in the Makefile.

## Architecture

The codebase is currently a single-file application (`src/main.c`) with:

- **Game struct**: Top-level container holding the simulation grid state (array of Droplets)
- **Droplet struct**: Represents a single tile with `volume` (float) and `size` (int) properties
- **Grid layout**: Fixed at 800x400 pixels with 10-pixel tiles (80x40 tile grid)
- **Rendering**: Each droplet is drawn as a blue rectangle if volume > 0

The simulation loop is standard Raylib: `InitWindow` → main loop with `BeginDrawing`/`EndDrawing` → `CloseWindow`.

## Known Issues

There's an indexing bug in `src/main.c:38` and initialization loops at lines 46-50: the code uses `x+y` for array indexing but should use `y*TILE_WIDTH+x` for proper 2D grid access.
