#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <map>
#include <stack>
#include <algorithm>

using namespace std;

// get boerd without sliders
short boardSize;
pair<short, short> target;
vector<vector<char> > gameBoard;
vector<pair<short, short> > currentState;


vector<vector<char> > gameBoardWithoutSLiders;

bool reachTarget;
vector<pair<short, short> > startState;
vector<pair<short, short> > finalState;


int queueIndex;
int queueSize;
vector<pair<short, short> > queue[5000000];


map<vector<pair<short, short> >, pair<vector<pair<short, short> >, char> > parentState;


int tmpStateIndex;
vector<pair<short, short> > tmpState;


int minimumNumberOfMoves;
string path;

int slidersCount;
int newStateIndex;
vector<pair<short, short> > newState;

void ReadBoardFromFile(const string& path) {
    ifstream file(path);
    if (!file.is_open()) {
        cout << "Could not open file" << endl;
        exit(0);
    }

    string line;
    getline(file, line);
    boardSize = stoi(line);

    if (boardSize < 1) {
        cout << "Board size must be at least 1." << endl;
        exit(0);
    }

    slidersCount = 0;

    gameBoard.resize(boardSize, vector<char>(boardSize));
    short row = 0;

    while (row < boardSize && getline(file, line)) {
        istringstream iss(line);
        string item;
        short col = 0;

        for(short i = 0 ; i < boardSize ; ++i)
        {
            string tmp;
            iss >> tmp;
            gameBoard[row][col++] = tmp[0];

            if(tmp[0] == 'o')
            {
                ++slidersCount;
            }
        }

        if (col != boardSize) {
            cout << "Each row must have exactly " + to_string(boardSize) + " columns." << endl;
            exit(0);
        }

        ++row;
    }

    if (row != boardSize) {
        cout << "File does not contain enough rows." << endl;
        exit(0);
    }

    getline(file, line);

    istringstream targetStream(line);
    string targetItem;

    targetStream >> targetItem;
    targetItem.pop_back();
    short targetColumn = stoi(targetItem);

    targetStream >> targetItem;
    short targetRow = stoi(targetItem);

    target = make_pair(targetRow, targetColumn);
}



void RevertSlidersPositions(vector<pair<short, short> > &newState, vector<pair<short, short> > &oldState)
{
    for(int i = 0 ; i < slidersCount ; ++i)
    {
        gameBoard[oldState[i].first][oldState[i].second] = '.';
    }

    for(int i = 0 ; i < slidersCount ; ++i)
    {
        gameBoard[newState[i].first][newState[i].second] = 'o';
    }
}

void Enqueue(vector<pair<short, short> > &state)
{
    for(int i = 0 ; i < slidersCount ; ++i)
    {
        if(state[i].first == target.first && state[i].second == target.second)
        {
            reachTarget = true;
            finalState = state;
            return ;
        }

    }

    queue[queueSize++] = state;
}

/**************************************************************************************************************/



void TryMoveToNextState(vector<pair<short, short> > &currentState, vector<pair<short, short> > &nextState, char direction)
{
    sort(nextState.begin(), nextState.end());
    if(parentState.find(nextState) == parentState.end())
    {
        Enqueue(nextState);
        parentState[nextState] = make_pair(currentState, direction);
    }
}

/************************************/



void MoveToRight()
{
    tmpStateIndex = 0;
    newStateIndex = 0;

    for (short row = 0; row < boardSize; ++row)
    {
        for (short column = 0; column < boardSize; ++column)
        {
            if (gameBoard[row][column] == 'o')
            {
                tmpState[tmpStateIndex++] = make_pair(row, column);
            }
            if (gameBoard[row][column] == '#' || column == boardSize - 1)
            {
                short tmpColumn = column - 1;
                if (gameBoard[row][column] != '#')
                    tmpColumn++;

                for(short i = tmpStateIndex - 1 ; i >= 0 ; --i)
                {
                    gameBoard[row][tmpState[i].second] = '.';
                    newState[newStateIndex++] = make_pair(row, tmpColumn);
                    gameBoard[row][tmpColumn] = 'o';
                    tmpColumn--;
                }

                tmpStateIndex = 0;
            }
        }
    }

    RevertSlidersPositions(queue[queueIndex], newState);
    TryMoveToNextState(queue[queueIndex], newState, 'r');
}

