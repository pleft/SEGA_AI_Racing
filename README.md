# SEGA_AI_Racing

## Build and Run Instructions
1. Requires [SGDK 2.00](https://github.com/Stephane-D/SGDK/releases/tag/v2.00).
2. Set the `%GDK%` environment variable to point to the SGDK install directory.
3. Add `%GDK%/bin` to the `%PATH%` environment variable.
4. Build the project from the root directory: `make -f %GDK%/makefile.gen`.
5. Grab `rom.bin` from the `out/` directory and load it in your favorite Sega Mega Drive emulator (e.g., BlastEm).