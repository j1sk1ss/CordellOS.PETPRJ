// #include <stdio.h>
// #include <time.h> 
// #include <stdlib.h> 
// #include <strings.h> 
// #include <windows.h> 
// #include <conio.h> 

#include "../include/snake.h"

typedef struct {
	int x, y; // initial position
	int movX, movY;

	char imagen;
} snake;

typedef struct {
	int x, y;
} fruit;

snake snk[N];
fruit frt;

int hardnes_level = 9999999;

int snake_size;
int max_score;

int snake_init(int hard, int score) {
    switch (hard) {
        case 0:
            hardnes_level = 99999999;
        break;

        case 1:
            hardnes_level = 9999999;
        break;

        case 2:
            hardnes_level = 999999;
        break;

        case 3:
            hardnes_level = 99999;
        break;

        case 4:
            hardnes_level = 9999;
        break;
    }

	char map[V][H];
	snake_size = 4;
	max_score = score;

	begin(map);
	show(map);
	loop(map);

	return snake_size - 4;
}

// Initialized in a initial position
void begin(char map[V][H]) {
	int i;
	// snake head
	snk[0].x = 32;
	snk[0].y = 10; // V

	frt.x = srand_r() % (H - 2) + 1; // automatic position of fruit
	frt.y = srand_r() % (V - 2) + 1;

	for (i = 0; i < snake_size; i++) {
		snk[i].movX = 1;
		snk[i].movY = 0;
	} // snake only move on X

	intro(map);
	intro_data(map);
}

// Show everything
void show(char map[V][H]) {
	int i, j;

	for (i = 0; i < V; i++) {
		for (j = 0; j < H; j++) 
			printf("%c", map[i][j]);
		
		printf("\n");
	}

	printf("Score: [%i]", snake_size - 4);
	printf("\nMax score: [%i]", max_score);
}

// The camp (map).
void intro(char map[V][H]) {
	int i, j;

	for (i = 0; i < V; i++) 
		for (j = 0; j < H; j++) 
			if (i == 0 || i == V - 1) map[i][j] = '-';
			else if (j == 0 || j == H - 1) map[i][j] = '|';
			else map[i][j] = ' ';
}

// Introduce every data inIT
void intro_data(char map[V][H]) {
	int i;
	for (i = 1; i < snake_size; i++) {
		snk[i].x = snk[i - 1].x - 1;
		snk[i].y = snk[i - 1].y;

		snk[i].imagen = 'X'; // body
	}

	snk[0].imagen = 'O'; // head

	for (i = 0; i < snake_size; i++)
		map[snk[i].y][snk[i].x] = snk[i].imagen;
	
	map[frt.y][frt.x] = 'M'; // fruit
}

void loop(char map[V][H]) {
	int dead = 0;

	do {
		gotoxy(0, 0);

		show(map);
		input(map, &dead);
		update(map); // automatic

        int delay = hardnes_level;
        while (--delay > 0)
            continue;
	} while (dead == 0);
}

int input(char map[V][H], int *dead) {
	int i;
	char key;

	// Only two ways to die, collision with map or body, every part of the snake is an diferent structure

	// CHECK GAME CONDITIONS

	if (snk[0].x == 0 || snk[0].x == H - 1 || snk[0].y == 0 || snk[0].y == V - 1)  // 0 es la cabeza de la serpiente, solo evaluaremos cuando la cabeza choque.
		*dead = 1;

	for (i = 1; i < snake_size && *dead == 0; i++) 
		if (snk[0].x == snk[i].x && snk[0].y == snk[i].y) 
			*dead = 1;

	// CHECK FRUIT, IF HEAD GET CLOSE, EAT IT!

	if (snk[0].x == frt.x && snk[0].y == frt.y) {
		snake_size += 1;

		snk[snake_size - 1].imagen = 'X';

		// Regenerated fruit position in a random position

		frt.x = srand_r() % (H - 2) + 1;
		frt.y = srand_r() % (V - 2) + 1;
	}

	// IF DEAD IS ZERO WE CAN KEEP GOING

	if (*dead == 0) {
        if (i686_inb(0x64) & 0x1) {
            key = i686_inb(0x60);
            key = get_character(key);

			if (key == 's' && snk[0].movY != -1) {
				snk[0].movX = 0;
				snk[0].movY = 1;
			}

			if (key == 'w' && snk[0].movY != 1) {
				snk[0].movX = 0;
				snk[0].movY = -1;
			}

			if (key == 'a' && snk[0].movX != 1) {
				snk[0].movX = -1;
				snk[0].movY = 0;
			}

			if (key == 'd' && snk[0].movX != -1) {
				snk[0].movX = 1;
				snk[0].movY = 0;
			}

            if (key == F3_BUTTON) {
				*dead = 1;
				return;
			}
		}
	}
}

void update(char map[V][H]) {
	intro(map);
	intro_data2(map);
}

void intro_data2(char map[V][H]) {
	int i;

	for (i = snake_size - 1; i > 0; i--) { // 0 is the head. so we going decresing until extremities
		snk[i].x = snk[i - 1].x;
		snk[i].y = snk[i - 1].y;
	}

	snk[0].x += snk[0].movX; // plus movemenrts
	snk[0].y += snk[0].movY;

	// now introduce the values in the camp

	for (i = 0; i < snake_size; i++) 
		map[snk[i].y][snk[i].x] = snk[i].imagen;
	
	map[frt.y][frt.x] = 'M';
}

void gotoxy(int x, int y) { 
    VGA_setcursor(x, y);
}