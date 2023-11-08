#include "../include/tetris.h"

int board[WIDTH][HEIGHT];
int currentPiece[4][4];
int currentX, currentY;
int score = 0;

void initialize_game() {
    memset(board, 0, sizeof(board));
    new_piece();
}

void new_piece() {
    int pieceIndex = srand_r() % 7; // 7 different Tetris pieces
    int pieceX = WIDTH / 2 - 2;
    int pieceY = 0;

    int pieces[7][4][4] = {
        {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 1, 0}, {1, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    };

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            currentPiece[i][j] = pieces[pieceIndex][i][j];

    currentX = pieceX;
    currentY = pieceY;
}

int check_collision(int newX, int newY) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (currentPiece[i][j] && (newX + j >= WIDTH || newX + j < 0 || newY + i >= HEIGHT || (newY + i >= 0 && board[newX + j][newY + i])))
                return 1;
    return 0;
}

void clear_rows() {
    for (int i = HEIGHT - 1; i >= 0; ) {
        int rowIsFull = 1;
        for (int j = 0; j < WIDTH; j++) 
            if (!board[j][i]) 
                rowIsFull = 0;
                break;
            
        if (rowIsFull) {
            for (int k = i; k > 0; k--) 
                for (int j = 0; j < WIDTH; j++) 
                    board[j][k] = board[j][k - 1];
                
            score += 10;
        }
        else i--;
    }
}

void merge_piece() {
    for (int i = 0; i < 4; i++) 
        for (int j = 0; j < 4; j++) 
            if (currentPiece[i][j]) 
                board[currentX + j][currentY + i] = 1;

    clear_rows();
    new_piece();
}

void draw_board() {
    VGA_text_clrscr();
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) 
            if (board[j][i]) printf("[]");
            else printf("  ");

        printf("\n");
    }

    printf("Score: %d\n", score);
}

int init_tetris() {
    initialize_game();
    while (1) {
        char key;
        if (i686_inb(0x64) & 0x1) { // Non-blocking input
            key = get_character(i686_inb(0x60));

            if (key == 'q') break;
            if (key == 'a' && !check_collision(currentX - 1, currentY)) currentX--;
            if (key == 'd' && !check_collision(currentX + 1, currentY)) currentX++;
            if (key == 's' && !check_collision(currentX, currentY + 1)) currentY++;
        }

        if (!check_collision(currentX, currentY + 1)) currentY++;
        else  merge_piece();
        
        draw_board();
    }

    return 0;
}