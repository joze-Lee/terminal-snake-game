#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_t *game, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_t *game, unsigned int snum);
static char next_square(game_t *game, unsigned int snum);
static void update_tail(game_t *game, unsigned int snum);
static void update_head(game_t *game, unsigned int snum);

/* Task 1 */
// game_t *create_default_game() {
//   game_t *game = malloc(sizeof(game_t));
//   game->num_rows = 18;
//   game->board = malloc(sizeof(char *) * game->num_rows);
//   for (int i = 0; i < game->num_rows; i++) {
//       game->board[i] = malloc(sizeof(char) * 22);  
//       strcpy(game->board[i], "#                  #\n");
//   }
//   strcpy(game->board[0],  "####################\n");
//   strcpy(game->board[2],  "# d>D    *         #\n");
//   strcpy(game->board[17], "####################\n");

//   game->num_snakes = 1;
//   game->snakes = malloc(sizeof(snake_t) * game->num_snakes);

//   game->snakes[0].head_row = 2;
//   game->snakes[0].head_col = 4;
//   game->snakes[0].tail_row = 2;
//   game->snakes[0].tail_col = 2;
//   game->snakes[0].live = true;

//   return game;
// }
char *strdup(const char *s) {
  size_t len = strlen(s) + 1;
  char *dup = malloc(len);
  if (dup) {
      strcpy(dup, s);
  }
  return dup;
}
game_t *create_default_game() {
  game_t *game = malloc(sizeof(game_t));
  if (game == NULL) {
      return NULL;
  }

  game->num_rows = 18;
  game->board = malloc(sizeof(char *) * game->num_rows);
  if (game->board == NULL) {
      free(game);
      return NULL;
  }

  for (int i = 0; i < game->num_rows; i++) {
      game->board[i] = malloc(sizeof(char) * 22);  // 21 chars + null terminator
      if (game->board[i] == NULL) {
          for (int j = 0; j < i; j++) free(game->board[j]);
          free(game->board);
          free(game);
          return NULL;
      }
      strcpy(game->board[i], "#                  #\n");
  }

  strcpy(game->board[0],  "####################\n");
  strcpy(game->board[2],  "# d>D    *         #\n");
  strcpy(game->board[17], "####################\n");

  // Do NOT manually set num_snakes or snake positions.
  // Let initialize_snakes discover them.
  if (initialize_snakes(game) == NULL) {
      // If it fails, free memory and return NULL
      for (int i = 0; i < game->num_rows; i++) free(game->board[i]);
      free(game->board);
      free(game);
      return NULL;
  }

  return game;
}



/* Task 2 */
void free_game(game_t *game) {
  // TODO: Implement this function.
  for (int i= 0; i < game-> num_rows; i++) {
      free(game->board[i]);
  }
  free(game-> board);
  free(game-> snakes);
  free(game); 
  return;
}

/* Task 3 */
void print_board(game_t *game, FILE *fp) {
  // TODO: Implement this function.
  for (int i = 0; i < game-> num_rows; i ++) {
    fprintf(fp,"%s", game-> board[i]);
  }
  return;
}

/*
  Saves the current game into filename. Does not modify the game object.
  (already implemented for you).
*/
void save_board(game_t *game, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(game, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_t *game, unsigned int row, unsigned int col) { return game->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_t *game, unsigned int row, unsigned int col, char ch) {
  game->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  char *choice = "wasd";
  for (int i = 0; i < strlen(choice); i++ ) {
      if(choice[i] == c) {
          return true; 
      }
  }

  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  char *choice = "WASDx"; 
  for (int i = 0; i < strlen(choice); i++ ) {
      if (choice[i] == c) {
          return true; 
      }
  }
  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  char * choices = "WASDwasd<v>^x";
  for(int i = 0; i < strlen(choices); i++) {
      if(c==choices[i]) {
          return true;
      }
  }
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  char *body = "^<v>";
  char *tail = "wasd"; 
  for (int i = 0; i < strlen(body); i++) {
      if(body[i]==c) {
        c = tail[i];
        break;
      }
  }
    
  return c;
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  char* head = "WASD";
  char* body = "^<v>";
  for (int i = 0; i < strlen(head); i++) {
      if (c==head[i]) {
          c= body[i]; 
          break;
      }
  }
  return c;
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S') {
      return cur_row + 1;
  } else if (c == '^' || c == 'w' || c == 'W') {
      return cur_row - 1;
  } else {
      return cur_row;
  }
}


/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D') {
      return cur_col + 1;
  } else if (c == '<' || c == 'a' || c == 'A') {
      return cur_col - 1;
  } else {
      return cur_col;
  }
}

