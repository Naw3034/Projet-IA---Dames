#ifndef LIST_H
#define LIST_H

#include "item.h"

typedef struct {
    int   numElements;
    char *name;
    Item *first;
    Item *last;
} list_t;


/* --- Gestion des nœuds --- */
Item *nodeAlloc();
void  freeItem(Item *node);

/* --- Gestion de la liste --- */
void  initList(list_t *list);
int   listCount(list_t *list);

/* Recherche un item dont le board correspond ; retourne NULL si absent.
   'size' est la dimension du plateau (nb de cases = size*size). */
Item *onList(list_t *list, char *board, int size);

Item *popFirst(list_t *list);   // retire et retourne le premier élément
Item *popLast(list_t *list);    // retire et retourne le dernier élément
Item *popBest(list_t *list);    // retire et retourne l'élément au score minimal

void  addFirst(list_t *list, Item *node);   // insère en tête
void  addLast(list_t *list, Item *node);    // insère en queue
void  delList(list_t *list, Item *node);    // supprime un nœud précis
void  cleanupList(list_t *list);            // vide la liste et libère la mémoire

void  printList(list_t list);              // affichage debug

#endif