//Programmer: Christopher Hercules 
//Purpose: create a small stealth game

#include<iostream>
#include<string>
#include<cctype>
#include<sstream>
using namespace std;

//storing row and coloum positions 
struct Position
{
    int r, c;
    bool operator == (const Position &other) const
    {
        return r == other.r && c == other.c;
    }
};

//storing door and switches 
struct Door
{
    Position pos;
    int group;
    bool open;
};

struct Switch
{
    Position pos;
    int group;
};

//storing every part of the level
struct Level
{
    string name;
    char **map;
    int rows, cols;
    Position player;
    Position goal;
    Position *guards;
    char *guardDirection;
    bool *guardPatrol;
    int guardCount;
    Door *doors;
    int doorCount;
    Switch *switches;
    int switchCount;
};

//creating the map
char ** createMap(int rows, int cols)
{
    char **m = new char*[rows];

    for (int i = 0; i < rows; i++)
    {
        m[i] = new char[cols];
    }
    return m;
}
//freeing up space 
void deleteMap(char **map, int rows)
{
    for (int i = 0; i < rows; i++)
    {
        delete[] map[i];
    }
    delete[] map;
}

//dealing with capalization
string toLower(string s)
{
    for (char &c : s)
    {
        c = tolower(c);
    }
    return s;
}

//which character is a wall
bool isWall(char c)
{
    return c == '#';
}

//where the doors are at
int doorAt(Level &lvl, int row, int col)
{
    for (int i = 0; i < lvl.doorCount; i++)
    {
        if(lvl.doors[i].pos.r == row && lvl.doors[i].pos.c == col)
        {
            return i;
        }
    }
    return -1;
}

//where the switches are at
int switchAt(Level &lvl, int row, int col)
{
    for (int i = 0; i < lvl.switchCount; i++)
    {
        if(lvl.switches[i].pos.r == row && lvl.switches[i].pos.c == col)
        {
            return i;
        }
    }
    return -1;
}

//door being closed 
bool closedDoor(Level &lvl, int row, int col)
{
    int index = doorAt(lvl, row, col);
    return index != -1 &&
            !lvl.doors[index].open;
}

void toggleDoor(Level &lvl, int group)
{
    for (int i = 0; i < lvl.doorCount; i++)
    {
        if(lvl.doors[i].group == group)
        {
            lvl.doors[i].open = !lvl.doors[i].open;
        }
    }
}

//switch activating
void switchActivated(Level &lvl, Position pos)
{
    int index = switchAt(lvl, pos.r, pos.c);

    if (index != -1)
    {
        cout << "Switch Group " << lvl.switches[index].group << "activated." << endl;

        toggleDoor(lvl, lvl.switches[index].group);
    }
}

//displaying the current level
void draw(Level &lvl)
{
    for (int r = 0; r < lvl.rows; r++)
    {
        for (int c = 0; c < lvl.cols; c++)
        {
            bool printed = false;

            if (lvl.player.r == r && lvl.player.c == c)
            {
                cout << '@';
                continue;
            }

            if (lvl.goal.r == r && lvl.goal.c == c)
            {
                cout << '$';
                continue;
            }

            for (int i = 0; i < lvl.guardCount; i++)
            {
                if (lvl.guards[i].r == r && lvl.guards[i].c == c)
                {
                    cout << lvl.guardDirection[i];
                    printed = true;
                    break;
                }
            }

            if (!printed)
            {
                int doorIndex = doorAt(lvl, r, c);
                if (doorIndex != -1 && !lvl.doors[doorIndex].open)
                {
                    cout << 'D';
                    printed = true;
                }
            }

            if (!printed)
            {
                int switchIndex = switchAt(lvl, r, c);
                if (switchIndex != -1)
                {
                    cout << 'S';
                    printed = true;
                }
            }

            if (!printed)
            {
                cout << lvl.map[r][c];
            }
        }
        cout << endl;
    }
}

//if the position moving to is blocked 
bool positionBlocked(Level &lvl, int row, int col)
{
    if (row < 0 || row >= lvl.rows ||
        col < 0 || col >= lvl.cols)
    {
        return true;
    }

    if (lvl.map[row][col] == '#')
    {
        return true;
    }

    if (closedDoor(lvl, row, col))
    {
        return true;
    }

    return false;
}

