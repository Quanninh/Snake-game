#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "game_setup.h"
#include "linked_list.h"
#include "mbstrings.h"

/** Updates the game by a single step, and modifies the game information
 * accordingly. Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - width: width of the board.
 *  - height: height of the board.
 *  - snake_p: pointer to your snake struct (not used until part 2!)
 *  - input: the next input.
 *  - growing: 0 if the snake does not grow on eating, 1 if it does.
 */
void update(int* cells, size_t width, size_t height, snake_t* snake_p,
            enum input_key input, int growing) {
    // update should update the board, your snake's data, and global
    // variables representing game information to reflect new state. If in the
    // updated position, the snake runs into a wall or itself, it will not move
    // and global variable g_game_over will be 1. Otherwise, it will be moved
    // to the new position. If the snake eats food, the game score (g_score)
    // increases by 1. This function assumes that the board is surrounded by
    // walls, so it does not handle the case where a snake runs off the board.

    // TODO: implement!
    // Do nothing if the game is over
    if (g_game_over) {
        return;
    }

	size_t head_x = snake_p->head_x;  // Initial head position (x-coordinate)
    size_t head_y = snake_p->head_y ; // Initial head position (y-coordinate)
	//static enum input_key current_direction = INPUT_RIGHT;  // Start moving right

    // Update the direction only if the new input is valid
    if (growing){
        if (input == INPUT_UP && snake_p->direction != INPUT_DOWN) {
            snake_p->direction = input;
        } else if (input == INPUT_DOWN && snake_p->direction != INPUT_UP) {
            snake_p->direction = input;
        } else if (input == INPUT_LEFT && snake_p->direction != INPUT_RIGHT) {
            snake_p->direction = input;
        } else if (input == INPUT_RIGHT && snake_p->direction != INPUT_LEFT) {
            snake_p->direction = input;
        }
    }
    else{
        if (input == INPUT_UP) {
            snake_p->direction = input;
        } else if (input == INPUT_DOWN) {
            snake_p->direction = input;
        } else if (input == INPUT_LEFT) {
            snake_p->direction = input;
        } else if (input == INPUT_RIGHT) {
            snake_p->direction = input;
        }
    }
    
    // Determine new head position based on current movement direction
    int new_head_x = head_x;
    int new_head_y = head_y;

    switch (snake_p->direction) {
        case INPUT_UP:    new_head_y--; break;
        case INPUT_DOWN:  new_head_y++; break;
        case INPUT_LEFT:  new_head_x--; break;
        case INPUT_RIGHT: new_head_x++; break;
        default:          new_head_x++;  // Default movement is to the right
    }

    // Check for collisions with walls
    if (cells[new_head_y * width + new_head_x] & FLAG_WALL) {
        g_game_over = 1;  // End the game if the snake hits a wall
        return;
    }

    // Check for collisions with itself
    if (cells[new_head_y * width + new_head_x] & FLAG_SNAKE) {
        int* tail = (int*)get_last(snake_p->body);
        if(!(new_head_x == tail[0] && new_head_y == tail[1])){
            g_game_over = 1;  // End the game if the snake hits itself
            return;
        } 
    }

    // Check if the snake eats food
    int ate_food = (cells[new_head_y * width + new_head_x] & FLAG_FOOD);
    if (ate_food) {
        g_score++;  // Increment score
        place_food(cells, width, height);  // Place new food
    }
    
    int* new_head_pos = malloc(2 * sizeof(int));
    if (!new_head_pos) {
        return;  // Exit if memory allocation fails
    }
    new_head_pos[0] = new_head_x;
    new_head_pos[1] = new_head_y;
    insert_first(&snake_p->body, new_head_pos, sizeof(int) * 2);
    cells[new_head_y * width + new_head_x] = FLAG_SNAKE;

    if (!growing || !ate_food) {
        int* tail_pos = (int*)remove_last(&snake_p->body);
        if (tail_pos) {
            cells[tail_pos[1] * width + tail_pos[0]] = FLAG_PLAIN_CELL;
            free(tail_pos);
        }
    } else {
        snake_p->length++;
    }


    // Update the board and snake position
    cells[new_head_y * width + new_head_x] = FLAG_SNAKE;  // Mark new head
    if (!growing) {
        cells[head_y * width + head_x] = FLAG_PLAIN_CELL;  // Clear old head
    }

    // Update snake's head position
    snake_p->head_x = new_head_x;
    snake_p->head_y = new_head_y;
}

/** Sets a random space on the given board to food.
 * Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - width: the width of the board
 *  - height: the height of the board
 */
void place_food(int* cells, size_t width, size_t height) {
    /* DO NOT MODIFY THIS FUNCTION */
    unsigned food_index = generate_index(width * height);
    if (*(cells + food_index) == FLAG_PLAIN_CELL) {
        *(cells + food_index) = FLAG_FOOD;
    } else {
        place_food(cells, width, height);
    }
    /* DO NOT MODIFY THIS FUNCTION */
}

/** Prompts the user for their name and saves it in the given buffer.
 * Arguments:
 *  - write_into: a pointer to the buffer to be written into.
 */
void read_name(char* write_into) {
    // TODO: implement! (remove the call to strcpy once you begin your
    // implementation)
     while (1) { // Loop until valid input is received
        // Prompt the user for their name
        printf("Name > ");
        fflush(stdout); // Ensure the prompt is displayed immediately

        // Temporary buffer for input
        char buffer[1000] = {0};

        // Read input from standard input (file descriptor 0)
        ssize_t bytes_read = read(0, buffer, sizeof(buffer) - 1);

        // Handle read errors or EOF
        if (bytes_read < 0) {
            if (errno == EINTR) { // Interrupted system call, retry
                continue;
            }
            perror("Error reading input");
            break; // Exit loop on unrecoverable read error
        } else if (bytes_read == 0) {
            // Handle EOF (end-of-file)
            printf("\nError: EOF received. Exiting.\n");
            break;
        }

        // Remove trailing newline, if present
        if (buffer[bytes_read - 1] == '\n') {
            buffer[bytes_read - 1] = '\0';
        }

        // Validate input (check for empty input)
        if (strlen(buffer) > 0) {
            // Copy valid input into the output buffer
            strcpy(write_into, buffer);
            return; // Exit the function on valid input
        } else {
            // Print an error message and reprompt
            printf("Name Invalid: must be longer than 0 characters.\n");
        }
    }
    
    strcpy(write_into, "placeholder");
}

/** Cleans up on game over — should free any allocated memory so that the
 * LeakSanitizer doesn't complain.
 * Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - snake_p: a pointer to your snake struct. (not needed until part 2)
 */
void teardown(int* cells, snake_t* snake_p) {
    // TODO: implement!
}
