#include "include/sista/sista.hpp"
#include <unordered_map>

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
// Dev
#define DEBUG 0 // Debug mode
#define INTRO 1 // Show the intro

class SandWorm {
public:
    static ANSI::Settings sandWormBodyStyle;
    static ANSI::Settings sandWormHeadStyle;

    sista::Pawn* head; // The head of the worm
    std::vector<sista::Pawn*> body; // The body of the worm

    SandWorm();
    SandWorm(sista::Pawn* head);
    ~SandWorm();

    void move();
    void erase();
};


enum Direction {UP, RIGHT, DOWN, LEFT};
extern std::unordered_map<Direction, sista::Coordinates> directionMap;
extern std::unordered_map<Direction, char> directionSymbol;

class Bullet : public sista::Pawn {
public:
    static ANSI::Settings bulletStyle;
    Direction direction;

    Bullet() : sista::Pawn('^', sista::Coordinates(HEIGHT / 2 - 1, WIDTH / 2), bulletStyle), direction(UP) {}
    Bullet(sista::Pawn*, Direction);
    ~Bullet();

    void move();
};