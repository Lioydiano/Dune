# `Dune`

`Dune` is a Dune-themed, [`Sista`](https://github.com/FLAK-ZOSO/Sista)-based, 2D terminal game.

## OS Compatibility

`Dune` is compatible with Linux, Windows, and macOS, thanks to the `Sista` library and to the cross-platform input implemented in `cross_platform.hpp`.

## Requirements

- A C++17 compiler - for compiling the source code
Such as `g++`, `clang++`, `MSVC++`...
- `make` - for building the project (optional)
If you don't have `make`, you can compile the source code manually as described in the [installation](#installation) section.
- A terminal emulator that supports ANSI escape codes - for running the game
Such as `xterm`, `gnome-terminal`, `Windows Terminal`, `PowerShell`...

## Installation

- Download the latest release from the [releases page](https://github.com/Lioydiano/Dune/releases) in the form of a `.zip` file

- Extract the contents of the `.zip` file

- Open a terminal and navigate to the extracted folder

The terminal emulator must support ANSI escape codes

```bash
cd /path/to/extracted/folder
```

In your case, the path to the extracted folder may be different, for example:

```bash
cd /home/user/Downloads/Dune
# or, on Windows
cd C:\Users\user\Downloads\Dune
```

- Set the preprocessor directives in the source code as you please

This is optional and will be explained in the [customization](#customization) section

- Compile the source code

```bash
make
```

If you don't have `make`, you can compile the source code manually.

```bash
# Makefile for sista.cpp
IMPLEMENTATIONS = include/sista/ANSI-Settings.cpp include/sista/border.cpp include/sista/coordinates.cpp include/sista/cursor.cpp include/sista/field.cpp include/sista/pawn.cpp

all:
    g++ -std=c++17 -Wall -g -c $(IMPLEMENTATIONS)
    g++ -std=c++17 -Wall -g -c dune.cpp
    g++ -std=c++17 -Wall -g -o dune dune.o ANSI-Settings.o border.o coordinates.o cursor.o pawn.o field.o
    rm -f *.o
```

Consider that you can always find this commands in the [`Makefile`](Makefile) file.

- Run the game

If you are on Linux or macOS, you can run the game with the following command:

```bash
./dune
```

If you are on Windows, you can run the game with the following command:

```bash
dune
```

## How to play

### Story

You are a miner in the desert planet of Arrakis, and you are extracting *melange*.

There is a safe area, the mine, from which you can't escape, and a dangerous area, the desert, where the sand worms are.

Unfortunately, the sand worms are coming for you, and you have to defend yourself!

Don't worry, there's a "plane" that can make you escape if you want, but leaving the mine will make you stop extracting *melange* and thus scoring less points.

### Objective

The objective of the game is to survive as long as possible and to extract as much *melange* as possible, without being eaten by the sand worms.

### Commands

- Use the `Q` key to escape with the plane - the game will end
- Use the `W`, `A`, `S`, and `D` keys to move the miner
- Use the `I`, `J`, `K`, and `L` keys to fire bullets

Obviously you can't pause the game, sand worms don't wait for anyone!

### Mechanics

- The miner can't leave the mine
- The worms only die if they are shot in the head

### Scoring

- The score is the amount of *melange* extracted
- The score is increased by 1 every frame the miner is extracting *melange*

## Customization

You can customize the game by setting the preprocessor directives in the source code.

- Open the `dune.hpp` file

- Set the preprocessor directives as you please

```cpp
// Size of the whole field
#define WIDTH 31
#define HEIGHT 19
// Size of the safe area
#define INTERNAL_WIDTH 15
#define INTERNAL_HEIGHT 9
// Sand worms
#define SAND_WORMS 7 // The number of sand worms at the start
#define SAND_WORM_SIZE 7 // The size of the sand worm
#define SAND_WORMS_SPAWN_RATE 50 // 1 in 50 chance of spawning a sand worm
// Bullets
#define BULLETS_ON_ALARM 1 // Can we shoot bullets when the alarm is on?
#define BULLETS_WITHOUT_ALARM 1 // Can we shoot bullets when the alarm is off?
// Time
#define FRAME 500 // The time between frames [ms]
#define COOLDOWN 10 // The cooldown of the controls [ms]
```

This is an example of the default preprocessor directives.

Changing the preprocessor directives will change the game's behavior.
For example, you could have a larger field, more sand worms, or faster game speed.
