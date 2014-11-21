#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "/home/sean/sketchbook/libraries/Time/Time.h"
#include <stdlib.h>

#include "/home/sean/sketchbook/chess/structs.h"

char *board_start = "RNBQKBNRPPPPPPPP                                "
                    "pppppppprnbqkbnr";

void printBoard(Board_ptr *board_ptr) {
    char str[80];
  
    char *board = board_ptr->val;
    Serial.print("   ---------------------------------\n");
    int i = 7;
    while (i >= 0) {
        sprintf(str, " %d |", i + 1);
        Serial.print(str);
        int j = 0;
        while (j < 8) {
            sprintf(str, " %c |", board[i*8 + j]);
            Serial.print(str);
            j++;
        }
        Serial.print("\n   ---------------------------------\n");
        i--;
    }
    Serial.print("     A   B   C   D   E   F   G   H  \n");
}

void resetBoard(Board_ptr *board_ptr) {
    char *board = board_ptr->val;
    int i = 0;
    while (i < strlen(board_start)) {
        board[i] = board_start[i];
        i++;
    }
}

/** Returns the opposite of COLOR. */
char oppositeColor(char col) {
    if (col == 'w') {
        return 'b';
    }
    return 'w';
}

char pieceColor(char piece) {
    if (piece == ' ') {
        return 'e';
    } else if (piece < 97) {
        return 'w';
    }
    return 'b';
}

/** Returns the color of the piece on SQUARE (i.e. "white"). */
char colorOfSquare(int pos, Board_ptr *board_ptr) {
    char *board = board_ptr->val;
    int i = pos / 8;
    int j = pos % 8;
    if (i < 0 || i > 7 || j < 0 || j > 7) {
        return 'e';
    }
    return pieceColor(board[i*8+j]);
}

/** Returns the number of points PIECE is worth. */
int valOfPiece(char piece) {
    if (piece == 'P' || piece == 'p') {
        return 1;
    } else if (piece == 'N' || piece == 'n') {
        return 3;
    } else if (piece == 'R' || piece == 'r') {
        return 5;
    } else if (piece == 'B' || piece == 'b') {
        return 3;
    } else if (piece == 'K' || piece == 'k') {
        return 9;
    } else if (piece == 'Q' || piece == 'q') {
        return 9;
    }
    return 0;
}

/** Given a square on the board (i.e. A6), returns the corresponding position
  * in the board array as a number ij. */
int getPosFromSquare(char *square, Board_ptr *board_ptr) {
    return (int) 8*(square[1]-49) + square[0]-65;
}

void getSquareFromPos(int pos, char *sq, Board_ptr *board_ptr) {
    sq[0] = (char) (65 + (pos % 8));
    sq[1] = (char) ((pos / 8) + 49);
    sq[2] = 0;
}

int isLegal(int frm_i, int frm_j, int to_i, int to_j, Board_ptr *board_ptr) {
    char *board = board_ptr->val;
    char col = colorOfSquare(frm_i*8+frm_j, board_ptr);
    // we cannot move off of the board
    if (to_i < 0 || to_i > 7 || to_j < 0 || to_j > 7) {
        return 0;
    }
    // we cannot capture our own pieces
    if (colorOfSquare(to_i*8+to_j, board_ptr) == col) {
        return 0;
    }
    return 1;
}

Moves *addMove(int frm, int to, Moves *move) {
    move->frm = frm;
    move->to = to;
    move->next = (Moves*) malloc(sizeof(Moves));
    return move->next;
}

Moves *genPawnMoves(Moves *move, char *square, char color, Board_ptr *board_ptr) {
    int pos = getPosFromSquare(square, board_ptr);
    int i = pos / 8;
    int j = pos % 8;
    int fwd = 1;
    int secondRow = 1;

    if (color == 'b') {
        fwd = -1;
        secondRow = 6;
    }

    // move forward one square
    if (isLegal(i, j, i+fwd, j, board_ptr) && colorOfSquare((i+fwd)*8+j, board_ptr) == 'e') {
        move = addMove(i*8+j, (i+fwd)*8+j, move);
    }
    // move forward two squares
    if (i == secondRow && isLegal(i, j, i+2*fwd, j, board_ptr)
        && colorOfSquare((i+2*fwd)*8+j, board_ptr) == 'e') {
        move = addMove(i*8+j, (i+2*fwd)*8+j, move);
    }
    // see if we can capture pieces
    if (isLegal(i, j, i+fwd, j+1, board_ptr) && colorOfSquare((i+fwd)*8+j+1, board_ptr) == oppositeColor(color)) {
        move = addMove(i*8+j, (i+fwd)*8+j+1, move);
    }
    // see if we can capture pieces
    if (isLegal(i, j, i+fwd, j-1, board_ptr) && colorOfSquare((i+fwd)*8+j-1, board_ptr) == oppositeColor(color)) {
        move = addMove(i*8+j, (i+fwd)*8+j-1, move);
    }

    return move;
}

