#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <array>
#include <cstdint>
#include <unordered_set>
#include <cmath>
#include <utility>
#include <algorithm>
#include <bit>
#include <cstdint>
#include <unordered_map>
#include <queue>
#include <optional>

constexpr int BOARD_WIDTH = 6;
constexpr int BOARD_HEIGHT = 6;
constexpr int BOARD_SIZE = BOARD_WIDTH * BOARD_HEIGHT;
constexpr int NUM_SHAPES = 4;

using Bitboard = std::uint64_t;
using Bitboards = std::array<Bitboard, NUM_SHAPES>;
using SameShapeFields = std::vector<Bitboard>;
using Fields = std::array<SameShapeFields, NUM_SHAPES>;

constexpr std::array<Bitboard, BOARD_SIZE> getColMasks(const int width, const int height) {
    std::array <Bitboard, BOARD_SIZE> masks{};
    int index = 0;
    for (int row = 0; row < height; row++) {
        Bitboard mask = ((1ULL << width) - 1) << width * row;
        for (int start = 0; start < width; start++) {
            masks[index] = mask;
            mask &= ~(1ULL << (row * width + start));
            index++;
        }
    }
    return masks;
}

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

//brukbare bit masks
constexpr std::array<Bitboard, BOARD_SIZE> colMasks = getColMasks(BOARD_HEIGHT, BOARD_WIDTH);
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

Bitboards getBitboards(const std::vector<int>& rotatedBoard) {
    Bitboards bbs{};
    for (int i = 0; i < BOARD_SIZE; i++) {
        int shape = rotatedBoard[i];
        bbs[shape - 1] |= 1ULL << i;
    }
    return bbs;
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

void printBitboards(const Bitboards& bbs, const int width, const int height) {
    for (Bitboard b : bbs) {
        printBitboard(b, width, height);
    }
}

void printBoardFromBitboards(const Bitboards& bbs, const int width, const int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = y * width + x;
            bool found = false;
            for (int shape = 0; shape < NUM_SHAPES; shape++) {
                if ((bbs[shape] >> i) & 1 == 1) {
                    std::cout << shape + 1 << " ";
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cout << "0" << " ";
            }

        }
        std::cout << "\n";
    }
}

int getShape(const Bitboards& bbs, const int index) {
    //Returnerer figuren på gitt index, -1 hvis det ikke er noe der
    for (int shape = 0; shape < NUM_SHAPES; shape++) {
        Bitboard bitboard = bbs[shape];
        if ((bitboard >> index) & 1ULL) {
            return shape;
        }
    }
    return -1;
}

Bitboard getField(const Bitboards& bbs, const int index, const int shape) {
    Bitboard b = bbs[shape];

    Bitboard first = b & (1ULL << index);
    Bitboard field = first;
    Bitboard frontier = first;

    b &= ~first;
    while (frontier) {
        Bitboard up = (frontier << 1) & ~topMask;
        Bitboard down = (frontier >> 1) & ~bottomMask;
        Bitboard left = (frontier << BOARD_HEIGHT);
        Bitboard right = (frontier >> BOARD_HEIGHT);
        Bitboard neighbors = (up | down | left | right) & b;

        b &= ~neighbors;
        frontier = neighbors;
        field |= neighbors;
    }
    return field;
}

