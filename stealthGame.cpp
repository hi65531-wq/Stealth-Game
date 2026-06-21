//Programmer: Christopher Hercules 
//Purpose: create a small stealth game

#include<iostream>
#include<string>
#include<cctype>
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
    int guardCount;
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

bool isWall(char c)
{
    return c == '#';
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
                cout << lvl.map[r][c];
            }
        }
        cout << endl;
    }
}

//moving the player 
bool movePlayer(Level &lvl, char input)
{
    input = tolower(input);

    int dr = 0, dc = 0;

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

    if (nr < 0 || nr > lvl.rows || nc < 0 || nc > lvl.cols)
    {
        return false;
    }
    if (isWall(lvl.map[nr][nc]))
    {
        return false;
    }

    lvl.player.r = nr;
    lvl.player.c = nc;
    return true;
}

//checking for a wall 
bool vision(Level &lvl, int r, int c, int dr, int dc)
{
    if (r < 0 || r > lvl.rows || c < 0 || c > lvl.cols)
    {
        return false; 
    }

    if (isWall(lvl.map[r][c]))
    {
        return false;
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

//moving the guard based on position  
void moveGuards(Level &lvl)
{
    for (int i = 0; i < lvl.guardCount; i++)
    {
        int dr = 0, dc = 0;

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
        else 
        {
            dc = 1;
        }

        int nr = lvl.guards[i].r + dr;
        int nc = lvl.guards[i].c + dc;

        if (nr < 0 || nr > lvl.rows || nc < 0 || nc > lvl.cols || isWall(lvl.map[nr][nc]))
        {
            reverseDir(lvl.guardDirection[i]);
            continue;
        }

        lvl.guards[i].r = nr;
        lvl.guards[i].c = nc;

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

    lvl.guards[0] = {1, 5};
    lvl.guardDirection[0] = 'v';
    lvl.guards[1] = {2,1};
    lvl.guardDirection[1] = '>';

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

    return lvl;
}

//the main function
int main()
{
    int levelCount = 2;
    Level *levels = new Level[levelCount];
    levels[0] = makeLevel1();
    levels[1] = makeLevel2();

    //menu
    cout << "Welcome to Ultra-Spy" << endl;
    cout << "Select a level: " << endl;

    for (int i = 0; i < levelCount; i++)
    {
        cout << i + 1 << ") " << levels[i].name << endl;
    }
    cout << ">";

    string input;
    cin >> input;
    input = toLower(input);

    int choice = 0;
    if (input == "2" || toLower(levels[1].name) == input)
    {
        choice = 1;
    }
    Level &lvl = levels[choice];

    //game loop
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

        cout << "Move(WASD): ";
        char move;
        cin >> move;

        if (movePlayer(lvl, move))
        {
            moveGuards(lvl);
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

    //cleaing up memory
    for (int i = 0; i < levelCount; i++)
    {
        deleteMap(levels[i].map, levels[i].rows);
        delete[] levels[i].guards;
        delete[] levels[i].guardDirection;
    }

    delete[] levels;
    return 0;
}