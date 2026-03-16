#ifndef MOVE_LIST_H
#define MOVE_LIST_H

#include "move.h"

/*
 * Liste doublement chaînée de coups.
 * Utilisée pour stocker les coups légaux générés à chaque
 * nœud de l'arbre Minimax.
 */
typedef struct {
    int   numElements;
    Move *first;
    Move *last;
} MoveList;

/* --- Gestion des nœuds --- */
Move *moveAlloc();
void  freeMove(Move *move);

/* --- Gestion de la liste --- */
void  initMoveList(MoveList *list);
int   moveListCount(MoveList *list);
int   moveListIsEmpty(MoveList *list);

void  addMoveFirst(MoveList *list, Move *move);   /* insère en tête  */
void  addMoveLast(MoveList *list, Move *move);    /* insère en queue */

Move *popMoveFirst(MoveList *list);   /* retire et retourne le premier */
void  delMove(MoveList *list, Move *move);        /* supprime un coup précis */
void  cleanupMoveList(MoveList *list);            /* vide et libère tout     */

void  printMoveList(MoveList *list);              /* affichage debug         */

#endif