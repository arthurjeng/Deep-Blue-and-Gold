typedef struct board_ptr {
    char *val;
    struct board_ptr *prev;
    struct board_ptr *next;
} Board_ptr;

typedef struct move {
    int frm;
    int to;
    struct move *next;
} Moves;