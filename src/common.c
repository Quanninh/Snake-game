#include "common.h"
#include "linked_list.h"
#include <stdlib.h>

// TODO: define global variables needed for your snake! (part 1A)
int g_snake_head_x = 2;
int g_snake_head_y = 2; // Initialize the head of the snake

int g_snake_direction = INPUT_RIGHT; // Initial the direction (Default Right)
// Definition of global variables for game status.
int g_game_over;
int g_score;

size_t g_name_len;        // Example: length of "PLAYER1"
const char *g_name; // Example: player's name

/** Sets the seed for random number generation.
 * Arguments:
 *  - `seed`: the seed.
 */
void set_seed(unsigned seed) {
    /* DO NOT MODIFY THIS FUNCTION */
    srand(seed);
    /* DO NOT MODIFY THIS FUNCTION */
}

/** Returns a random index in [0, size)
 * Arguments:
 *  - `size`: the upper bound for the generated value (exclusive).
 */
unsigned generate_index(unsigned size) {
    /* DO NOT MODIFY THIS FUNCTION */
    return rand() % size;
    /* DO NOT MODIFY THIS FUNCTION */
}
