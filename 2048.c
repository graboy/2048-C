#include <stdio.h>

typedef struct Tile_ Tile;

struct Tile_ {
    int value;
    Tile *(neighbor[4]);
};

enum Direction { N, S, E, W };

void collapse(Tile *t, enum Direction dir);

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
    }
}
