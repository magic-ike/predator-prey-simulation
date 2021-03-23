#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

const int GRID_SIZE = 20;
const int INITIAL_ANT_COUNT = 100;
const int NUMBER_OF_ANT_STEPS_TO_BREED = 3;
const int INITIAL_DOODLEBUG_COUNT = 5;
const int NUMBER_OF_DOODLEBUG_STEPS_TO_BREED = 8;
const int NUMBER_OF_DOODLEBUG_STEPS_TO_STARVE = 3;

class World;

class Organism {
public:
    Organism(int newX, int newY): x(newX), y(newY), stepsSinceLastBreeding(0) {};
    virtual void move(World* world) = 0;
    int getX() const { return x; };
    int getY() const { return y; };
protected:
    int x, y;
    int stepsSinceLastBreeding;
    virtual void breed(World* world) = 0;
};


class Ant: public Organism {
public:
    Ant(int newX, int newY): Organism(newX, newY) {};
    virtual void move(World* world);
private:
    virtual void breed(World* world);
};


class Doodlebug: public Organism {
public:
    Doodlebug(int newX, int newY): Organism(newX, newY), stepsSinceLastMeal(0) {};
    virtual void move(World* world);
private:
    int stepsSinceLastMeal;
    virtual void breed(World* world);
    void starve(World* world);
};


class World {
public:
    World();
    ~World();
    void drawWorld() const;
    void runTimeStep();
    char getGridCell(int x, int y) const { return grid[x][y]; };
    void setGridCell(int x, int y, char c) { grid[x][y] = c; };
    void spawnNewAnt(int x, int y);
    void removeDeadAnt(int x, int y);
    void spawnNewDoodlebug(int x, int y);
    void removeDeadDoodlebug(int x, int y);
private:
    char* grid[GRID_SIZE];
    vector<Ant> ants;
    vector<Doodlebug> doodlebugs;
};

World::World() {
    //initializes the grid
    for (int i = 0; i < GRID_SIZE; i++) {
        grid[i] = new char[GRID_SIZE];
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = '-';
        }
    }

    //spawns 100 ants
    int i = 0;
    while (i < INITIAL_ANT_COUNT) {
        int x = (rand() % GRID_SIZE) + 1;
        int y = (rand() % GRID_SIZE) + 1;

        if (grid[x][y] == '-') {
            grid[x][y] = 'o';
            ants.push_back(Ant(x, y));
            i++;
        }
    }

    //spawns 5 doodlebugs
    i = 0;
    while (i < INITIAL_DOODLEBUG_COUNT) {
        int x = (rand() % GRID_SIZE) + 1;
        int y = (rand() % GRID_SIZE) + 1;

        if (grid[x][y] == '-') {
            grid[x][y] = 'X';
            doodlebugs.push_back(Doodlebug(x, y));
            i++;
        }
    }
}

World::~World() {
    for (int i = 0; i < GRID_SIZE; i++) {
        delete [] grid[i];
        grid[i] = nullptr;
    }
}

void World::drawWorld() const {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            cout << grid[j][i] << "\t";
        }
        cout << endl;
    }
}

void World::runTimeStep() {
    for (int i = 0; i < doodlebugs.size(); i++) {
        doodlebugs[i].move(this);
    }
    for (int i = 0; i < ants.size(); i++) {
        ants[i].move(this);
    }
}

void World::spawnNewAnt(int x, int y) {
    grid[x][y] = 'o';
    ants.push_back(Ant(x, y));
}

void World::removeDeadAnt(int x, int y) {
    grid[x][y] = '-';
    for (int i = 0; i < ants.size(); i++) {
        if (ants[i].getX() == x && ants[i].getY() == y) {
            ants.erase(ants.begin() + i);
            break;
        }
    }
}

void World::spawnNewDoodlebug(int x, int y) {
    grid[x][y] = 'X';
    doodlebugs.push_back(Doodlebug(x, y));
}

void World::removeDeadDoodlebug(int x, int y) {
    grid[x][y] = '-';
    for (int i = 0; i < doodlebugs.size(); i++) {
        if (doodlebugs[i].getX() == x && doodlebugs[i].getY() == y) {
            doodlebugs.erase(doodlebugs.begin() + i);
            break;
        }
    }
}


void Ant::move(World *world) {
    int direction = (rand() % 4) + 1;
    int oldX = x, newX = x;
    int oldY = y, newY = y;

    switch (direction) {
        case 1: //move up
            newY = max(newY-1, 0);
            break;
        case 2: //move down
            newY = min(newY+1, GRID_SIZE-1);
            break;
        case 3: //move left
            newX = max(newX-1, 0);
            break;
        case 4: //move right
            newX = min(newX+1, GRID_SIZE-1);
            break;
    }
    if (world->getGridCell(newX, newY) == '-') {
        x = newX;
        y = newY;
        world->setGridCell(oldX, oldY, '-');
        world->setGridCell(newX, newY, 'o');
    }

    stepsSinceLastBreeding++;
    breed(world);
}

