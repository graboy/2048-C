#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <inttypes.h>
#include <stdlib.h>

typedef struct Tile_ Tile;
typedef struct Board_ Board;

struct Tile_ {
    uint16_t value;
    Tile *(neighbor[4]);
};

/* TODO board building function */
struct Board_ {
    Tile ***tile;
};

enum Direction { NORTH, SOUTH, EAST, WEST };

int boardsize;

void collapse(Tile *t, enum Direction dir);
Tile *make_tile(uint16_t value);
Board *make_board_move(Board *board, enum Direction dir);
int make_move(enum Direction dir);

bool boards_are_equal(Board *b1, Board *b2);
Board *clone_board(Board *b);
void destroy_board(Board *b);

void destroy_board(Board *b)
{
    for (int x = 0; x < boardsize; x++) {
        for (int y = 0; y < boardsize; y++) {
            free(b->tile[x][y]);
        }
    }
    free(b);
}

bool boards_are_equal(Board *b1, Board *b2)
{
    for (int x = 0; x < boardsize; x++) {
        for (int y = 0; y < boardsize; y++) {
            if (memcmp(b1->tile[x][y], b2->tile[x][y], sizeof(Tile)) != 0)
                return false;
        }
    }

    return true;
}

Board *clone_board(Board *b)
{
    Board *nb = malloc(sizeof(Board));
    for (int x = 0; x < boardsize; x++) {
        for (int y = 0; y < boardsize; y++) {
            nb->tile[x][y] = malloc(sizeof(Tile));
            memcpy(nb->tile[x][y], b->tile[x][y], sizeof(Tile));
        }
    }

    return nb;
}

/* Returns new board if good, null if it can't move */
Board *make_board_move(Board *old, enum Direction dir)
{
    Board *new = clone_board(old);

    switch (dir) {
    case NORTH:
        for (int i = 0; i < boardsize; i++)
            collapse(new->tile[i][boardsize-1], NORTH);
        break;

    case SOUTH:
        for (int i = 0; i < boardsize; i++)
            collapse(new->tile[i][0], SOUTH);
        break;

    case EAST:
        for (int i = 0; i < boardsize; i++)
            collapse(new->tile[0][i], EAST);
        break;

    case WEST:
        for (int i = 0; i < boardsize; i++)
            collapse(new->tile[boardsize-1][i], WEST);
        break;
    }

    if (boards_are_equal(new, old)) {
        destroy_board(new);
        new = NULL;
    }

    return new;
}

void collapse(Tile *t, enum Direction dir)
{
    Tile *mov = t;
    Tile *nxt = mov->neighbor[dir];
    while (nxt != NULL) {
        if (nxt->value == 0) {
            /* Slide into empty slot */
            nxt->value = mov->value;
            mov->value = 0;
        } else {
            collapse(nxt, dir);
            if (nxt->value == mov->value) {
                /* Join mov and nxt */
                mov->value = 0;
                nxt->value <<= 1;
            }
            break;
        }
        mov = nxt;
        nxt = nxt->neighbor[dir];
    }

    while ((nxt != NULL) && (nxt->value == 0)) {
        /* Slide into empty slot */
        nxt->value = mov->value;
        mov->value = 0;
        mov = nxt;
        nxt = nxt->neighbor[dir];
    }
}

Tile *make_tile(uint16_t value)
{
    Tile *t = malloc(sizeof(Tile));
    if (t == NULL) {
        printf("ERROR: bad malloc in function \"make_tile\". Exiting...");
        exit(1);
    }
    t->value = value;
    return t;
}

int main(int argc, char *argv[])
{
    Board *board;

    if (argc == 1) {
        boardsize = 4;
    } else if (argc == 2) {
        sscanf(argv[1], "%i", &boardsize);
    } else {
        printf("Format: %s <board size>", argv[0]);
    }

    for (int i = 0; i < boardsize * boardsize; i++)
        board->tile[0][i] = make_tile(0);

    char in;
    enum Direction dir;
    while (scanf("%c", &in) != EOF) {
        if (in == 'w')
            dir = NORTH;
        else if (in == 's')
            dir = SOUTH;
        else if (in == 'd')
            dir = EAST;
        else if (in == 'a')
            dir = WEST;
        else
            /* TODO */ ;

        Board *newboard;

        /* If move is not valid */
        if ((newboard = make_board_move(board, dir)) == NULL) {
            continue;
        }


    }


    /*
    int c = argc - 1;

    Tile *(tiles[c]);
    int i;
    for (i = 0; i < c; i++) {
        int value;
        sscanf(argv[i+1], "%i", &value);
        tiles[i] = make_tile(value);
    }

    // setup Tile->neighbor[]
    for (i = 0; i < c; i++) {
        tiles[i]->neighbor[NORTH] = NULL;
        tiles[i]->neighbor[SOUTH] = NULL;
    }

    for (i = 1; i < c - 1; i++) {
        tiles[i]->neighbor[EAST] = tiles[i+1];
        tiles[i]->neighbor[WEST] = tiles[i-1];
    }
    tiles[0]->neighbor[EAST] = tiles[1];
    tiles[0]->neighbor[WEST] = NULL;
    tiles[c-1]->neighbor[WEST] = tiles[c-2];
    tiles[c-1]->neighbor[EAST] = NULL;

    collapse(tiles[0], EAST);

    for (i = 0; i < c; i++)
        printf("%i ", tiles[i]->value);
    putchar('\n');

    for (i = 0; i < c; i++)
        free(tiles[i]);
    */

    return 0;
}