/*
  Task 4.2

  Helper function for update_game. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  snake_t snake = game->snakes[snum];
  unsigned int head_r = snake.head_row;
  unsigned int head_c = snake.head_col;
  char head = game->board[head_r][head_c];
  char next_square = '?';
  unsigned int next_sr = get_next_row(head_r, head);
  unsigned int next_sc = get_next_col(head_c, head);
  
  next_square = game->board[next_sr][next_sc];
  return next_square;
}

/*
  Task 4.3
  Helper function for update_game. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_t *game, unsigned int snum) {
  snake_t *snake = &game->snakes[snum];

  if (!snake->live) {
      return;  // Don't update a dead snake
  }

  unsigned int head_row = snake->head_row;
  unsigned int head_col = snake->head_col;
  char head_char = game->board[head_row][head_col];

  unsigned int next_row = get_next_row(head_row, head_char);
  unsigned int next_col = get_next_col(head_col, head_char);

  // char next_char = game->board[next_row][next_col];

  // Move head: current head becomes body, new cell becomes new head
  game->board[head_row][head_col] = head_to_body(head_char);
  game->board[next_row][next_col] = head_char;

  // Update snake struct
  snake->head_row = next_row;
  snake->head_col = next_col;
}


/*
  Task 4.4

  Helper function for update_game. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_t *game, unsigned int snum) {
  unsigned int x = game->snakes[snum].tail_row;
  unsigned int y = game->snakes[snum].tail_col;
  char curr_tail = get_board_at(game, x, y);

  // Convert tail character to body direction
  char dir;
  switch (curr_tail) {
      case 'w': dir = '^'; break;
      case 'a': dir = '<'; break;
      case 's': dir = 'v'; break;
      case 'd': dir = '>'; break;
      default: return;  // Invalid tail
  }

  // Follow the tail
  unsigned int new_x = get_next_row(x, dir);
  unsigned int new_y = get_next_col(y, dir);
  char next_body = get_board_at(game, new_x, new_y);

  // Only promote valid body character to tail
  if (next_body == '^' || next_body == '<' || next_body == 'v' || next_body == '>') {
      char new_tail = body_to_tail(next_body);
      set_board_at(game, new_x, new_y, new_tail);
  }

  // Clear the old tail
  set_board_at(game, x, y, ' ');

  // Update snake struct
  game->snakes[snum].tail_row = new_x;
  game->snakes[snum].tail_col = new_y;
}
 



/* Task 4.5 */
void update_game(game_t *game, int (*add_food)(game_t *game)) {
  // TODO: Implement this function.
  for (unsigned int i = 0; i < game->num_snakes; i++) {
      if (game->snakes[i].live) {
          char next = next_square(game, i);

          if (next == '*') {
              update_head(game, i);
              add_food(game);
          } else if (is_snake(next) || next == '#') {
              set_board_at(game, game->snakes[i].head_row, game->snakes[i].head_col, 'x');
              game->snakes[i].live = false;
          } else {
              update_head(game, i);
              update_tail(game, i);
          }
      }
  }

  return;
}



/* Task 5.1 */

/* Task 5.1 */
char *read_line(FILE *fp) {
  size_t buf_size = 64;
  size_t line_size = 0;
  char *buf = malloc(buf_size * sizeof(char));
  if (buf == NULL) {
      return NULL;
  }

  char *result = NULL;

  while (fgets(buf + line_size, (int)(buf_size - line_size), fp) != NULL) {

      line_size = strlen(buf);

      if (buf[line_size - 1] == '\n') {
          result = buf;
          break;
      }

      char *tmp = realloc(buf, buf_size * 2);
      if (tmp == NULL) {
          free(buf);
          return NULL;
      }

      buf = tmp;
      buf_size *= 2;
  }

  if (result == NULL && line_size > 0) {
      result = buf; // Last line without newline
  }

  if (result == NULL) {
      free(buf);
      return NULL;
  }

  if (buf_size == line_size) {
      result = realloc(result, strlen(result) + 1);
  }

  result[line_size] = '\0';
  return result;
}





