#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "movelist.h"

/* =========================================================
   Gestion des nœuds
   ========================================================= */

/* Alloue et initialise un coup vide. */
Move *moveAlloc()
{
    Move *move = (Move *) malloc(sizeof(Move));
    assert(move);

    move->from_row    = -1;
    move->from_col    = -1;
    move->to_row      = -1;
    move->to_col      = -1;
    move->num_captures = 0;
    memset(move->captures, -1, sizeof(move->captures));

    move->prev = NULL;
    move->next = NULL;

    return move;
}

/* Libère un coup. */
void freeMove(Move *move)
{
    if (move) free(move);
}

/* =========================================================
   Initialisation
   ========================================================= */

void initMoveList(MoveList *list)
{
    assert(list);
    list->numElements = 0;
    list->first       = NULL;
    list->last        = NULL;
}

int moveListCount(MoveList *list)
{
    return list->numElements;
}

int moveListIsEmpty(MoveList *list)
{
    return list->numElements == 0;
}

/* =========================================================
   Insertions
   ========================================================= */

void addMoveFirst(MoveList *list, Move *move)
{
    if (list->first == NULL) {
        list->first = move;
        list->last  = move;
        move->prev  = NULL;
        move->next  = NULL;
    } else {
        move->next        = list->first;
        move->prev        = NULL;
        list->first->prev = move;
        list->first       = move;
    }
    list->numElements++;
}

void addMoveLast(MoveList *list, Move *move)
{
    if (list->last == NULL) {
        list->first = move;
        list->last  = move;
        move->prev  = NULL;
        move->next  = NULL;
    } else {
        move->prev       = list->last;
        move->next       = NULL;
        list->last->next = move;
        list->last       = move;
    }
    list->numElements++;
}

/* =========================================================
   Suppressions
   ========================================================= */

/* Retire et retourne le premier coup ; retourne NULL si liste vide. */
Move *popMoveFirst(MoveList *list)
{
    if (list->first == NULL) return NULL;

    Move *move  = list->first;
    list->first = move->next;

    if (list->first != NULL)
        list->first->prev = NULL;
    else
        list->last = NULL;

    move->prev = NULL;
    move->next = NULL;
    list->numElements--;
    return move;
}

/* Supprime un coup précis de la liste (sans le libérer). */
void delMove(MoveList *list, Move *move)
{
    if (move->prev != NULL)
        move->prev->next = move->next;
    else
        list->first = move->next;

    if (move->next != NULL)
        move->next->prev = move->prev;
    else
        list->last = move->prev;

    move->prev = NULL;
    move->next = NULL;
    list->numElements--;
}

/* Vide complètement la liste et libère chaque coup. */
void cleanupMoveList(MoveList *list)
{
    Move *move;
    while (list->first != NULL) {
        move = popMoveFirst(list);
        freeMove(move);
    }
}

/* =========================================================
   Affichage debug
   ========================================================= */

void printMoveList(MoveList *list)
{
    Move *move = list->first;
    int   i    = 1;

    if (moveListIsEmpty(list)) {
        printf("(liste vide)\n");
        return;
    }

    while (move != NULL) {
        printf("  coup %d : (%d,%d) -> (%d,%d)",
               i++,
               move->from_row, move->from_col,
               move->to_row,   move->to_col);

        if (move->num_captures > 0) {
            printf("  [captures :");
            for (int c = 0; c < move->num_captures; c++)
                printf(" (%d,%d)", move->captures[c][0], move->captures[c][1]);
            printf("]");
        }
        printf("\n");
        move = move->next;
    }
    printf("  total : %d coup(s)\n", list->numElements);
}