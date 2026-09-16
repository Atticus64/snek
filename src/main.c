#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <termios.h>
#include <unistd.h>

#define WINDOW_WIDTH 100
#define WINDOW_HEIGHT 20


int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Disable buffering and echoing
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore settings
    return ch;
}

typedef struct Player {
	char icon;
	int x;
	int y;
	int lifetime;
} player_t;

typedef struct Window {
	int width;
	int height;
	char** data;
	bool exit;
	int seconds;
} window_t;

window_t* create_window(int heigth, int width) {
	window_t* my_Window = malloc(sizeof(window_t));
	my_Window->data = malloc(width * sizeof(char *));
	for (int i = 0; i < heigth; i++) {
		my_Window->data[i] = malloc(width * sizeof(char));
	}

	for(int i = 0; i < heigth; i++) {
		for (int j = 0; j < width; j++) {
			if (i != 0) {
				my_Window->data[i][j] = '_';
			}
			// last row
			my_Window->data[heigth - 1][j] = '*';

			// first row
			my_Window->data[0][j] = '*';
		}

		// right col
		my_Window->data[i][0] = '*';
		// left col
		my_Window->data[i][heigth - 1] = '*';
		
		// first row
	}
	my_Window->height = heigth;
	my_Window->width = heigth;

	return my_Window;
}

void destroy_window(window_t* window) {
	free(window->data);
	free(window);
}

bool limit_player(char movement, player_t* player, window_t* canvas) {

	bool limit_exceded = false;

	if (player->x == canvas->height - 1 && movement == 'j') {
		player->x = 0;
		canvas->data[player->x][canvas->height] = '*';
		return true;
	}
	
	if (player->x == 1 && movement == 'k') {
		player->x = canvas->height - 1;
		canvas->data[player->x][player->y] = '*';
		return true;
	}

	if (player->y == canvas->width - 1 && movement == 'l') {
		player->y = 0;
		canvas->data[player->x][canvas->width] = '*';
		return true;
	}
	
	if (player->y == 1 && movement == 'h') {
		player->y = canvas->width - 1;
		canvas->data[player->x][player->y] = '_';
		return true;
	}

	bool x_limits_vulk = player->x == 0 || player->x == canvas->height - 1;
	bool in_limit = player->x == canvas->height || player->y == canvas->width;
	bool y_limits_vulk = player->y == canvas->width - 1 || player->y == 0;
	limit_exceded = x_limits_vulk || y_limits_vulk || in_limit;

	return limit_exceded;
};

bool check_collision(char movement, player_t* player, window_t* canvas) {


	char current_pos = canvas->data[player->x][player->y];

	if (current_pos == '$') {
		return true;
	}

	return false;
}

void refresh_player(char movement, player_t* player, window_t* canvas) {
	// TODO!
	//
	bool limited_exceded = limit_player(movement, player, canvas);

	

	canvas->data[player->x][player->y] = '$';
	switch (movement) {
		case 'l': 
			player->y += 1;
			break;
		case 'j':
			player->x += 1;
			break;
		case 'k':
			player->x -= 1;
			break;
		case 'h':
			player->y -= 1;
			break;
		case 'q':
			canvas->exit = true;
			break;
		default:
			break;
	}

	if (check_collision(movement, player, canvas)) {
		canvas->data[player->x][player->y] = 'X';
		canvas->exit = true;
	}

	if (limited_exceded) {
		canvas->data[player->x][player->y] = 'X';
		canvas->exit = true;
	} else {
		canvas->data[player->x][player->y] = '&';
	}
}

void draw_window(window_t* window, player_t* player) {
	int input;
	while(1) {
		for (int i = 0; i < window->width; i++) {
			for ( int j = 0; j < window->height; j++) {
				
				printf("%c", window->data[i][j]);
			} 
			printf("\n");
		}
		input = getch();


		// up
		refresh_player(input, player, window);

		if (window->exit) {
			system("clear");
			printf(":( Game over");
			return;
		}
		system("clear");
		window->seconds += 1000;
		int hours = window->seconds / 3600;
		player->lifetime += hours;
	}
}

void initial_page() {
	puts("	___                     _                ___                    _      ");
 	puts(" / __|   _ _      ___    | |__     o O O  | _ )     _ _   _  _   | |_    ");
  	puts(" \\__ \\  | ' \\    / -_)   | / /    o       | _ \\    | '_| | +| |  | ' \\   ");
  	puts(" |___/  |_||_|   \\___|   |_\\_\\   TS__[O]  |___/   _|_|_   \\_,_|  |_||_|  ");
	puts("_|\"""""|_|\"""""|_|\"""""|_|\"""""| {======|_|\"""""|_|\"""""|_|\"""""|_|\"""""| ");
	puts("`-0-0-'\"`-0-0-'\"`-0-0-'\"`-0-0-'./o--000'\"`-0-0-'\"`-0-0-'\"`-0-0-\'\"`-0-0-\'\")\"");
}

void get_score(int lifetime) {



	if (lifetime > 24) {
		printf("Score => %d days lived", lifetime / 24);
	} else {
		printf("Score => %d hours lived", lifetime);
	}
	
}

void start_game() {
	system("clear");
	char* data[] = {};
	player_t* player = malloc(sizeof(player_t));
	player->icon = '&';
	player->x = 10;
	player->y = 10;

	window_t* window = create_window(WINDOW_HEIGHT, WINDOW_WIDTH);
	draw_window(window, player);

	get_score(player->lifetime);
	destroy_window(window);
	free(player);
}

int main() {
	system("clear");
	initial_page();
	puts("Press any key to start...");
	getch();
	start_game();
	return 0;
}


