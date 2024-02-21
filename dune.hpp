#include "include/sista/sista.hpp"
#include <unordered_map>

// Size of the whole field
#define WIDTH 51
#define HEIGHT 29
// Size of the safe area
#define INTERNAL_WIDTH 20
#define INTERNAL_HEIGHT 12
// Sand worms
#define SAND_WORMS 10
#define SAND_WORM_SIZE 6


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