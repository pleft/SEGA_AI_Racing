# SEGA AI Racing

A classic-style racing game for the Sega Mega Drive/Genesis, created with SGDK 2.00.

## Build Instructions

1. Install [SGDK 2.00](https://github.com/Stephane-D/SGDK/releases/tag/v2.00)
2. Set the `%GDK%` environment variable to point to your SGDK installation directory
3. Add `%GDK%/bin` to your `%PATH%` environment variable
4. Open a command prompt in the project directory
5. Run `make -f %GDK%/makefile.gen`
6. Load the generated `out/rom.bin` file in your favorite Sega Mega Drive emulator (e.g., BlastEm, Gens, Fusion)

## Controls

- **D-Pad Left/Right**: Steer the car
- **Button A**: Brake
- **Button B**: Accelerate
- **Button C**: View scoreboard (on game over screen)
- **Start**: Start game / Pause / Resume

## Gameplay

Race along a winding track, avoiding other cars and trying to achieve the highest score. Your score increases based on your speed, and the game gets progressively more difficult the longer you play.

## Features

- Dynamic, scrolling road with curves and straight sections
- Random track generation for replayability
- Opponent cars with varied behavior
- Simple physics including acceleration, braking, and steering
- Score tracking and high score system
- Sound effects for various game events

## Credits

Created by Panos L (2025)
Developed using SGDK 2.00 by Stephane Dallongeville