Moves *genKnightMoves(Moves *move, char *square, char color, Board_ptr *board_ptr) {
    int pos = getPosFromSquare(square, board_ptr);
    int i = pos / 8;
    int j = pos % 8;

    if (isLegal(i, j, i+1, j+2, board_ptr)) {
        move = addMove(pos, (i+1)*8+j+2, move);
    }
    if (isLegal(i, j, i+1, j-2, board_ptr)) {
        move = addMove(pos, (i+1)*8+j-2, move);
    }
    if (isLegal(i, j, i-1, j+2, board_ptr)) {
        move = addMove(pos, (i-1)*8+j+2, move);
    }
    if (isLegal(i, j, i-1, j-2, board_ptr)) {
        move = addMove(pos, (i-1)*8+j-2, move);
    }
    if (isLegal(i, j, i+2, j+1, board_ptr)) {
        move = addMove(pos, (i+2)*8+j+1, move);
    }
    if (isLegal(i, j, i+2, j-1, board_ptr)) {
        move = addMove(pos, (i+2)*8+j-1, move);
    }
    if (isLegal(i, j, i-2, j+1, board_ptr)) {
        move = addMove(pos, (i-2)*8+j+1, move);
    }
    if (isLegal(i, j, i-2, j-1, board_ptr)) {
        move = addMove(pos, (i-2)*8+j-1, move);
    }
    return move;
}

Moves *genBishopMoves(Moves *move, char *square, char color, Board_ptr *board_ptr) {
    int pos = getPosFromSquare(square, board_ptr);
    int i = pos / 8;
    int j = pos % 8;

    int new_i = i+1;
    int new_j = j+1;
    while (new_i < 8 && new_j < 8) {
        if (isLegal(i, j, new_i, new_j, board_ptr)) {
            move = addMove(pos, new_i*8+new_j, move);
        }
        if (colorOfSquare(new_i*8+new_j, board_ptr) != 'e') {
            break;
        }
        new_i++;
        new_j++;
    }
    new_i = i-1;
    new_j = j-1;
    while (new_i >= 0 && new_j >= 0) {
        if (isLegal(i, j, new_i, new_j, board_ptr)) {
            move = addMove(pos, new_i*8+new_j, move);
        }
        if (colorOfSquare(new_i*8+new_j, board_ptr) != 'e') {
            break;
        }
        new_i--;
        new_j--;
    }
    new_i = i-1;
    new_j = j+1;
    while (new_i >= 0 && new_j >= 0) {
        if (isLegal(i, j, new_i, new_j, board_ptr)) {
            move = addMove(pos, new_i*8+new_j, move);
        }
        if (colorOfSquare(new_i*8+new_j, board_ptr) != 'e') {
            break;
        }
        new_i--;
        new_j++;
    }
    new_i = i+1;
    new_j = j-1;
    while (new_i >= 0 && new_j >= 0) {
        if (isLegal(i, j, new_i, new_j, board_ptr)) {
            move = addMove(pos, new_i*8+new_j, move);
        }
        if (colorOfSquare(new_i*8+new_j, board_ptr) != 'e') {
            break;
        }
        new_i++;
        new_j--;
    }

    return move;
}

Moves *genRookMoves(Moves *move, char *square, char color, Board_ptr *board_ptr) {
    int pos = getPosFromSquare(square, board_ptr);
    int i = pos / 8;
    int j = pos % 8;

    int new_i = i+1;
    int new_j = j;
    while (new_i < 8 && new_j < 8) {
        if (isLegal(i, j, new_i, new_j, board_ptr)) {
            move = addMove(pos, new_i*8+new_j, move);
        }
        if (colorOfSquare(new_i*8+new_j, board_ptr) != 'e') {
            break;
        }
        new_i++;
    }
    new_i = i-1;
    new_j = j;
    while (new_i >= 0 && new_j >= 0) {
        if (isLegal(i, j, new_i, new_j, board_ptr)) {
            move = addMove(pos, new_i*8+new_j, move);
        }
        if (colorOfSquare(new_i*8+new_j, board_ptr) != 'e') {
            break;
        }
        new_i--;
    }
    new_i = i;
    new_j = j+1;
    while (new_j < 8) {
        if (isLegal(i, j, new_i, new_j, board_ptr)) {
            move = addMove(pos, new_i*8+new_j, move);
        }
        if (colorOfSquare(new_i*8+new_j, board_ptr) != 'e') {
            break;
        }
        new_j++;
    }
    new_i = i;
    new_j = j-1;
    while (new_j >= 0) {
        if (isLegal(i, j, new_i, new_j, board_ptr)) {
            move = addMove(pos, new_i*8+new_j, move);
        }
        if (colorOfSquare(new_i*8+new_j, board_ptr) != 'e') {
            break;
        }
        new_j--;
    }

    return move;
}

