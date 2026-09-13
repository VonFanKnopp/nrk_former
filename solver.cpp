#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <array>
#include <cstdint>
#include <bitset>
constexpr int BOARD_WIDTH = 3;
constexpr int BOARD_HEIGHT = 3;
constexpr int BOARD_SIZE = BOARD_WIDTH * BOARD_HEIGHT;
constexpr int NUM_SHAPES = 4;

using Bitboard = std::uint64_t;
using Bitboards = std::array<Bitboard, NUM_SHAPES>;
using Field = std::vector<Bitboard>;
using Fields = std::array<Field, NUM_SHAPES>;

constexpr Bitboard getTopMask(const int width, const int height) {
    Bitboard mask = 0;
    for (int i = 0; i < width; i++) {
        mask |= 1ULL << height * i;
    }
    return mask;
}

constexpr Bitboard getBottomMask(const int width, const int height) {
    Bitboard mask = 0;
    for (int i = 0; i < width; i++) {
        mask |= 1ULL << (height * (i + 1) - 1);
    }
    return mask;
}

constexpr Bitboard topMask = getTopMask(BOARD_WIDTH, BOARD_HEIGHT);
constexpr Bitboard bottomMask = getBottomMask(BOARD_WIDTH, BOARD_HEIGHT);


std::vector<int> parseBoardString(std::string boardString) {
    std::vector<int> board = std::vector<int>();

    for (char c : boardString) {

        if (std::isdigit(static_cast<unsigned char>(c))) {
            int shape = c - '0'; // ok?
            board.push_back(shape);
        }
    }
    return board;
}

void printBoard(std::vector<int> board, const int width, const int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            std::cout << board[index] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

std::vector<int> rotateBoard(std::vector<int> board) {
    std::vector<int> rotatedBoard = std::vector<int>();
    for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
            int index = y * BOARD_WIDTH + x;
            rotatedBoard.push_back(board[index]);
        }
    }
    return rotatedBoard;
}

Bitboards getBitboards(const std::vector<int> &rotatedBoard) {
    Bitboards bbs{};
    for (int i = 0; i < BOARD_SIZE; i++) {
        int shape = rotatedBoard[i];
        bbs[shape - 1] |= 1ULL << i;
    }
    return bbs;
}

void printBitboard(const Bitboard b, const int width, const int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = y * width + x;
            {
                std::cout << ((b >> i) & 1) << " ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void printBitboards(const Bitboards &bbs, const int width, const int height) {
    for (Bitboard b : bbs) {
        printBitboard(b, width, height);
    }
}

Fields getFields(Bitboards bbs) {

    Fields fields{};

    for (int shape = 0; shape < NUM_SHAPES; shape++) {
        Bitboard b = bbs[shape];
        std::vector<Bitboard> same_shape_fields;
        while (b) {
            Bitboard first = b & -b;
            Bitboard field = first;
            Bitboard frontier = first;

            b &= ~first;
            while (frontier) {
                Bitboard up = (frontier >> 1) & ~topMask;
                Bitboard down = (frontier << 1) & ~bottomMask;
                Bitboard left = (frontier << BOARD_HEIGHT);
                Bitboard right = (frontier >> BOARD_HEIGHT);

                Bitboard neighbors = (up | down | left | right) & b;
                b &= ~neighbors;
                frontier = neighbors;
                field |= neighbors;
            }
            same_shape_fields.push_back(field);
        }
        fields[shape] = same_shape_fields;
    }
    return fields;
}

int main() {
    const std::string boardString = std::string("[1 1 1 3 2 3 3 2 1]");

    std::vector<int> board = parseBoardString(boardString);
    printBoard(board, BOARD_WIDTH, BOARD_HEIGHT);
    std::vector<int> r_board = rotateBoard(board);
    printBoard(r_board, BOARD_HEIGHT, BOARD_WIDTH);
    Bitboards bbs = getBitboards(r_board);
    Fields fields = getFields(bbs);
    for (Field field : fields) {
        for (Bitboard b : field) {
            printBitboard(b, BOARD_HEIGHT, BOARD_WIDTH);
        }
    }
    return 0;
}
