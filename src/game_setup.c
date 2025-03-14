#include "game_setup.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> //Addition

// Some handy dandy macros for decompression
#define E_CAP_HEX 0x45
#define E_LOW_HEX 0x65
#define S_CAP_HEX 0x53
#define S_LOW_HEX 0x73
#define W_CAP_HEX 0x57
#define W_LOW_HEX 0x77
#define DIGIT_START 0x30
#define DIGIT_END 0x39

/** Initializes the board with walls around the edge of the board.
 *
 * Modifies values pointed to by cells_p, width_p, and height_p and initializes
 * cells array to reflect this default board.
 *
 * Returns INIT_SUCCESS to indicate that it was successful.
 *
 * Arguments:
 *  - cells_p: a pointer to a memory location where a pointer to the first
 *             element in a newly initialized array of cells should be stored.
 *  - width_p: a pointer to a memory location where the newly initialized
 *             width should be stored.
 *  - height_p: a pointer to a memory location where the newly initialized
 *              height should be stored.
 */
enum board_init_status initialize_default_board(int** cells_p, size_t* width_p,
                                                size_t* height_p) {
    *width_p = 30;
    *height_p = 20;
    int* cells = malloc(30 * 20 * sizeof(int));
    *cells_p = cells;
    for (int i = 0; i < 30 * 20; i++) {
        cells[i] = FLAG_PLAIN_CELL;
    }

    // Set edge cells!
    // Top and bottom edges:
    for (int i = 0; i < 30; ++i) {
        cells[i] = FLAG_WALL;
        cells[i + (30 * (20 - 1))] = FLAG_WALL;
    }
    // Left and right edges:
    for (int i = 0; i < 20; ++i) {
        cells[i * 30] = FLAG_WALL;
        cells[i * 30 + 30 - 1] = FLAG_WALL;
    }

    // Add snake
    cells[30 * 2 + 2] = FLAG_SNAKE;
    
    

    return INIT_SUCCESS;
}

/** Initialize variables relevant to the game board.
 * Arguments:
 *  - cells_p: a pointer to a memory location where a pointer to the first
 *             element in a newly initialized array of cells should be stored.
 *  - width_p: a pointer to a memory location where the newly initialized
 *             width should be stored.
 *  - height_p: a pointer to a memory location where the newly initialized
 *              height should be stored.
 *  - snake_p: a pointer to your snake struct (not used until part 2!)
 *  - board_rep: a string representing the initial board. May be NULL for
 * default board.
 */
enum board_init_status initialize_game(int** cells_p, size_t* width_p,
                                       size_t* height_p, snake_t* snake_p,
                                       char* board_rep) {
    // TODO: implement!
    

enum board_init_status status;
    // Use the default board initialization for now
    if (board_rep == NULL) {
        status = initialize_default_board(cells_p, width_p, height_p);
        snake_p->head_x = 2;
        snake_p->head_y = 2;
        snake_p->direction = INPUT_RIGHT;
        snake_p->length = 1;
        snake_p->body = NULL;
        int* head_pos = malloc(2 * sizeof(int));
        if (head_pos == NULL) {
            return INIT_ERR_BAD_CHAR;
        }
        head_pos[0] = snake_p->head_x;
        head_pos[1] = snake_p->head_y;
        insert_first(&snake_p->body, head_pos, sizeof(int) * 2);
    } else {
        // Future implementation: decompress board from board_rep
        status = decompress_board_str(cells_p, width_p, height_p, snake_p, board_rep);
    }
    // If the board initialization failed, return the error status
    if (status != INIT_SUCCESS) {
        return status;
    }
	g_game_over = 0;
	g_score = 0;
	
	// Initialize snake's data
	//g_snake_head_x = 2;
	//g_snake_head_y = 2;
	//g_snake_direction = INPUT_RIGHT;
    snake_p->direction = INPUT_RIGHT;
    // Initialize snake food
	place_food(*cells_p, *width_p, *height_p);
    return INIT_SUCCESS;
}