Moves *genQueenMoves(Moves *move, char *square, char color, Board_ptr *board_ptr) {
    move = genBishopMoves(move, square, color, board_ptr);
    return genRookMoves(move, square, color, board_ptr);
}

Moves *genKingMoves(Moves *move, char *square, char color, Board_ptr *board_ptr) {
    int pos = getPosFromSquare(square, board_ptr);
    int i = pos / 8;
    int j = pos % 8;

    if (isLegal(i, j, i+1, j, board_ptr)) {
        move = addMove(pos, (i+1)*8+j, move);
    }
    if (isLegal(i, j, i+1, j+1, board_ptr)) {
        move = addMove(pos, (i+1)*8+j+1, move);
    }
    if (isLegal(i, j, i+1, j-1, board_ptr)) {
        move = addMove(pos, (i+1)*8+j-1, move);
    }
    if (isLegal(i, j, i-1, j, board_ptr)) {
        move = addMove(pos, (i-1)*8+j, move);
    }
    if (isLegal(i, j, i-1, j+1, board_ptr)) {
        move = addMove(pos, (i-1)*8+j+1, move);
    }
    if (isLegal(i, j, i-1, j-1, board_ptr)) {
        move = addMove(pos, (i-1)*8+j-1, move);
    }
    if (isLegal(i, j, i, j+1, board_ptr)) {
        move = addMove(pos, i*8+j+1, move);
    }
    if (isLegal(i, j, i, j-1, board_ptr)) {
        move = addMove(pos, i*8+j-1, move);
    }
    return move;
}

Moves *generateMovesForSquare(Moves *moves, char *square, char color, Board_ptr *board_ptr) {
    char *board = board_ptr->val;
    int pos = getPosFromSquare(square, board_ptr);
    int i = pos / 8;
    int j = pos % 8;
    char piece = board[i*8+j];

    if (piece == 'P' || piece == 'p') {
        moves = genPawnMoves(moves, square, color, board_ptr);
    } else if (piece == 'N' || piece == 'n') {
        moves = genKnightMoves(moves, square, color, board_ptr);
    } else if (piece == 'B' || piece == 'b') {
        moves = genBishopMoves(moves, square, color, board_ptr);
    } else if (piece == 'R' || piece == 'r') {
        moves = genRookMoves(moves, square, color, board_ptr);
    } else if (piece == 'Q' || piece == 'q') {
        moves = genQueenMoves(moves, square, color, board_ptr);
    } else if (piece == 'K' || piece == 'k') {
        moves = genKingMoves(moves, square, color, board_ptr);
    }
    return moves;
}

void printMoves(Moves *move, Board_ptr *board_ptr) {
    char *frm = (char *) malloc(2);
    char *to = (char *) malloc(2);
    while (move->next != NULL) {
        getSquareFromPos(move->to, to, board_ptr);
        getSquareFromPos(move->frm, frm, board_ptr);
        printf("%s", frm);
        printf("%s\n", to);
        move = move->next;
    }
}

void genPossibleMoves(Moves **moves, char color, Board_ptr *board_ptr) {
    char *curSquare = (char *) malloc(2);
    char *board = board_ptr->val;
    char *sq[2];
    (*moves)->next = NULL;

    Moves *curMove = *moves;

    // iterate through each piece on the board.
    int i = 0;
    while (i < 8) {
        int j = 0;
        while (j < 8) {
            if (pieceColor(board[i*8+j]) == color) {
                getSquareFromPos(i*8+j, curSquare, board_ptr);
                curMove = generateMovesForSquare(curMove, curSquare, color, board_ptr);
            }
            j++;
        }
        i += 1;
    }
}

void makeMove(char *move, Board_ptr **board_ptr) {
    char *board = (*board_ptr)->val;
    char *new_board = (char* ) malloc(strlen(board));
    strcpy(new_board, board);

    (*board_ptr)->next = (Board_ptr *) malloc(sizeof(Board_ptr));
    (*board_ptr)->next->prev = *board_ptr;
    (*board_ptr) = (*board_ptr)->next;

    char old_sq[2];
    char new_sq[2];

    old_sq[0] = move[0];
    old_sq[1] = move[1];
    new_sq[0] = move[2];
    new_sq[1] = move[3];
    int frm = getPosFromSquare(old_sq, *board_ptr);
    int to = getPosFromSquare(new_sq, *board_ptr);

    int i = frm / 8;
    int j = frm % 8;
    char tmp = new_board[i*8+j];
    new_board[i*8+j] = ' ';

    i = to / 8;
    j = to % 8;
    new_board[i*8+j] = tmp;
    (*board_ptr)->val = new_board;
}