//moving the player 
bool movePlayer(Level &lvl, char input)
{
    input = tolower(input);

    int dr = 0;
    int dc = 0;

    if (input == 'w')
    {
        dr = -1;
    }
    else if (input == 's')
    {
        dr = 1;
    }
    else if (input == 'a')
    {
        dc = -1;
    }
    else if (input == 'd')
    {
        dc = 1;
    }
    else
    {
        return false;
    }

    int nr = lvl.player.r + dr;
    int nc = lvl.player.c + dc;

    if (positionBlocked(lvl, nr, nc))
    {
        return false;
    }

    lvl.player.r = nr;
    lvl.player.c = nc;

    switchActivated(lvl, lvl.player);

    return true;
}

//checking for a wall 
bool vision(Level &lvl, int r, int c, int dr, int dc)
{
    if (r < 0 || r >= lvl.rows || c < 0 || c >= lvl.cols)
    {
        return false; 
    }

    if (isWall(lvl.map[r][c]))
    {
        return false;
    }

    if (closedDoor(lvl, r, c))
    {
        return false;
    }

    for (int i = 0; i < lvl.guardCount; i++)
    {
        if (lvl.guards[i].r == r &&
            lvl.guards[i].c == c)
        {
            return false;
        }
    }

    if (lvl.player.r == r && lvl.player.c == c)
    {
        return true;
    }

    return vision(lvl, r + dr, c + dc, dr, dc);
}

//whether the guard is seeing the player 
bool guardSees(Level &lvl, int i)
{
    int r = lvl.guards[i].r;
    int c = lvl.guards[i].c;
    char d = lvl.guardDirection[i];

    if (d == '^')
    {
        return vision(lvl, r -1, c, -1, 0);
    }
    if (d == 'v')
    {
        return vision(lvl, r + 1, c, 1, 0);
    }
    if (d == '<')
    {
        return vision(lvl, r, c -1, 0, -1);
    }
    return vision(lvl, r, c + 1, 0, 1);
}

//checking every guard to see if they see the player
bool playerSeen(Level &lvl)
{
    for (int i = 0; i < lvl.guardCount; i++)
    {
        if (guardSees(lvl, i))
        {
            return true;
        }
    }
    return false;
}

//player htis guard
bool playerHitGuard(Level &lvl)
{
    for (int i = 0; i < lvl.guardCount; i++)
    {
        if (lvl.player == lvl.guards[i])
        {
            return true;
        }
    }
    return false;
}

//if the player won
bool playerWon(Level &lvl)
{
    return lvl.player == lvl.goal;
}

//changing the direction
void reverseDir(char &d)
{
    if (d == '^')
    {
        d = 'v';
    }
    else if (d == 'v')
    {
        d = '^';
    }
    else if (d == '<')
    {
        d = '>';
    }
    else
    {
        d = '<';
    }
}

//clockwise direction for patrol guard 
void clockwiseDir(char &d)
{
    if (d == '^')
    {
        d = '>';
    }
    else if (d == '>')
    {
        d = 'v';
    }
    else if (d == 'v')
    {
        d = '<';
    }
    else if (d == '<')
    {
        d = '^';
    }
}

//moving the guard based on position  
void moveGuards(Level &lvl)
{
    for (int i = 0; i < lvl.guardCount; i++)
    {
        int dr = 0;
        int dc = 0;

        if (lvl.guardDirection[i] == '^')
        {
            dr = -1;
        }
        else if (lvl.guardDirection[i] == 'v')
        {
            dr = 1;
        }
        else if (lvl.guardDirection[i] == '<')
        {
            dc = -1;
        }
        else if (lvl.guardDirection[i] == '>') 
        {
            dc = 1;
        }

        int nr = lvl.guards[i].r + dr;
        int nc = lvl.guards[i].c + dc;

        if (positionBlocked(lvl, nr, nc))
        {
            if (lvl.guardPatrol[i])
            {
                clockwiseDir(lvl.guardDirection[i]);
            }
            else 
            {
                reverseDir(lvl.guardDirection[i]);
            }

            dr = 0;
            dc = 0;

            if(lvl.guardDirection[i] == '^')
            {
                dr = -1;
            }
            else if(lvl.guardDirection[i] == 'v')
            {
                dr = 1;
            }
            else if(lvl.guardDirection[i] == '<')
            {
                dc = -1;
            }
            else if(lvl.guardDirection[i] == '>')
            {
                dc = 1;
            }
            
            nr = lvl.guards[i].r + dr;
            nc = lvl.guards[i].c + dc;
        }

        if(!positionBlocked(lvl, nr, nc))
        {
            lvl.guards[i].r = nr;
            lvl.guards[i].c = nc;
            switchActivated(lvl, lvl.guards[i]);
        }
    }
}