void MoveToLeft()
{
    tmpStateIndex = 0;
    newStateIndex = 0;

    for (short row = 0; row < boardSize; ++row)
    {
        for (short column = boardSize - 1; column >= 0; --column)
        {
            if (gameBoard[row][column] == 'o')
            {
                tmpState[tmpStateIndex++] = make_pair(row, column);
            }
            if (gameBoard[row][column] == '#' || column == 0)
            {
                short tmpColumn = column + 1;
                if (gameBoard[row][column] != '#')
                    tmpColumn--;

                for(short i = tmpStateIndex - 1 ; i >= 0 ; --i)
                {
                    gameBoard[row][tmpState[i].second] = '.';
                    newState[newStateIndex++] = make_pair(row, tmpColumn);
                    gameBoard[row][tmpColumn] = 'o';
                    tmpColumn++;
                }

                tmpStateIndex = 0;
            }
        }
    }

    RevertSlidersPositions(queue[queueIndex], newState);
    TryMoveToNextState(queue[queueIndex], newState, 'l');
}

void MoveToUp()
{
    tmpStateIndex = 0;
    newStateIndex = 0;

    for (short column = 0; column < boardSize; ++column)
    {
        for (short row = boardSize - 1; row >= 0 ; --row)
        {
            if (gameBoard[row][column] == 'o')
            {
                tmpState[tmpStateIndex++] = make_pair(row, column);
            }
            if (gameBoard[row][column] == '#' || row == 0)
            {
                short tmpRow = row + 1;
                if (gameBoard[row][column] != '#')
                    tmpRow--;

                for(short i = tmpStateIndex - 1 ; i >= 0 ; --i)
                {
                    gameBoard[tmpState[i].first][column] = '.';
                    newState[newStateIndex++] = make_pair(tmpRow, column);
                    gameBoard[tmpRow][column] = 'o';
                    tmpRow++;
                }

                tmpStateIndex = 0;
            }
        }
    }

    RevertSlidersPositions(queue[queueIndex], newState);
    TryMoveToNextState(queue[queueIndex], newState, 'u');
}

void MoveToDown()
{
    tmpStateIndex = 0;
    newStateIndex = 0;

    for (short column = 0; column < boardSize; ++column)
    {
        for (short row = 0; row < boardSize; ++row)
        {
            if (gameBoard[row][column] == 'o')
            {
                tmpState[tmpStateIndex++] = make_pair(row, column);
            }
            if (gameBoard[row][column] == '#' || row == boardSize - 1)
            {
                short tmpRow = row - 1;
                if (gameBoard[row][column] != '#')
                    tmpRow++;

                for(short i = tmpStateIndex - 1 ; i >= 0 ; --i)
                {
                    gameBoard[tmpState[i].first][column] = '.';
                    newState[newStateIndex++] = make_pair(tmpRow, column);
                    gameBoard[tmpRow][column] = 'o';
                    tmpRow--;
                }

                tmpStateIndex = 0;
            }
        }
    }

    RevertSlidersPositions(queue[queueIndex], newState);
    TryMoveToNextState(queue[queueIndex], newState, 'd');
}

/**************************************************************************************************************/

void Bfs()
{
    queueIndex = 0;
    queueSize = 0;
    reachTarget = false;

    Enqueue(currentState);

    while (queueIndex < queueSize && reachTarget == false)
    {
        if (queueIndex != 0) {
            RevertSlidersPositions(queue[queueIndex], queue[queueIndex - 1]);

            char p = parentState[queue[queueIndex]].second;
            if(p=='r'||p=='l'){
                MoveToUp();
                MoveToDown();
            }else{
                MoveToRight();
                MoveToLeft();
            }
            ++queueIndex;
        }
        else {
            MoveToRight();

            MoveToLeft();

            MoveToUp();

            MoveToDown();

            ++queueIndex;
        }
    }
}


void UpdateBoardWithoutSlidersandCurrentState()
{
    for (short row = 0; row < boardSize; ++row)
    {
        gameBoardWithoutSLiders.push_back(vector<char>());
        for (short column = 0; column < boardSize; ++column)
        {
            if (gameBoard[row][column] != 'o')
            {
                gameBoardWithoutSLiders[row].push_back(gameBoard[row][column]);
            }
            else
            {
                currentState.push_back(make_pair(row, column));
                startState.push_back(make_pair(row, column));
                gameBoardWithoutSLiders[row].push_back('.');
            }
        }
    }
}