/** Undoes the last move. */
void undoMove(Board_ptr **board_ptr) {
    if ((*board_ptr)->prev != NULL) {
        *board_ptr = (*board_ptr)->prev;
    }
}

int scoreBoard(Board_ptr *board_ptr, char color) {
    char *board = board_ptr->val;
    int pts = 0;
    int i = 0;
    while (i < 64) {
        if (pieceColor(board[i]) == color) {
            pts += valOfPiece(board[i]);
        } else if (pieceColor(board[i]) == oppositeColor(color)) {
            pts -= valOfPiece(board[i]);
        }
        i++;
    }
    return pts;
}

void makeMove2(Moves *move, Board_ptr **board_ptr) {
    int frm = move->frm;
    int to = move->to;
    char *curMove = (char *) malloc(4);
    getSquareFromPos(frm, curMove, *board_ptr);
    getSquareFromPos(to, curMove+2, *board_ptr);
    makeMove(curMove, board_ptr);
}

int alphabeta(Board_ptr *board_ptr, int depth, int alpha, int beta, char turn, char color) {
    if (depth == 0) {
        return scoreBoard(board_ptr, turn);
    }

    Moves *moves = (Moves *) malloc(sizeof(Moves));
    genPossibleMoves(&moves, color, board_ptr);
    int cur;
    if (color == turn) {
        while (moves->next != NULL) {
            makeMove2(moves, &board_ptr);
            cur = alphabeta(board_ptr, depth-1, alpha, beta, turn, oppositeColor(color));
            undoMove(&board_ptr);
            if (cur > alpha) {
                alpha = cur;
            }
            if (beta <= alpha) {
                break;
            }
            moves = moves->next;
        }
        return alpha;
    } else {
        while (moves->next != NULL) {
            makeMove2(moves, &board_ptr);
            cur = alphabeta(board_ptr, depth-1, alpha, beta, turn, oppositeColor(color));
            if (cur < beta) {
                beta = cur;
            }
            undoMove(&board_ptr);
            if (beta <= alpha) {
                break;
            }
            moves = moves->next;
        }
        return beta;
    }
}

void getBestMove(char **m, char color, Board_ptr *board_ptr) {
    Serial.println("top of getBestMove");
    Moves *moves = (Moves *) malloc(sizeof(Moves));
    genPossibleMoves(&moves, color, board_ptr);
    Serial.println("top-middle of getBestMove");

    Moves *best = moves;

    int bestScore;
    if (moves->next != NULL) {
        makeMove2(best, &board_ptr);
        bestScore = alphabeta(board_ptr, 0, -99, 99, color, oppositeColor(color));
        undoMove(&board_ptr);
    } else {
        return;
    }
    
    Serial.println("middle of getBestMove");

    int curScore;
    //srand(Time(NULL));
    while (moves->next != NULL) {
        makeMove2(moves, &board_ptr);
        curScore = alphabeta(board_ptr, 0, -99, 99, color, oppositeColor(color));
        undoMove(&board_ptr);
        if (curScore > bestScore || (curScore == bestScore && (rand() % 2) == 0)) {
            bestScore = curScore;
            best = moves;
        }
        moves = moves->next;
    }

    int frm = best->frm;
    int to = best->to;
    getSquareFromPos(frm, *m, board_ptr);
    getSquareFromPos(to, *m+2, board_ptr);
}

Board_ptr *board_ptr;
char *move;
int rc;
size_t *len;
                    
void setup() {
  Serial.begin(9600);
  Serial.println("Deep Blue and Gold");

    board_ptr = (Board_ptr *) malloc(sizeof(Board_ptr));
    board_ptr->val = (char *) malloc(64);
    resetBoard(board_ptr);
    printBoard(board_ptr);
    
    move = (char *) malloc(4);
    Serial.println("Enter move> ");
    Serial.flush();
}

  char mov[5]; // Allocate some space for the string
  char inChar=-1; // Where to store the character read
  int index = 0; // Index into array; where to store the character
  
  
void loop() {

  // send data only when you receive data:
  if (Serial.available() > 0) {
      // read the incoming byte:
      inChar = Serial.read();
  
      // store what you got:
      mov[index] = inChar;
      index += 1;
      
      if (index >= 4) {
        mov[index] = '\0';
        index = 0;

        makeMove(mov, &board_ptr);
        printBoard(board_ptr);
        Serial.println(mov);


        getBestMove(&move, 'b', board_ptr);
        move[4] = '\0';
        Serial.println(move);
        makeMove(move, &board_ptr);
        //printf("black moves %s\n", move);
        printBoard(board_ptr);
        
        free(move);
        //Serial.flush();
        
        Serial.print("Enter move> ");
        
      }
      
  }

}