/* Task 5.2 */
game_t *load_board(FILE *fp) {
  // TODO: Implement this function.
  game_t *game = malloc(sizeof(game_t));
  game->num_rows = 0;
  game->board = NULL;
  game->num_snakes = 0; 
  game->snakes= NULL;
  char *line;
  size_t capacity = 4;
  game->board = malloc(sizeof(char*) * capacity);
  while((line = read_line(fp))!= NULL) {
      if(game-> num_rows >= capacity) {
          capacity*=2;
          game->board = realloc(game->board, sizeof(char*) * capacity);
      }
      // game->board[game->num_rows++]= line;
      game->board[game->num_rows++] = strdup(line);
      free(line);

  }

  return game;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/


/*static void find_head(game_t *game, unsigned int snum) {
  snake_t *snake = &game->snakes[snum];
  unsigned int row = snake->tail_row;
  unsigned int col = snake->tail_col;
  char ch = game->board[row][col];

  int steps = 0;
  int max_steps = game->num_rows * 100;

  while (!is_head(ch)) {
    if (++steps > max_steps) {
      fprintf(stderr, "Skipping snake %u: exceeded step limit at (%u, %u), char = '%c'\n", snum, row, col, ch);
      snake->live = 0;
      return;
    }

    if (!is_snake(ch)) {
      fprintf(stderr, "Skipping snake %u: invalid character '%c' at (%u, %u)\n", snum, ch, row, col);
      snake->live = 0;
      return;
    }

    unsigned int next_row = get_next_row(row, ch);
    unsigned int next_col = get_next_col(col, ch);

    // Bounds check before accessing game->board[next_row][next_col]
    if (next_row >= game->num_rows || next_col >= strlen(game->board[next_row])) {
      fprintf(stderr, "Skipping snake %u: out-of-bounds move to (%u, %u)\n", snum, next_row, next_col);
      snake->live = 0;
      return;
    }

    row = next_row;
    col = next_col;
    ch = game->board[row][col];
  }

  snake->head_row = row;
  snake->head_col = col;
}*/
static void find_head(game_t *game, unsigned int snum) {
  unsigned int x = game->snakes[snum].tail_row;
  unsigned int y = game->snakes[snum].tail_col;

  while (!is_head(game->board[x][y])) {
      char c = game->board[x][y];
      unsigned int next_x = get_next_row(x, c);
      unsigned int next_y = get_next_col(y, c);

      // Out-of-bounds check to prevent invalid memory access
      if (next_x >= game->num_rows || next_y >= strlen(game->board[next_x])) {
          game->snakes[snum].live = false;
          return;
      }

      x = next_x;
      y = next_y;
  }

  game->snakes[snum].head_row = x;
  game->snakes[snum].head_col = y;
}




/* Task 6.2 */
game_t *initialize_snakes(game_t *state) {
  state->snakes = NULL;
  state->num_snakes = 0;
  
  int capacity = 4;
  state->snakes = malloc(sizeof(snake_t) * (size_t)capacity);
  if (state->snakes == NULL) {
    return NULL;
}

  for (unsigned int i = 0; i < state->num_rows; i++) {
      for (unsigned int j = 0; j < strlen(state->board[i]); j++) {
          if (!is_tail(get_board_at(state, i, j))) {
              continue;   
          }

          // state->snakes = realloc(state->snakes, sizeof(snake_t) * (state->num_snakes + 1));
       
         
          if (state->num_snakes >= capacity) {
            capacity *= 2;
            snake_t *temp = realloc(state->snakes, sizeof(snake_t) * (size_t)capacity);
            if (temp == NULL) {
                free(state->snakes);
                return NULL;
            }
            state->snakes = temp;
          }

          unsigned int snum = state->num_snakes; 
          state->snakes[snum].tail_row = i;
          state->snakes[snum].tail_col = j;
          state->snakes[snum].live = true;

          find_head(state, snum);

          state->num_snakes += 1;
      }
  }
 
  if (state->num_snakes > 0) {
    snake_t *trimmed = realloc(state->snakes, sizeof(snake_t) * state->num_snakes);
    if (trimmed != NULL) {
        state->snakes = trimmed;
    }
  }


  return state;
}
  /*unsigned int snum = 0;
  unsigned int *tail_rows = malloc(1000 * sizeof(unsigned int));
  unsigned int *tail_cols = malloc(1000 * sizeof(unsigned int));

  // Step 1: Find all tails
  for (unsigned int i = 1; i < game->num_rows; i++) {
    for (unsigned int j = 1; j < strlen(game->board[i]); j++) {
      if (is_tail(game->board[i][j])) {
        tail_rows[snum] = i;
        tail_cols[snum] = j;
        snum++;
      }
    }
  }

  // Step 2: Allocate exactly enough space for snakes
  game->num_snakes = snum;
  game->snakes = malloc(sizeof(snake_t) * snum);

  // Step 3: Initialize each snake and find its head
  for (unsigned int i = 0; i < snum; i++) {
    snake_t *s = &game->snakes[i];
    s->tail_row = tail_rows[i];
    s->tail_col = tail_cols[i];
    s->live = true;
    find_head(game, i);  // this sets head_row, head_col, or marks snake dead
  }

  free(tail_rows);
  free(tail_cols);
  return game;
}*/