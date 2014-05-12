#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <inttypes.h>
#include <stdlib.h>

#define BOARD_FULL 1

typedef struct Tile_ Tile;
typedef struct Board_ Board;

struct Tile_ {
    uint16_t value;
    Tile *(neighbor[4]);
};

struct Board_ {
    Tile **tile;
};

enum Direction { NORTH, SOUTH, EAST, WEST };

int boardsize;

void collapse(Tile *t, enum Direction dir);
Board *make_empty_board(void);
Board *make_board_move(Board *board, enum Direction dir);
int make_move(enum Direction dir);

bool boards_are_equal(Board *b1, Board *b2);
Board *clone_board(Board *b);
void destroy_board(Board *b);
void setup_neighbors(Board *b);
void print_board(Board *b);
int place_random_block(Board *b);

int place_random_block(Board *b)
{
    int emptycount = 0;
    Tile *(empty[boardsize * boardsize]);

    for (int x = 0; x < boardsize; x++) {
        for (int y = 0; y < boardsize; y++) {
            if (b->tile[x][y].value == 0)
                empty[emptycount++] = &b->tile[x][y];
        }
    }

    if (emptycount == 0)
        return BOARD_FULL;

    int rand = random() % emptycount;
    int value = ((random() % 10) == 0) ? 4 : 2;

    empty[rand]->value = value;
}

Board *make_empty_board(void)
{
    Board *b = malloc(sizeof(Board));
    if (b == NULL) goto badmalloc;

    b->tile = malloc(sizeof(Tile *) * boardsize);
    if (b->tile == NULL) goto badmalloc;

    for (int x = 0; x < boardsize; x++) {
        b->tile[x] = malloc(sizeof(Tile) * boardsize);
        if (b->tile[x] == NULL) goto badmalloc;
    }

    setup_neighbors(b);

    return b;

badmalloc: {
    printf("ERROR: bad malloc in function \"make_empty_board\". Exiting...");
    exit(1);
    }
}

void destroy_board(Board *b)
{
    for (int x = 0; x < boardsize; x++) {
            free(b->tile[x]);
    }
    free(b);
}

void print_board(Board *b)
{
    printf("\n");
    for (int y = 0; y < boardsize; y++) {
        printf("\n");
        for (int x = 0; x < boardsize; x++) {
            printf("%4i ", b->tile[x][y].value);
        }
        printf("\n");
    }
}

void setup_neighbors(Board *b)
{
    for (int x = 0; x < boardsize; x++) {
        for (int y = 0; y < boardsize; y++) {
            if (y < boardsize - 1)
                b->tile[x][y].neighbor[NORTH] = &b->tile[x][y+1];
            else
                b->tile[x][y].neighbor[NORTH] = NULL;

            if (y > 0)
                b->tile[x][y].neighbor[SOUTH] = &b->tile[x][y-1];
            else
                b->tile[x][y].neighbor[SOUTH] = NULL;

            if (x > 0)
                b->tile[x][y].neighbor[WEST]  = &b->tile[x-1][y];
            else
                b->tile[x][y].neighbor[WEST]  = NULL;

            if (x < boardsize - 1)
                b->tile[x][y].neighbor[EAST]  = &b->tile[x+1][y];
            else
                b->tile[x][y].neighbor[EAST]  = NULL;
        }
    }
}


bool boards_are_equal(Board *b1, Board *b2)
{
    for (int x = 0; x < boardsize; x++) {
        for (int y = 0; y < boardsize; y++) {
            if (b1->tile[x][y].value != b2->tile[x][y].value)
                return false;
        }
    }

    return true;
}

Board *clone_board(Board *b)
{
    Board *nb = make_empty_board();

    for (int x = 0; x < boardsize; x++) {
        for (int y = 0; y < boardsize; y++) {
                nb->tile[x][y].value = b->tile[x][y].value;
        }
    }


    return nb;
}

/* Returns new board if good, null if move isn't available */
Board *make_board_move(Board *old, enum Direction dir)
{
    Board *new = clone_board(old);

    switch (dir) {
    case NORTH:
        for (int x = 0; x < boardsize; x++)
            collapse(&new->tile[x][boardsize-1], SOUTH);
        break;

    case SOUTH:
        for (int x = 0; x < boardsize; x++)
            collapse(&new->tile[x][0], NORTH);
        break;

    case EAST:
        for (int y = 0; y < boardsize; y++)
            collapse(&new->tile[0][y], EAST);
        break;

    case WEST:
        for (int y = 0; y < boardsize; y++)
            collapse(&new->tile[boardsize-1][y], WEST);
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
    while (nxt != NULL && nxt->value == 0) {
            /* Slide into empty slot */
            nxt->value = mov->value;
            mov->value = 0;
            mov = nxt;
            nxt = nxt->neighbor[dir];
    }

    if (nxt != NULL)
        collapse(nxt, dir);
    else
        return;

    while (nxt != NULL && nxt->value == 0) {
            /* Slide into empty slot */
            nxt->value = mov->value;
            mov->value = 0;
            mov = nxt;
            nxt = nxt->neighbor[dir];
    }

    if (mov->value == nxt->value) {
        /* Join mov and nxt */
        mov->value = 0;
        nxt->value <<= 1;
    }
}

int main(int argc, char *argv[])
{

    if (argc == 1) {
        boardsize = 4;
    } else if (argc == 2) {
        sscanf(argv[1], "%i", &boardsize);
    } else {
        printf("Format: %s <board size>\n", argv[0]);
        exit(0);
    }

    Board *board = make_empty_board();

    place_random_block(board);

    print_board(board);

    char in;
    enum Direction dir;
    while (scanf("%c", &in) != EOF) {
        if  (in   ==   'w')
            dir = NORTH;
        else if (in == 's')
            dir = SOUTH;
        else if (in == 'd')
            dir = EAST;
        else if (in == 'a')
            dir = WEST;
        else if (in == 'q')
            break;
        else
            continue;

        Board *newboard;

        /* If move is not valid */
        if ((newboard = make_board_move(board, dir)) == NULL) {
            continue;
        } else {
            destroy_board(board);
            board = newboard;
            place_random_block(board);
        }

        print_board(board);
    }

    destroy_board(board);

    return 0;
}
