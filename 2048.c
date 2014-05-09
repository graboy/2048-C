#include <stdio.h>
#include <stddef.h>
#include <inttypes.h>
#include <stdlib.h>

typedef struct Tile_ Tile;

struct Tile_ {
    uint16_t value;
    Tile *(neighbor[4]);
};

enum Direction { NORTH, SOUTH, EAST, WEST };

void collapse(Tile *t, enum Direction dir);
Tile *make_tile(uint16_t value);

void collapse(Tile *t, enum Direction dir)
{
    Tile *mov = t;
    Tile *nxt = mov->neighbor[dir];
    while (nxt != NULL) {
        if (nxt->value == 0) {
            /* Slide into empty slot */
            nxt->value = mov->value;
            mov->value = 0;
        } else if (nxt->value == mov->value) {
            /* Join mov and nxt */
            mov->value = 0;
            nxt->value <<= 1;
            collapse(nxt, dir);
            break;
        } else {
            /* Collision: move others forward */
            collapse(nxt, dir);
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
    int c = argc - 1;

    Tile *(tiles[c]);
    int i;
    for (i = 0; i < c; i++) {
        int value;
        sscanf(argv[i+1], "%i", &value);
        tiles[i] = make_tile(value);
    }

    /* setup Tile->neighbor[] */
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
    tiles[c - 1]->neighbor[WEST] = tiles[c - 2];
    tiles[c - 1]->neighbor[EAST] = NULL;

    collapse(tiles[0], EAST);

    for (i = 0; i < c; i++)
        printf("%i ", tiles[i]->value);
    putchar('\n');

    for (i = 0; i < c; i++)
        free(tiles[i]);

    return 0;
}
