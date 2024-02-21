#include "dune.hpp"
#include "cross_platform.hpp"
#include <thread>
#include <chrono>

#if DEBUG
    #include <fstream>
    std::ofstream debug("debug.log");
#endif

sista::Field* field;
sista::Border* border;
std::vector<SandWorm*> sandWorms;
std::vector<Bullet*> bullets;
sista::Pawn* miner;
bool finished = false;


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
    bool won = false, alarm = false, bullet = false;
    Direction bullet_direction = UP;
    std::thread th = std::thread([&won, &bullet, &bullet_direction]() {
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
            // Prevent the miner from moving too fast or shooting too fast
            std::this_thread::sleep_for(std::chrono::milliseconds(COOLDOWN));
            if (input == 'i' || input == 'I') {
                bullet = true;
                bullet_direction = UP;
                continue;
            } else if (input == 'l' || input == 'L') {
                bullet = true;
                bullet_direction = RIGHT;
                continue;
            } else if (input == 'k' || input == 'K') {
                bullet = true;
                bullet_direction = DOWN;
                continue;
            } else if (input == 'j' || input == 'J') {
                bullet = true;
                bullet_direction = LEFT;
                continue;
            }
            sista::Coordinates coords = miner->getCoordinates();
            sista::Coordinates new_coords = coords;
            if (input == 'w' || input == 'W') {
                new_coords.y--;
            } else if (input == 'd' || input == 'D') {
                new_coords.x++;
            } else if (input == 's' || input == 'S') {
                new_coords.y++;
            } else if (input == 'a' || input == 'A') {
                new_coords.x--;
            }
            // Check if the new position is inside the safe area
            if (
                new_coords.x >= (WIDTH - INTERNAL_WIDTH) / 2 + 1 &&
                new_coords.x < (WIDTH + INTERNAL_WIDTH) / 2 - 1 &&
                new_coords.y >= (HEIGHT - INTERNAL_HEIGHT) / 2 + 1 &&
                new_coords.y < (HEIGHT + INTERNAL_HEIGHT) / 2 - 1
            ) {
                if (field->isFree(new_coords)) {
                    field->movePawnFromTo(coords, new_coords);
                }
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
        // A new sand worm may spawn
        if (rand() % SAND_WORMS_SPAWN_RATE == 0) {
            sandWorms.push_back(new SandWorm());
        }
        // Check if all worms are dead
        if (sandWorms.size() == 0) {
            won = true;
            finished = true;
            break;
        }
        #if BULLETS_WITHOUT_ALARM
        // Move the bullets
        for (std::vector<Bullet*>::iterator it = bullets.begin(); it != bullets.end(); it++) {
            #if DEBUG
            debug << "Moving bullet in " << (*it)->getCoordinates().y << ", " << (*it)->getCoordinates().x << std::endl;
            #endif
            if ((*it) != nullptr)
                (*it)->move();
        }
        for (std::vector<Bullet*>::iterator it = bullets.begin(); it != bullets.end(); it++) {
            if ((*it) == nullptr) {
                bullets.erase(it);
                it--;
            }
        }
        // Check if a bullet was shot
        if (bullet) {
            sista::Coordinates coords = miner->getCoordinates();
            coords = coords + directionMap[bullet_direction];
            if (field->isFree(coords)) {
                new Bullet(miner, bullet_direction);
            }
            bullet = false;
        }
        #endif
        // Move the sand worms
        for (SandWorm* sandWorm : sandWorms) {
            sandWorm->move();
        }
        std::cout << std::flush;
        if (!alarm) { // Unless the alarm was already triggered
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
                    cursor.set(sista::Coordinates(HEIGHT + 3, WIDTH / 4));
                    ANSI::setForegroundColor(ANSI::ForegroundColor::F_RED);
                    std::cout << "The sand worms are coming!\n";
                    cursor.set(sista::Coordinates(HEIGHT + 4, WIDTH / 4 - 2));
                    std::cout << "You must be ready to run away! [Q]\n";
                    #if !BULLETS_ON_ALARM
                    cursor.set(sista::Coordinates(HEIGHT + 5, WIDTH / 4 - 2));
                    std::cout << "Your weapons are now ineffective!\n";
                    #endif
                    std::this_thread::sleep_for(std::chrono::milliseconds(FRAME));
                    while (true) {
                        counter++;
                        cursor.set(sista::Coordinates(HEIGHT + 2, WIDTH / 3));
                        std::cout << "Melange: " << counter << std::flush;
                        // A new sand worm may spawn
                        if (rand() % SAND_WORMS_SPAWN_RATE == 0) {
                            sandWorms.push_back(new SandWorm());
                        }
                        // Check if all worms are dead
                        if (sandWorms.size() == 0) {
                            won = true;
                            finished = true;
                            break;
                        }
                        #if BULLETS_ON_ALARM
                        // Move the bullets
                        for (std::vector<Bullet*>::iterator it = bullets.begin(); it != bullets.end(); it++) {
                            #if DEBUG
                            debug << "Moving bullet in " << (*it)->getCoordinates().y << ", " << (*it)->getCoordinates().x << std::endl;
                            #endif
                            if ((*it) != nullptr)
                                (*it)->move();
                        }
                        for (std::vector<Bullet*>::iterator it = bullets.begin(); it != bullets.end(); it++) {
                            if ((*it) == nullptr) {
                                bullets.erase(it);
                                it--;
                            }
                        }
                        // Check if a bullet was shot
                        if (bullet) {
                            sista::Coordinates coords = miner->getCoordinates();
                            coords = coords + directionMap[bullet_direction];
                            if (field->isFree(coords)) {
                                new Bullet(miner, bullet_direction);
                            }
                            bullet = false;
                        }
                        #endif
                        // Move the sand worms
                        for (SandWorm* sandWorm : sandWorms) {
                            sandWorm->move();
                            if (finished) {
                                break;
                            }
                        }
                        if (finished) {
                            break;
                        }
                        std::cout << std::flush;
                        std::this_thread::sleep_for(std::chrono::milliseconds(FRAME));
                    }
                    finished = true;
                    break;
                }
            }
        }
        // Wait a bit
        if (finished) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(FRAME));
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
SandWorm::~SandWorm() {}
void SandWorm::move() {
    // Check a random direction until it finds a valid one
    sista::Coordinates old_head_coords = head->getCoordinates();
    sista::Coordinates new_head_coords = old_head_coords;
    int trials = 7;
    int direction;
    do {
        sista::Pawn* pawn = field->getPawn(new_head_coords);
        if (pawn != nullptr) {
            if (pawn == miner) {
                finished = true;
                return;
            }
        }
        new_head_coords = old_head_coords;
        direction = rand() % 4;
        switch (direction) {
            case 0: // Up
                if (new_head_coords.y > 0) {
                    new_head_coords.y--;
                }
                break;
            case 1: // Right
                if (new_head_coords.x < WIDTH - 1) {
                    new_head_coords.x++;
                }
                break;
            case 2: // Down
                if (new_head_coords.y < HEIGHT - 1) {
                    new_head_coords.y++;
                }
                break;
            case 3: // Left
                if (new_head_coords.x > 0) {
                    new_head_coords.x--;
                }
                break;
        }
        if (trials <= 0) {
            // The worm is stuck
            for (sista::Pawn* pawn : body) {
                if (pawn != nullptr && rand() % 2 == 0) {
                    body.erase(std::find(body.begin(), body.end(), pawn));
                    field->erasePawn(pawn);
                }
                // delete pawn;
            }
            trials = 7;
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
void SandWorm::erase() {
    field->erasePawn(head);
    // delete head;
    for (sista::Pawn* pawn : body) {
        field->erasePawn(pawn);
        // delete pawn;
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


std::unordered_map<Direction, sista::Coordinates> directionMap = {
    {UP, sista::Coordinates(-1, 0)},
    {RIGHT, sista::Coordinates(0, 1)},
    {DOWN, sista::Coordinates(1, 0)},
    {LEFT, sista::Coordinates(0, -1)}
};
std::unordered_map<Direction, char> directionSymbol = {
    {UP, '^'}, {RIGHT, '>'}, {DOWN, 'v'}, {LEFT, '<'}
};


Bullet::Bullet(sista::Pawn* shooter, Direction direction_) : sista::Pawn('^', sista::Coordinates(0, 0), bulletStyle), direction(direction_) {
    sista::Coordinates coords = shooter->getCoordinates();
    switch (direction) {
        case UP:
            coords.y--;
            break;
        case RIGHT:
            coords.x++;
            break;
        case DOWN:
            coords.y++;
            break;
        case LEFT:
            coords.x--;
            break;
    }
    this->setCoordinates(coords);
    this->settings = bulletStyle;
    this->symbol = directionSymbol[direction];
    bool free = field->isFree(coords);
    if (free) {
        field->addPrintPawn(this);
        bullets.push_back(this);
    } else {
        delete this;
    }
}
Bullet::~Bullet() {
    #if DEBUG
    debug << "\tDeleting bullet in " << this->getCoordinates().y << ", " << this->getCoordinates().x << std::endl;
    #endif
    std::vector<Bullet*>::iterator it = std::find(bullets.begin(), bullets.end(), this);
    *it = nullptr;
    #if DEBUG
    debug << "\tNulling bullet in the vector" << std::endl;
    #endif
    field->erasePawn(this);
    #if DEBUG
    debug << "\tErasing bullet from the field" << std::endl;
    #endif
}
void Bullet::move() {
    sista::Coordinates coords = this->getCoordinates();
    sista::Coordinates new_coords = coords + directionMap[direction];
    if (field->isOutOfBounds(new_coords)) {
        #if DEBUG
        debug << "\tThe bullet is out of bounds" << std::endl;
        #endif
        delete this;
        #if DEBUG
        debug << "\tThe bullet was deleted" << std::endl;
        #endif
        return;
    }
    #if DEBUG
    debug << "\tMoving bullet from " << coords.y << ", " << coords.x << " to " << new_coords.y << ", " << new_coords.x << std::endl;
    #endif
    bool free = field->isFree(new_coords);
    if (free) {
        #if DEBUG
        debug << "\tThe bullet is free to move" << std::endl;
        #endif
        field->movePawn(this, new_coords);
    } else { // We may have hit something
        #if DEBUG
        debug << "\tThe bullet hit something" << std::endl;
        #endif
        sista::Pawn* pawn = field->getPawn(new_coords);
        if (pawn != nullptr) {
            for (SandWorm* sandWorm : sandWorms) {
                if (pawn == sandWorm->head) { // If you hit a head...
                    #if DEBUG
                    debug << "\tThe bullet hit a sand worm head" << std::endl;
                    #endif
                    sandWorms.erase(std::find(sandWorms.begin(), sandWorms.end(), sandWorm));
                    sandWorm->erase(); // ...the whole worm is destroyed
                    delete this;
                    #if DEBUG
                    debug << "\tThe bullet was deleted" << std::endl;
                    #endif
                    return;
                }
                for (sista::Pawn* body_part : sandWorm->body) { // If you hit a body part...
                    if (pawn == body_part) { // ...only that body part is destroyed
                        #if DEBUG
                        debug << "\tThe bullet hit a sand worm body part" << std::endl;
                        #endif
                        sandWorm->body.erase(std::find(sandWorm->body.begin(), sandWorm->body.end(), body_part));
                        field->erasePawn(body_part);
                        delete this;
                        #if DEBUG
                        debug << "\tThe bullet was deleted" << std::endl;
                        #endif
                        return;
                    }
                }
            }
        }
        #if DEBUG
        debug << "\tThe bullet hit something else" << std::endl;
        #endif
        delete this;
        #if DEBUG
        debug << "\tThe bullet was deleted" << std::endl;
        #endif
    }
}
ANSI::Settings Bullet::bulletStyle = ANSI::Settings(
    ANSI::ForegroundColor::F_RED,
    ANSI::BackgroundColor::B_BLACK,
    ANSI::Attribute::BRIGHT
);