//creating the levels
Level makeLevel1()
{
    Level lvl;
    lvl.name = "A New Enemy";
    lvl.rows = 6; 
    lvl.cols = 8;

    lvl.map = createMap(lvl.rows, lvl.cols);

    string raw[6] = {
        "########",
        "#      #",
        "#   #  #",
        "#   #  #",
        "#   # $#",
        "########"
    };

    for (int r = 0; r < lvl.rows; r++)
    {
        for (int c = 0; c < lvl.cols; c++)
        {
            lvl.map[r][c] = raw[r][c];
        }
    }

    lvl.player = {4,1};
    lvl.goal = {4,6};
    lvl.guardCount = 2;
    lvl.guards = new Position[2];
    lvl.guardDirection = new char[2];
    lvl.guardPatrol = new bool[2];
    lvl.guardPatrol[0] = false;
    lvl.guardPatrol[1] = true;
    lvl.guards[0] = {1, 5};
    lvl.guardDirection[0] = 'v';
    lvl.guards[1] = {2,1};
    lvl.guardDirection[1] = '>';

    lvl.doorCount = 2;
    lvl.doors = new Door[lvl.doorCount];
    lvl.doors[0].pos = {1, 3};
    lvl.doors[0].group = 1;
    lvl.doors[0].open = false;
    lvl.doors[1].pos = {3, 2};
    lvl.doors[1].group = 1;
    lvl.doors[1].open = false;

    lvl.switchCount = 1;
    lvl.switches = new Switch[lvl.switchCount];
    lvl.switches[0].pos = {4, 2};
    lvl.switches[0].group = 1;

    return lvl;
}

Level makeLevel2()
{
    Level lvl;
    lvl.name = "Survive";
    lvl.rows = 6;
    lvl.cols = 8;

    lvl.map = createMap(lvl.rows, lvl.cols);

    string raw[6] = {
        "########",
        "#      #",
        "# ###  #",
        "#   #  #",
        "#    $ #",
        "########"
    };

    for (int r = 0; r < lvl.rows; r++)
    {
        for (int c = 0; c < lvl.cols; c++)
        {
            lvl.map[r][c] = raw[r][c];
        }
    }

    lvl.player = {1, 1};
    lvl.goal = {4, 5};
    lvl.guardCount = 1;
    lvl.guards = new Position[1];
    lvl.guardDirection = new char[1];
    lvl.guards[0] = {4, 2};
    lvl.guardDirection[0] = '^';
    lvl.guardPatrol = new bool[1];
    lvl.guardPatrol[0] = false;

    lvl.doorCount = 0;
    lvl.doors = nullptr;
    lvl.switchCount = 0;
    lvl.switches = nullptr;

    return lvl;
}

Level makeLevel3()
{
    Level lvl;
    lvl.name = "No Limit";
    lvl.rows = 8;
    lvl.cols = 12;

    lvl.map = createMap(lvl.rows, lvl.cols);

    string raw[8] = 
    {
        "############",
        "#          #",
        "#  ######  #",
        "#       #  #",
        "#  #       #",
        "#  ######  #",
        "#          #",
        "############"
    };

    for (int r = 0; r < lvl.rows; r++)
    {
        for (int c = 0; c < lvl.cols; c++)
        {
            lvl.map[r][c] = raw[r][c];
        }
    }

    lvl.player = {1, 1};
    lvl.goal = {6, 10};
    lvl.guardCount = 2;
    lvl.guards = new Position[lvl.guardCount];
    lvl.guardDirection = new char[lvl.guardCount];
    lvl.guardPatrol = new bool[lvl.guardCount];

    lvl.guards[0] = {1, 3};
    lvl.guardDirection[0] = '>';
    lvl.guardPatrol[0] = true;
    lvl.guards[1] = {6, 5};
    lvl.guardDirection[1] = '>';
    lvl.guardPatrol[1] = false;

    lvl.doorCount = 0;
    lvl.doors = nullptr;
    lvl.switchCount = 0;
    lvl.switches = nullptr;

    return lvl;
}

