#include "include/sista/sista.hpp"

// Size of the whole field
#define WIDTH 51
#define HEIGHT 29
// Size of the safe area
#define INTERNAL_WIDTH 20
#define INTERNAL_HEIGHT 12
// Sand worms
#define SAND_WORMS 10
#define SAND_WORM_SIZE 7


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
};