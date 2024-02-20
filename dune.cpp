#include "dune.hpp"


sista::Field* field;
sista::Border* border;
std::vector<SandWorm*> sandWorms;

int main() {
    std::ios_base::sync_with_stdio(false);
    ANSI::reset(); // Reset the settings

    // Initialize the sista stuff
    sista::Field field_(WIDTH, HEIGHT);
    field = &field_;
    sista::Border border_(
        '#', ANSI::Settings(
            ANSI::ForegroundColor::F_WHITE,
            ANSI::BackgroundColor::B_BLACK,
            ANSI::Attribute::BRIGHT
        )
    );
    border = &border_;
    field->print(border_);
    char c;
    std::cin >> c;

    return 0;
}


SandWorm::SandWorm() {
    // Choose where to spawn the worm [on the border of the field]
    int side = rand() % 4;
    int x, y;
    switch (side) {
        case 0: // Top
            x = rand() % WIDTH;
            y = 0;
            break;
        case 1: // Right
            x = WIDTH - 1;
            y = rand() % HEIGHT;
            break;
        case 2: // Bottom
            x = rand() % WIDTH;
            y = HEIGHT - 1;
            break;
        case 3: // Left
            x = 0;
            y = rand() % HEIGHT;
            break;
    }
    // Create the head
    head = new sista::Pawn(
        'H', sista::Coordinates(y, x),
        sandWormHeadStyle
    );
}
SandWorm::SandWorm(sista::Pawn* head) {
    this->head = head;
}
SandWorm::~SandWorm() {
    delete head;
    for (sista::Pawn* pawn : body) {
        delete pawn;
    }
}
void SandWorm::move() {
    // Check a random direction until it finds a valid one
    sista::Coordinates old_head_coords = head->getCoordinates();
    sista::Coordinates new_head_coords = old_head_coords;
    do {
        int direction = rand() % 4;
        switch (direction) {
            case 0: // Up
                new_head_coords.y--;
                break;
            case 1: // Right
                new_head_coords.x++;
                break;
            case 2: // Down
                new_head_coords.y++;
                break;
            case 3: // Left
                new_head_coords.x--;
                break;
        }
    } while (
        new_head_coords.x < 0 || new_head_coords.x >= WIDTH ||
        new_head_coords.y < 0 || new_head_coords.y >= HEIGHT ||
        !field->isFree(new_head_coords)
    );
    // Move the head
    field->movePawn(head, new_head_coords);
    // Move the body [no need to move the whole tail, just add a P2 to the head's old position]
    body.push_back(new sista::Pawn('o', old_head_coords, sandWormBodyStyle));
}
ANSI::Settings SandWorm::sandWormBodyStyle = ANSI::Settings(
    ANSI::ForegroundColor::F_GREEN,
    ANSI::BackgroundColor::B_BLACK,
    ANSI::Attribute::BRIGHT
);
ANSI::Settings SandWorm::sandWormHeadStyle = ANSI::Settings(
    ANSI::ForegroundColor::F_GREEN,
    ANSI::BackgroundColor::B_BLACK,
    ANSI::Attribute::BLINK
);