Level makeLevel4() 
{ 
    Level lvl; 
    lvl.name = "Switch Operation"; 
    lvl.rows = 8; 
    lvl.cols = 12; 

    lvl.map = createMap(lvl.rows, lvl.cols); 

    string raw[8] = 
    { 
        "############", 
        "#          #", 
        "#   ####   #", 
        "#          #", 
        "#   ####   #", 
        "#          #", 
        "#          #", 
        "############" 
    }; 

    for (int r = 0; r < lvl.rows; r++) 
    { 
        for (int c = 0; c < lvl.cols; c++) 
        { 
            lvl.map[r][c] = raw[r][c]; 
        } 
    } 
    lvl.player = {1, 1}; 
    lvl.goal = {6, 10}; 

    lvl.guardCount = 1; 
    lvl.guards = new Position[lvl.guardCount]; 
    lvl.guardDirection = new char[lvl.guardCount]; 
    lvl.guardPatrol = new bool[lvl.guardCount]; 
    lvl.guards[0] = {5, 2}; 
    lvl.guardDirection[0] = '>'; 
    lvl.guardPatrol[0] = false;

    lvl.doorCount = 4;
    lvl.doors = new Door[lvl.doorCount]; 
    lvl.doors[0].pos = {1, 5}; 
    lvl.doors[0].group = 1; 
    lvl.doors[0].open = false; 
    lvl.doors[1].pos = {3, 5}; 
    lvl.doors[1].group = 1; 
    lvl.doors[1].open = false; 
    lvl.doors[2].pos = {4, 8}; 
    lvl.doors[2].group = 2; 
    lvl.doors[2].open = false; 
    lvl.doors[3].pos = {6, 8}; 
    lvl.doors[3].group = 2; 
    lvl.doors[3].open = false; 

    lvl.switchCount = 2; 
    lvl.switches = new Switch[lvl.switchCount]; 
    lvl.switches[0].pos = {2, 2}; 
    lvl.switches[0].group = 1; 
    lvl.switches[1].pos = {5, 5}; 
    lvl.switches[1].group = 2; 
    return lvl; 
} 

//ispect the levels and tell info about it and other parts of it
void inspect(Level &lvl)
{
    cout << "Enter row and column. (2 4 or 2,4): ";

    string input;
    getline(cin >> ws, input);

    for (char &c : input)
    {
        if (c == ',')
        {
            c = ' ';
        }
    }

    stringstream parser(input);
    int row; 
    int col;
    char extra;

    if (!(parser >> row >> col) || parser >> extra)
    {
        cout << "Invalid coordinates. Enter two numbers(2 4 or 2,4.)" << endl;
        return;
    }
    

    if (row < 0 || row >= lvl.rows || col < 0 || col >= lvl.cols)
    {
        cout << "That position is outside the map" << endl;
        return;
    }

    if (lvl.player.r == row && lvl.player.c == col)
    {
        cout << "This is your current position" << endl;
        return;
    }

    if (lvl.goal.r == row && lvl.goal.c == col)
    {
        cout << "That is the goal, reach to complete mission." << endl;
        return;
    }

    for (int i = 0; i < lvl.guardCount; i++)
    {
        if (lvl.guards[i].r == row && lvl.guards[i].c == col)
        {
            cout << "Guard Facing " << lvl.guardDirection[i] << "." << endl;

            if (lvl.guardPatrol[i])
            {
                cout << "Type: Patrol Guard." << endl;
                cout << "It moves clockwise." << endl;
            }
            else
            {
                cout << "Type: Normal Guard." << endl;
                cout << "It reverses its direction when blcoked." << endl;
            }
            return;
        }
    }

    int doorIndex = doorAt(lvl, row, col);
    if (doorIndex != -1)
    {
        cout << "Door Group: " << lvl.doors[doorIndex].group << endl;

        if (lvl.doors[doorIndex].open)
        {
            cout << "The Door is open." << endl;
        }
        else 
        {
            cout << "The Door is closed." << endl;
        }
        return;
    }

    int switchIndex = switchAt(lvl, row, col);

    if (switchIndex != -1)
    {
        cout << "Switch group: " << lvl.switches[switchIndex].group << endl;
        return;
    }

    if(isWall(lvl.map[row][col]))
    {
        cout << "This wall blocks movement and vision." << endl;
        return;
    }

    cout << "Empty floor tile." << endl;
}

