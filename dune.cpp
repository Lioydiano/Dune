#include "dune.hpp"
#include "cross_platform.hpp"
#include <thread>
#include <chrono>


sista::Field* field;
sista::Border* border;
std::vector<SandWorm*> sandWorms;
sista::Pawn* miner;


int main() {
    #ifdef __APPLE__
        term_echooff();
    #endif
    std::ios_base::sync_with_stdio(false);
    ANSI::reset(); // Reset the settings
    srand(time(NULL)); // Seed the random number generator

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

    // Create the sand worms
    for (int i = 0; i < SAND_WORMS; i++) {
        sandWorms.push_back(new SandWorm());
    }
    // Initialize the safe area
    sista::Cursor cursor;
    ANSI::Settings safeAreaStyle(
        ANSI::ForegroundColor::F_BLACK,
        ANSI::BackgroundColor::B_YELLOW,
        ANSI::Attribute::BRIGHT
    );
    sista::Coordinates safeAreaPoint(
        (HEIGHT - INTERNAL_HEIGHT) / 2,
        (WIDTH - INTERNAL_WIDTH) / 2
    );
    field->print(border_);
    for (int y = (HEIGHT - INTERNAL_HEIGHT) / 2; y < (HEIGHT + INTERNAL_HEIGHT) / 2; y++) {
        for (int x = (WIDTH - INTERNAL_WIDTH) / 2; x < (WIDTH + INTERNAL_WIDTH) / 2; x++) {
            safeAreaPoint.y = y;
            safeAreaPoint.x = x;
            cursor.set(safeAreaPoint);
            safeAreaStyle.apply();
            std::cout << ' ';
        }
    }
    std::cout << std::flush;
    ANSI::reset(); // Reset the settings
    // Initialize the miner
    miner = new sista::Pawn(
        'M', sista::Coordinates(HEIGHT / 2, WIDTH / 2),
        ANSI::Settings(
            ANSI::ForegroundColor::F_YELLOW,
            ANSI::BackgroundColor::B_BLACK,
            ANSI::Attribute::BRIGHT
        )
    );
    field->addPrintPawn(miner);

    // Main loop
    bool finished = false, won = false;
    std::thread th = std::thread([&finished, &won]() {
        while (true) {
            #if defined(_WIN32) or defined(__linux__)
                char input = getch();
            #elif __APPLE__
                char input = getchar();
            #endif
            if (input == 'q' || input == 'Q') {
                break;
            }
            if (finished) {
                return;
            }
        }
        finished = true;
        won = true;
        return;
    });
    int counter = 0;
    while (true) {
        counter++;
        cursor.set(sista::Coordinates(HEIGHT + 2, WIDTH / 3));
        std::cout << "Melange: " << counter << std::flush;
        // Move the sand worms
        for (SandWorm* sandWorm : sandWorms) {
            sandWorm->move();
        }
        std::cout << std::flush;
        // Check if there's any sand worm in the INTERNAL_WIDTH x INTERNAL_HEIGHT area
        for (SandWorm* sandWorm : sandWorms) {
            sista::Coordinates head_coords = sandWorm->head->getCoordinates();
            if ( // Check if the head is inside the safe area
                head_coords.x >= (WIDTH - INTERNAL_WIDTH) / 2 &&
                head_coords.x < (WIDTH + INTERNAL_WIDTH) / 2 &&
                head_coords.y >= (HEIGHT - INTERNAL_HEIGHT) / 2 &&
                head_coords.y < (HEIGHT + INTERNAL_HEIGHT) / 2
            ) {
                // Worms are now able to hear the sound of the mining
                cursor.set(sista::Coordinates(HEIGHT + 3, WIDTH / 3));
                ANSI::setForegroundColor(ANSI::ForegroundColor::F_RED);
                std::cout << "The sand worms are coming!\n";
                std::cout << "You have three seconds to run away! [Q]\n";
                for (int i = 6; i > 0; i--) {
                    cursor.set(sista::Coordinates(HEIGHT + 4, WIDTH / 3 + 40));
                    std::cout << i << std::flush;
                    // Move the sand worms
                    for (SandWorm* sandWorm : sandWorms) {
                        sandWorm->move();
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
                finished = true;
                won = false;
                goto end;
            }
        }
        end:
        // Wait a bit
        if (finished) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    if (won) {
        cursor.set(sista::Coordinates(HEIGHT / 2, WIDTH + 10));
        ANSI::setForegroundColor(ANSI::ForegroundColor::F_GREEN);
        std::cout << "You won! " << "You estracted " << counter << " units of melange!";
    } else {
        cursor.set(sista::Coordinates(HEIGHT / 2, WIDTH + 10));
        ANSI::setForegroundColor(ANSI::ForegroundColor::F_RED);
        std::cout << "You lost, the worms ate you and your melange!\n";
        cursor.set(sista::Coordinates(HEIGHT / 2 + 1, WIDTH + 10));
        std::cout << "Not even Duke Leto Atreides could have saved you!";
    }
    std::cout << std::flush;
    ANSI::reset(); // Reset the settings
    std::cout << SHOW_CURSOR;
    th.join();

    #if defined(_WIN32) or defined(__linux__)
        getch();
    #elif __APPLE__
        getchar();
    #endif
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
    field->addPrintPawn(head);
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
    int trials = 7;
    int direction;
    do {
        new_head_coords = old_head_coords;
        direction = rand() % 4;
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
        if (trials <= 0) {
            return;
        }
        trials--;
    } while (
        new_head_coords.x < 0 || new_head_coords.x >= WIDTH ||
        new_head_coords.y < 0 || new_head_coords.y >= HEIGHT ||
        !field->isFree(new_head_coords)
    );
    // Move the head
    field->movePawn(head, new_head_coords);
    // Move the body [no need to move the whole tail, just add a P2 to the head's old position]
    char symbol;
    switch (direction) {
        case 0: // Up
            symbol = '^';
            break;
        case 1: // Right
            symbol = '>';
            break;
        case 2: // Down
            symbol = 'v';
            break;
        case 3: // Left
            symbol = '<';
            break;
    }
    body.push_back(new sista::Pawn(symbol, old_head_coords, sandWormBodyStyle));
    field->addPrintPawn(body.back());
    // Consider that the body has a maximum size
    if (body.size() > SAND_WORM_SIZE) {
        field->erasePawn(body.front());
        delete body.front();
        body.erase(body.begin());
    }
}
ANSI::Settings SandWorm::sandWormBodyStyle = ANSI::Settings(
    ANSI::ForegroundColor::F_GREEN,
    ANSI::BackgroundColor::B_BLACK,
    ANSI::Attribute::BRIGHT
);
ANSI::Settings SandWorm::sandWormHeadStyle = ANSI::Settings(
    ANSI::ForegroundColor::F_GREEN,
    ANSI::BackgroundColor::B_BLACK,
    ANSI::Attribute::RAPID_BLINK
);