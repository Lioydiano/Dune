#include "include/sista/sista.hpp"

// Size of the whole field
#define WIDTH 50
#define HEIGHT 30
// Size of the safe area
#define INTERNAL_WIDTH 20
#define INTERNAL_HEIGHT 12


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