//deleting the levels for space 
void deleteLevel(Level &lvl)
{
    deleteMap(lvl.map, lvl.rows); 
    delete[] lvl.guards;
    delete[] lvl.guardDirection;
    delete[] lvl.guardPatrol;
    delete[] lvl.doors;
    delete[] lvl.switches;
}

//the main function
int main()
{
    int levelCount = 4;
    Level *levels = new Level[levelCount];
    levels[0] = makeLevel1();
    levels[1] = makeLevel2();
    levels[2] = makeLevel3();
    levels[3] = makeLevel4();

    //menu
    bool running = true;
    while(running)
    {
        int choice = -1;

        while (choice == -1)
        {
            cout << "Welcome to Ultra Spy" << endl;
            cout << "Select a Level: " << endl;

            for (int i = 0; i < levelCount; i++)
            {
                cout << i + 1 << ") " << levels[i].name << endl;
            }

            cout << "5) Quit " << endl;
            cout << "> ";

            string input; 
            getline(cin >> ws, input);
            input = toLower(input);

            for (int i = 0; i < levelCount; i++)
            {
                if (input == to_string(i + 1) || input == toLower(levels[i].name))
                {
                    choice = i;
                }
            }

            if (input == "5" || input == "quit")
            {
                running = false;
                break;
            }

            if (choice == -1)
            {
                cout << "Invalid choice. Try Again" << endl;
            }
        } 
        if (!running)
        {
            break;
        }   

        //game loop
        deleteLevel(levels[choice]);
        if (choice == 0)
        {
            levels[choice] = makeLevel1();
        }
        else if (choice == 1)
        {
            levels[choice] = makeLevel2();
        }
        else if (choice == 2)
        {
            levels[choice] = makeLevel3();
        }
        else if (choice == 3)
        {
            levels[choice] = makeLevel4();
        }
        Level &lvl = levels[choice];

        while(true)
        {
            draw(lvl);
            if(playerSeen(lvl))
            {
                cout << "You were spotted!!!";
                break;
            }

            if(playerWon(lvl))
            {
                cout << "Mission Complete!!!";
                break;
            }

            cout << "Move(WASD) or type Inspect: ";

            string move;
            getline(cin >> ws, move);
            move = toLower(move);

            if (move == "inspect")
            {
                inspect(lvl);
                continue;
            }

            if (move.length() != 1)
            {
                cout << "Move(WASD) or inspect." << endl;
                continue;
            }

            char m = move[0];

            if (!movePlayer(lvl, m))
            {
                cout << "You cannot move there." << endl;
                continue;
            }

            if (playerHitGuard(lvl))
            {
                draw(lvl);
                cout << "You ran into a guard!" << endl;
                break;
            }

            moveGuards(lvl);

            if (playerHitGuard(lvl))
            {
                draw(lvl);
                cout << "A guard caught you!" << endl;
                break;
            }

            if (playerSeen(lvl))
            {
                draw(lvl);
                cout << "You've been Caught!";
                break;
            }

            if (playerWon(lvl))
            {
                draw(lvl);
                cout << "Mission Complete!!!";
                break;
            }
        }

        //play again
        cout << "Play Again? (y/n): ";
        string again;
        getline(cin >> ws, again);
        again = toLower(again);

        if (again != "y")
        {
            running = false;
        }
    }

    //cleaing up memory
    for (int i = 0; i < levelCount; i++)
    {
        deleteLevel(levels[i]);
    }

    delete[] levels;
    return 0;
}