Fields getFields(const Bitboards& bbs) {
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
                Bitboard up = (frontier << 1) & ~topMask;
                Bitboard down = (frontier >> 1) & ~bottomMask;
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

inline int getLeftIndex(const Bitboard f) {
    return std::countr_zero(f);
}

inline int getRightIndex(const Bitboard f) {
    return std::bit_width(f) - 1;
}

inline bool isOverlap(int x_start, int x_end, int y_start, int y_end) {
    return x_start <= y_end && y_start <= x_end;
}

// TODO:
// Kan hende jeg bare trenger antallet, og ikke hele greia, men vi får se.
// Gjøre om til sortering + scanning
// Ren scanline approach med Bitboards i stedet for fields.

std::vector<SameShapeFields> getConnectableFields(const Fields& fields) {
    std::vector<SameShapeFields> connectableFields;

    for (int shape = 0; shape < NUM_SHAPES; shape++) {
        SameShapeFields current = fields[shape];
        while (current.size() > 0) {
            SameShapeFields connectableField{};
            Bitboard field = current.back();
            current.pop_back();

            connectableField.push_back(field);

            int min_col = std::max(getLeftIndex(field) / BOARD_HEIGHT - 1, 0);
            int max_col = std::min(getRightIndex(field) / BOARD_HEIGHT + 1, BOARD_WIDTH - 1);

            size_t i = 0;
            while (i < current.size()) {
                const Bitboard newField = current[i];
                int new_min_col = getLeftIndex(newField) / BOARD_HEIGHT;
                int new_max_col = getRightIndex(newField) / BOARD_HEIGHT;

                if (isOverlap(min_col, max_col, new_min_col, new_max_col)) {
                    min_col = std::max(std::min(new_min_col - 1, min_col), 0);
                    max_col = std::min(std::max(new_max_col + 1, max_col), BOARD_WIDTH - 1);
                    connectableField.push_back(newField);
                    current[i] = current.back();
                    current.pop_back();
                    i = 0;
                }
                else {
                    i++;
                }
            }
            connectableFields.push_back(connectableField);
        }
    }
    return connectableFields;
}

Bitboards reconstructFallingCol(Bitboards& bbs, Bitboard fullBb, Bitboard colMask) {
    Bitboards newCols{};
    int offset = 0;
    while (colMask) {

        Bitboard first = colMask & -colMask;
        colMask &= ~first;

        if (first & fullBb == 0) { continue; }

        for (int i = 0; i < NUM_SHAPES; i++) {
            if (bbs[i] & first) {
                newCols[i] |= 1ULL << (offset);
                offset++;
            }
        }
    }
    return newCols;
}

Bitboards explore(Bitboards bbs, Bitboard field, int shape) {
    // Fjerner et field og samtidig flytter alt over ned.
    // Sletter hele den delen av brettet som er påvirket, og bygger opp de nye kolonnene
    bbs[shape] &= ~field;
    Bitboard fullBitboard = bbs[0] | bbs[1] | bbs[2] | bbs[3];
    while (field) {
        Bitboard index = getLeftIndex(field);
        field &= ~index;
        Bitboard colMask = colMasks[index];
        // printBitboard(colMask, BOARD_HEIGHT, BOARD_WIDTH);
        Bitboards newCols = reconstructFallingCol(bbs, fullBitboard, colMask);
        for (int i = 0; i < NUM_SHAPES; i++) {
            Bitboard newCol = newCols[i] << index;
            if (newCol == 0) { continue; }
            bbs[i] &= ~colMask;
            bbs[i] |= newCol;
        }
        field &= ~colMask;
    }
    return bbs;
}

inline bool isSolved(const Bitboard& remaining) {
    return remaining == 0;
}

Bitboards canonicalize(Bitboards board) {
    std::sort(board.begin(), board.end());
    return board;
}

struct MoveNode {
    std::uint64_t parentIndex;
    int move;
};

struct SearchNode {
    Bitboards boardState;
    Bitboard remaining;
    std::uint64_t moveTreeIndex;
    int depth;
};

struct BoardHash {
    std::size_t operator()(const Bitboards& board) const {
        std::size_t seed = 0;

        for (Bitboard value : board) {
            seed ^= std::hash<Bitboard>{}(value)
                +0x9e3779b9
                + (seed << 6)
                + (seed >> 2);
        }
        return seed;
    }
};

struct PQHeuristic {
    bool operator()(const SearchNode& nodeA, const SearchNode& nodeB) const {
        return heuristic(nodeA) < heuristic(nodeB);
    }
    float heuristic(const SearchNode& node) const {
        if (node.depth == 0) {
            return 0.0f;
        }
        int empty = BOARD_SIZE - std::popcount(node.remaining);
        float heuristic = static_cast<float>(empty) / node.depth;
        return heuristic;
    }
};

int getMaxSearchDepth(std::vector<SearchNode> nextBatch) {
    int maxConnectableFields = 0;

    for (const SearchNode& node : nextBatch) {
        Fields fields = getFields(node.boardState);
        std::vector<SameShapeFields> connectableFields = getConnectableFields(fields);

        maxConnectableFields = std::max(maxConnectableFields, static_cast<int>(connectableFields.size()));
    }
    return maxConnectableFields;
}

std::vector<int> reconstructSolutionPath(const std::vector<MoveNode>& moveTree, const SearchNode& solutionNode) {
    std::vector<int> path{};

    std::uint64_t treeIndex = solutionNode.moveTreeIndex;

    while (true) {
        MoveNode moveNode = moveTree[treeIndex];

        int move = moveNode.move;
        if (move == -1) { break; }
        path.push_back(move);
        treeIndex = moveNode.parentIndex;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<std::pair<int, int>> convertToNormalCoords(const std::vector<int>& path) {
    std::vector<std::pair<int, int>> normalCoords{};

    for (int move : path) {
        int x = move / BOARD_HEIGHT;
        int y = move % BOARD_HEIGHT;
        y = BOARD_HEIGHT - y - 1;
        normalCoords.push_back(std::make_pair(x, y));
    }
    return normalCoords;
}
void solve(const std::string& boardString) {
    std::vector<int> board = parseBoardString(boardString);
    std::vector<int> r_board = rotateBoard(board);

    std::optional<SearchNode> solution;
    int moveTreeSolutionIndex;
    Bitboards initialBitboards = getBitboards(r_board);
    printBoardFromBitboards(initialBitboards, BOARD_HEIGHT, BOARD_WIDTH);
    SearchNode startSearchNode = SearchNode{
        initialBitboards,
        initialBitboards[0] | initialBitboards[1] | initialBitboards[2] | initialBitboards[3],
        0,
        0
    };
    MoveNode startMoveNode = MoveNode{ 0,-1 };

    std::vector<MoveNode> moveTree;
    std::unordered_map<Bitboards, int, BoardHash> visited;
    std::priority_queue <SearchNode, std::vector<SearchNode>, PQHeuristic> pq;
    pq.push(startSearchNode);
    moveTree.push_back(startMoveNode);

    /*
    Siden man aldri vil finne en løsning på mindre enn |connecable fields| trekk, kan man generere alle brett inntill det.
    Derfor søke til dypde |connectable fields| før det er poeng i å sjekke noe som helst, hvis ingen løsning er funnet,
    gjenta prosessen på alle nodes som er igjen.
    */

    std::vector<SearchNode> nextBatch;
    int maxSearchDepth = getConnectableFields(getFields(startSearchNode.boardState)).size();


    while (!solution) {
        if (pq.size() == 0 && nextBatch.size() == 0) { break; }

        while (pq.size() > 0) {
            SearchNode currNode = pq.top();
            pq.pop();
            MoveNode currMoveNode = moveTree[currNode.moveTreeIndex];
            while (currNode.remaining) {
                Bitboards boardState = currNode.boardState;

                const int firstCellIndex = getLeftIndex(currNode.remaining);
                const int shape = getShape(boardState, firstCellIndex);
                const Bitboard field = getField(boardState, firstCellIndex, shape);

                currNode.remaining &= ~field;

                Bitboards newBoardState = explore(boardState, field, shape);
                int newDepth = currNode.depth + 1;

                Bitboards key = canonicalize(newBoardState);
                auto it = visited.find(key);
                if (it != visited.end() && it->second <= newDepth) {
                    continue;
                }
                visited[key] = newDepth;
                // printBitboard(currNode.remaining, BOARD_HEIGHT, BOARD_WIDTH);
                // printBitboard(field, BOARD_HEIGHT, BOARD_WIDTH);
                // std::cout << firstCellIndex << " \n";

                MoveNode newMoveNode = MoveNode{
                    currNode.moveTreeIndex,
                    firstCellIndex
                };
                moveTree.push_back(newMoveNode);

                SearchNode newNode = SearchNode{
                    newBoardState,
                    newBoardState[0] | newBoardState[1] | newBoardState[2] | newBoardState[3],
                    moveTree.size() - 1,
                    currNode.depth + 1
                };

                if (isSolved(newNode.remaining) && newNode.depth <= maxSearchDepth) {
                    solution = newNode;
                    maxSearchDepth = currNode.depth + 1;
                    moveTreeSolutionIndex = moveTree.size() - 1;
                    break;
                }

                if (newNode.depth == maxSearchDepth) {
                    // std::cout << "pushed to batch" << "\n";
                    nextBatch.push_back(newNode);
                }
                else {
                    // std::cout << "pushed to pq" << "\n";
                    pq.push(newNode);
                }
            }
        }
        maxSearchDepth += getMaxSearchDepth(nextBatch);
        std::cout << "Search Depth increased " << maxSearchDepth << "\n";
        std::cout << moveTree.size() << "\n";
        for (SearchNode node : nextBatch) {
            pq.push(node);
        }
        nextBatch.clear();
    }
    SearchNode s = solution.value();
    std::vector<int> path = reconstructSolutionPath(moveTree, s);
    std::vector<std::pair<int, int>> pathNormalCoords = convertToNormalCoords(path);

    for (int move : path) {
        std::cout << move << "\n";
    }
    for (std::pair<int, int> move : pathNormalCoords) {
        std::cout << "(" << move.first << ", " << move.second << "), ";
    }
    std::cout << s.depth << "\n";
    std::cout << moveTree.size();
}

int main() {
    // 7x9
    // const std::string boardString = std::string("[1 1 1 3 2 3 3 2 1 2 4 4 3 4 3 1 1 3 4 3 4 4 2 2 2 2 1 2 3 2 2 3 2 4 4 3 3 3 4 2 4 4 2 4 3 4 3 4 4 3 4 4 2 3 2 3 4 3 3 4 3 2 4]");
    // 4x4
    // const std::string boardString = std::string("[1 1 1 3 2 3 3 2 1 2 4 4 3 4 3 1]");
    const std::string boardString = std::string("[1 1 1 3 2 3 3 2 1 2 4 4 3 4 3 1 1 3 4 3 4 4 2 2 2 2 1 2 3 2 2 3 2 4 4 3]");
    // 3x3
    // const std::string boardString = std::string("[1 1 1 3 2 3 3 2 1]");
    solve(boardString);
    return 0;
}