import sys
import time
from random import randint, seed

import numpy as np
import pygame
from pyglm import glm


def draw_board(surface, board_data: np.ndarray):
    for i, cell in enumerate(board_data):
        if cell == 0:
            continue

        row = i // BOARD_WIDTH
        col = i % BOARD_WIDTH

        pos = glm.vec2(col, row) * TILE_SIZE + TILE_PADDING * glm.vec2(col + 1, row + 1)
        rect = pygame.Rect(pos, (TILE_SIZE, TILE_SIZE))
        pygame.draw.rect(surface, color_map[cell], rect)


def init_board(width: int, height: int) -> np.ndarray:
    board = np.empty(width * height, dtype=np.int8)

    for i in range(width * height):
        board[i] = randint(1, 4)
    return board


def get_field(board_data: np.ndarray, index):
    field = []
    if board_data[index] == 0:
        return np.array(field)

    visited = np.zeros_like(board_data)
    q = [index]
    field = []
    color = board_data[index]
    while q:
        curr = q.pop()
        if visited[curr]:
            continue
        field.append(curr)
        visited[curr] = 1
        for delta in (-BOARD_WIDTH, BOARD_WIDTH, -1, 1):
            new_pos = curr + delta
            if new_pos < 0 or new_pos >= board_data.size:
                continue
            if delta == 1 and curr in RIGHT_MASK or delta == -1 and curr in LEFT_MASK:
                continue
            if board_data[new_pos] == color:
                q.append(new_pos)
    return np.array(field)


def fall(board_data: np.ndarray, field: np.ndarray):
    """Mutates board state"""
    board_data[field] = 0
    cols = {i % BOARD_WIDTH for i in field}
    max_y = max(field) // BOARD_WIDTH
    # print(max_y)
    for col in cols:
        indices = range(max_y * BOARD_WIDTH + col, -1, -BOARD_WIDTH)
        non_zero = []
        num_zeros = 0
        for i in indices:
            shape = board[i]
            if shape != 0:
                non_zero.append(shape)
            else:
                num_zeros += 1
        non_zero.extend([0] * num_zeros)
        board[indices] = non_zero


def solved(board_state):
    for x in range(BOARD_WIDTH):
        index = (BOARD_HEIGHT - 1) * BOARD_WIDTH + x
        shape = board_state[index]
        if shape != 0:
            return False

    return True


class MoveHelper:
    def __init__(self, moves, click_delay_ms=250):
        self.moves: list = moves
        self.delay = click_delay_ms / 1000

        self.time_since_last_move = time.time()

    def do_move(self, board):
        if time.time() - self.time_since_last_move >= self.delay:
            self.time_since_last_move = time.time()

            x, y = self.moves.pop(0)
            index = y * BOARD_WIDTH + x
            field = get_field(board, index)
            if field.size > 0:
                fall(board, field)


pygame.init()
pygame.display.set_caption("Former")
seed(2)

BOARD_WIDTH = 7
BOARD_HEIGHT = 9
TILE_SIZE = 64
TILE_PADDING = 2
LEFT_MASK = [BOARD_WIDTH * i for i in range(BOARD_HEIGHT)]
RIGHT_MASK = [BOARD_WIDTH * i + BOARD_WIDTH - 1 for i in range(BOARD_HEIGHT)]

color_map = {1: (93, 196, 121), 2: (53, 177, 231), 3: (255, 156, 84), 4: (249, 88, 171)}

win_size = glm.vec2(BOARD_WIDTH, BOARD_HEIGHT) * TILE_SIZE + glm.vec2(BOARD_WIDTH + 1, BOARD_HEIGHT + 1) * TILE_PADDING
win = pygame.display.set_mode(win_size)

board = init_board(BOARD_WIDTH, BOARD_HEIGHT)
mover = MoveHelper(
    [(0, 0), (1, 4), (0, 6), (3, 7), (0, 7), (0, 8), (5, 6), (6, 7), (0, 8), (4, 8), (6, 8), (0, 8)], 500
)
print(board)
while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()
        if event.type == pygame.MOUSEBUTTONDOWN:
            mouse_pos = glm.vec2(pygame.mouse.get_pos())
            relative_mouse = mouse_pos // TILE_SIZE

            relative_mouse.x = glm.clamp(relative_mouse.x, 0, BOARD_WIDTH - 1)
            relative_mouse.y = glm.clamp(relative_mouse.y, 0, BOARD_HEIGHT - 1)
            index = int(relative_mouse.x + relative_mouse.y * BOARD_WIDTH)
            field = get_field(board, index)
            if field.size > 0:
                fall(board, field)
            if solved(board):
                pass

    if mover.moves:
        mover.do_move(board)

    win.fill((0, 0, 0))
    draw_board(win, board)
    pygame.display.update()