void Intialize()
{
    UpdateBoardWithoutSlidersandCurrentState();
    tmpState.resize(boardSize);
    newState.resize(slidersCount);
}

string GetDirection(char ch)
{
    if(ch == 'r')
    {
        return "right";
    }

    if(ch == 'l')
    {
        return "left";
    }

    if(ch == 'u')
    {
        return "up";
    }

    return "down";
}


void BuildOutput()
{
    ofstream outputFile("output.txt", ios::out);

    if (!outputFile.is_open()) {
        cout << "Error opening file. Creating a new file." << endl;
        return;
    }

    if(reachTarget == false)
    {
        outputFile << "Unsolvable" << endl;
        return;
    }

    minimumNumberOfMoves = 0;
    stack<vector<pair<short, short> > > reversedPath;
    vector<string> reversedPathDirections;

    while (finalState != startState)
    {
        auto p = parentState[finalState];
        auto prevState = p.first;
        auto direction = p.second;

        reversedPathDirections.push_back(GetDirection(direction));
        reversedPath.push(finalState);
        finalState = prevState;
        minimumNumberOfMoves++;
    }

    reversedPath.push(startState);
    reversedPathDirections.push_back("Initial");

    for(int i = reversedPathDirections.size() - 2 ; i >= 0 ; --i)
    {
        path += reversedPathDirections[i];
        if(i != 0)path += ", ";
    }

    outputFile << "Solvable" << endl;
    outputFile << "Min number of moves: " << minimumNumberOfMoves << endl;
    outputFile << "Sequence of moves: " << path << endl;

    while (reversedPath.size() > 0)
    {
        auto state = reversedPath.top();
        reversedPath.pop();

        for (auto slider : state)
        {
            gameBoardWithoutSLiders[slider.first][slider.second] = 'o';
        }

        auto direction = reversedPathDirections.back();
        reversedPathDirections.pop_back();

        outputFile << direction << endl;
        for (int i = 0; i < boardSize; i++)
        {
            for (int j = 0; j < boardSize; j++)
            {
                outputFile << gameBoardWithoutSLiders[i][j];
                if(j + 1 != boardSize)
                    outputFile << ", ";
            }
            outputFile << endl;
        }
        outputFile << endl;

        for (auto slider : state)
        {
            gameBoardWithoutSLiders[slider.first][slider.second] = '.';
        }
    }
}

int main()
{
    /****************************************************************************************************************************/
    cout<<"start \n";
    //ReadBoardFromFile("Sample Tests/Case1.txt");
    //ReadBoardFromFile("Sample Tests/Case2.txt");
    //ReadBoardFromFile("Sample Tests/Case3.txt");
    //ReadBoardFromFile("Sample Tests/Case4.txt");
    //ReadBoardFromFile("Sample Tests/Case5.txt");
    //ReadBoardFromFile("Sample Tests/Case6.txt");

    //ReadBoardFromFile("Complete Tests/1 small/Case 1/Case1.txt");
    //ReadBoardFromFile("Complete Tests/1 small/Case 2/Case2.txt");
    //ReadBoardFromFile("Complete Tests/1 small/Case 3/Case3.txt");

    //ReadBoardFromFile("Complete Tests/2 medium/Case 1/Case1.txt");
    //ReadBoardFromFile("Complete Tests/2 medium/Case 2/Case2.txt");
    //ReadBoardFromFile("Complete Tests/2 medium/Case 3/Case3.txt");

    ReadBoardFromFile("Complete Tests/3 large/Case 1/Case1.txt");
    //ReadBoardFromFile("Complete Tests/3 large/Case 2/Case2.txt");
    //ReadBoardFromFile("Complete Tests/3 large/Case 3/Case3.txt");


    Intialize();

    /****************************************************************************************************************************/

    // Start
    auto start = std::chrono::high_resolution_clock::now();

    Bfs();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    auto durationInSeconds = duration.count() / 1000.0;
    double durationInMinutes = duration.count() / (1000.0 * 60.0);

    cout <<  "\n\nTime taken to display the initial seconds : " << durationInSeconds << endl;
    cout <<  "Time taken to display the initial minutes : " << durationInMinutes << endl;
    cout << endl;

    BuildOutput();

    return 0;
}