void Ant::breed(World *world) {
    if (stepsSinceLastBreeding == NUMBER_OF_ANT_STEPS_TO_BREED) {
        int yAbove = max(y-1, 0);
        int yBelow = min(y+1, GRID_SIZE-1);
        int xToTheLeft = max(x-1, 0);
        int xToTheRight = min(x+1, GRID_SIZE-1);

        if (world->getGridCell(x, yAbove) == '-') { //empty space above
            world->spawnNewAnt(x, yAbove);
        } else if (world->getGridCell(x, yBelow) == '-') { //empty space below
            world->spawnNewAnt(x, yBelow);
        } else if (world->getGridCell(xToTheLeft, y) == '-') { //empty space to the left
            world->spawnNewAnt(xToTheLeft, y);
        } else if (world->getGridCell(xToTheRight, y) == '-') { //empty space to the right
            world->spawnNewAnt(xToTheRight, y);
        }
        stepsSinceLastBreeding = 0;
    }
}


void Doodlebug::move(World *world) {
    int oldX = x, newX = x;
    int oldY = y, newY = y;
    int yAbove = max(y-1, 0);
    int yBelow = min(y+1, GRID_SIZE-1);
    int xToTheLeft = max(x-1, 0);
    int xToTheRight = min(x+1, GRID_SIZE-1);
    bool antEaten = false;

    if (yAbove >= 0 && world->getGridCell(x, yAbove) == 'o') { //ant above
        newY = yAbove;
        antEaten = true;
    } else if (yBelow <= GRID_SIZE-1 && world->getGridCell(x, yBelow) == 'o') { //ant below
        newY = yBelow;
        antEaten = true;
    } else if (xToTheLeft >= 0 && world->getGridCell(xToTheLeft, y) == 'o') { //ant to the left
        newX = xToTheLeft;
        antEaten = true;
    } else if (xToTheRight <= GRID_SIZE-1 && world->getGridCell(xToTheRight, y) == 'o') { //ant to the right
        newX = xToTheRight;
        antEaten = true;
    }

    if (antEaten) {
        world->removeDeadAnt(newX, newY);
        stepsSinceLastMeal = 0;
    } else {
        int direction = (rand() % 4) + 1;

        switch (direction) {
            case 1: //move up
                newY = max(newY-1, 0);
                break;
            case 2: //move down
                newY = min(newY+1, GRID_SIZE-1);
                break;
            case 3: //move left
                newX = max(newX-1, 0);
                break;
            case 4: //move right
                newX = min(newX+1, GRID_SIZE-1);
                break;
        }
        stepsSinceLastMeal++;
    }

    x = newX;
    y = newY;
    world->setGridCell(oldX, oldY, '-');
    world->setGridCell(newX, newY, 'X');

    stepsSinceLastBreeding++;
    breed(world);
    starve(world);
}

void Doodlebug::breed(World *world) {
    if (stepsSinceLastBreeding == NUMBER_OF_DOODLEBUG_STEPS_TO_BREED) {
        int yAbove = max(y-1, 0);
        int yBelow = min(y+1, GRID_SIZE-1);
        int xToTheLeft = max(x-1, 0);
        int xToTheRight = min(x+1, GRID_SIZE-1);

        if (world->getGridCell(x, yAbove) == '-') { //empty space above
            world->spawnNewDoodlebug(x, yAbove);
        } else if (world->getGridCell(x, yBelow) == '-') { //empty space below
            world->spawnNewDoodlebug(x, yBelow);
        } else if (world->getGridCell(xToTheLeft, y) == '-') { //empty space to the left
            world->spawnNewDoodlebug(xToTheLeft, y);
        } else if (world->getGridCell(xToTheRight, y) == '-') { //empty space to the right
            world->spawnNewDoodlebug(xToTheRight, y);
        }
        stepsSinceLastBreeding = 0;
    }
}

void Doodlebug::starve(World *world) {
    if (stepsSinceLastMeal == NUMBER_OF_DOODLEBUG_STEPS_TO_STARVE) {
        world->removeDeadDoodlebug(x, y);
    }
}


int main() {
    srand(time(0));

    cout << "This is a two-dimensional predator-prey simulation.\n";
    cout << "In this simulation, ants (o) are the prey and doodlebugs (X) are the predators.\n";
    cout << "Press 'Enter' to move to the next time step (epoch).\n";
    cout << "Type anything before pressing 'Enter' to end the simulation.\n\n";

    World world;
    char input;
    int epoch = 1;

    cout << "EPOCH " << epoch << endl;
    world.drawWorld();
    cin.get(input);

    while (input == '\n') {
        world.runTimeStep();
        cout << "EPOCH " << ++epoch << endl;
        world.drawWorld();
        cin.get(input);
    }

    return 0;
}