/** Takes in a string `compressed` and initializes values pointed to by
 * cells_p, width_p, and height_p accordingly. Arguments:
 *      - cells_p: a pointer to the pointer representing the cells array
 *                 that we would like to initialize.
 *      - width_p: a pointer to the width variable we'd like to initialize.
 *      - height_p: a pointer to the height variable we'd like to initialize.
 *      - snake_p: a pointer to your snake struct (not used until part 2!)
 *      - compressed: a string that contains the representation of the board.
 * Note: We assume that the string will be of the following form:
 * B24x80|E5W2E73|E5W2S1E72... To read it, we scan the string row-by-row
 * (delineated by the `|` character), and read out a letter (E, S or W) a number
 * of times dictated by the number that follows the letter.
 */
enum board_init_status decompress_board_str(int** cells_p, size_t* width_p,
                                            size_t* height_p, snake_t* snake_p,
                                            char* compressed) {
    // TODO: implement!
    if (!compressed || !cells_p || !width_p || !height_p || !snake_p) {
        return INIT_ERR_BAD_CHAR; // Handle null pointers.
    }

    // Parse dimensions
    size_t width, height;
    if (sscanf(compressed, "B%zux%zu|", &height, &width) != 2 || height == 0 || width == 0) {
        return INIT_ERR_BAD_CHAR; // Malformed dimensions or zero values.
    }

    *width_p = width;
    *height_p = height;

    // Allocate memory for the board
    *cells_p = calloc(height * width, sizeof(int));
    if (!*cells_p) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize snake state
    size_t snake_count = 0;
    snake_p->length = 0;
    snake_p->body = malloc(sizeof(node_t));
    
    // Decode RLE string
    size_t current_row = 0, current_col = 0;
    char *rle_start = strchr(compressed, '|') + 1;
    if (!rle_start) {
        free(*cells_p);
        return INIT_ERR_BAD_CHAR;
    }

    for (char* ptr = rle_start; *ptr; ) {
        char cell_type = *ptr++;
        if (cell_type != 'W' && cell_type != 'E' && cell_type != 'S') {
            free(*cells_p);
            return INIT_ERR_BAD_CHAR;
        }

        if (!isdigit(*ptr)) {
            free(*cells_p);
            return INIT_ERR_BAD_CHAR;
        }

        size_t run_length = strtoul(ptr, &ptr, 10);
        for (size_t i = 0; i < run_length; ++i) {
            if (current_row >= height || current_col >= width) {
                free(*cells_p);
                return INIT_ERR_INCORRECT_DIMENSIONS;
            }

            int cell_value = 0;
            if (cell_type == 'W') {
                cell_value = FLAG_WALL;
            } else if (cell_type == 'E') {
                cell_value = FLAG_PLAIN_CELL;
            } else { // 'S'
                if (++snake_count > 1) {
                    free(*cells_p);
                    return INIT_ERR_WRONG_SNAKE_NUM;
                }
                cell_value = FLAG_SNAKE;
                //g_snake_head_x = current_col; // chỗ này thay tọa độ
                //g_snake_head_y = current_row;
                snake_p->head_x = current_col;
                snake_p->head_y = current_row;
                snake_p->direction = INPUT_RIGHT;
                snake_p->length = 1;
                snake_p->body = NULL;
                int* head_pos = malloc(2 * sizeof(int));
                head_pos[0] = snake_p->head_x;
                head_pos[1] = snake_p->head_y;
                insert_first(&snake_p->body, head_pos, sizeof(int) * 2);
            }

            (*cells_p)[current_row * width + current_col] = cell_value;

            if (++current_col == width) {
                current_col = 0;
                ++current_row;
            }
        }

        if (*ptr == '|') {
            ++ptr; // Move past the delimiter
        }
    }

    // Final validation
    if (current_row != height || current_col != 0) {
        free(*cells_p);
        free(snake_p->body->data);
        return INIT_ERR_INCORRECT_DIMENSIONS;
    }

    if (snake_count != 1) {
        free(*cells_p);
        free(snake_p->body->data);
        return INIT_ERR_WRONG_SNAKE_NUM;
    }

    return INIT_SUCCESS;
    //return INIT_UNIMPLEMENTED;
} 