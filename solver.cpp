#include <iostream>
#include <string>
#include <vector>
#include <cctype>

const int BOARD_WIDTH = 9;
const int BOARD_HEIGHT = 7;

std::vector<int> parseBoardString(std::string boardString)
{
    std::vector<int> board = std::vector<int>();

    for (char c : boardString)
    {

        if (std::isdigit(static_cast<unsigned char>(c)))
        {
            int shape = c - '0'; // ok?
            board.push_back(shape);
        }
    }
    return board;
}

void printBoard(std::vector<int> board, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = y * width + x;
            std::cout << board[index] << " ";
        }
        std::cout << "\n";
    }
    int size = board.size();
}

int main()
{

    std::string boardString = std::string("[1 1 1 3 2 3 3 2 1 2 4 4 3 4 3 1 1 3 4 3 4 4 2 2 2 2 1 2 3 2 2 3 2 4 4 3 3 3 4 2 4 4 2 4 3 4 3 4 4 3 4 4 2 3 2 3 4 3 3 4 3 2 4]");
    std::vector<int> board = parseBoardString(boardString);
    printBoard(board, BOARD_WIDTH, BOARD_HEIGHT);

    return 0;
}
