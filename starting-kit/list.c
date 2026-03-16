#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "list.h"

/* =========================================================
   Gestion des nœuds
   ========================================================= */

Item *nodeAlloc()
{
    Item *node = (Item *) malloc(sizeof(Item));
    assert(node);

    node->board  = NULL;
    node->parent = NULL;
    node->prev   = NULL;
    node->next   = NULL;
    node->score  = 0;
    node->depth  = 0;
    node->size   = 0;

    return node;
}

void freeItem(Item *node)
{
    if (node == NULL) return;
    if (node->board) free(node->board);
    free(node);
}

/* =========================================================
   Initialisation et comptage
   ========================================================= */

void initList(list_t *list)
{
    assert(list);
    list->numElements = 0;
    list->first       = NULL;
    list->last        = NULL;
}

int listCount(list_t *list)
{
    return list->numElements;
}

/* =========================================================
   Recherche
   ========================================================= */

/*
 * Retourne le premier item dont le board correspond à 'board',
 * ou NULL si absent.
 * Utilise memcmp (et non strcmp) car le board contient des entiers
 * compactés qui peuvent valoir 0 (VIDE), ce qui casserait strcmp.
 */
Item *onList(list_t *list, char *board, int size)
{
    Item *current = list->first;

    while (current != NULL) {
        if (memcmp(current->board, board, (size_t)(size * size)) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

/* =========================================================
   Suppressions
   ========================================================= */

/* Retire et retourne le premier élément ; retourne NULL si liste vide. */
Item *popFirst(list_t *list)
{
    if (list->first == NULL) return NULL;

    Item *item    = list->first;
    list->first   = item->next;

    if (list->first != NULL)
        list->first->prev = NULL;
    else
        list->last = NULL;

    item->next = NULL;
    item->prev = NULL;
    list->numElements--;
    return item;
}

/* Retire et retourne le dernier élément ; retourne NULL si liste vide. */
Item *popLast(list_t *list)
{
    if (list->last == NULL) return NULL;

    Item *item  = list->last;
    list->last  = item->prev;

    if (list->last != NULL)
        list->last->next = NULL;
    else
        list->first = NULL;

    item->next = NULL;
    item->prev = NULL;
    list->numElements--;
    return item;
}

/* Supprime un nœud précis de la liste (sans le libérer). */
void delList(list_t *list, Item *node)
{
    if (node->prev != NULL)
        node->prev->next = node->next;
    else
        list->first = node->next;

    if (node->next != NULL)
        node->next->prev = node->prev;
    else
        list->last = node->prev;

    node->prev = NULL;
    node->next = NULL;
    list->numElements--;
}

/*
 * Retire et retourne l'élément dont le score est minimal.
 * Utile pour trier les coups candidats dans le Minimax.
 */
Item *popBest(list_t *list)
{
    if (list->first == NULL) return NULL;

    Item *best    = list->first;
    Item *current = list->first->next;

    while (current != NULL) {
        if (current->score < best->score)
            best = current;
        current = current->next;
    }

    delList(list, best);
    return best;
}

/* =========================================================
   Insertions
   ========================================================= */

/* Insère un nœud en tête de liste. */
void addFirst(list_t *list, Item *node)
{
    if (list->first == NULL) {
        list->first = node;
        list->last  = node;
        node->prev  = NULL;
        node->next  = NULL;
    } else {
        node->next        = list->first;
        node->prev        = NULL;
        list->first->prev = node;
        list->first       = node;
    }
    list->numElements++;
}

/* Insère un nœud en queue de liste. */
void addLast(list_t *list, Item *node)
{
    if (list->last == NULL) {
        list->first = node;
        list->last  = node;
        node->prev  = NULL;
        node->next  = NULL;
    } else {
        node->prev       = list->last;
        node->next       = NULL;
        list->last->next = node;
        list->last       = node;
    }
    list->numElements++;
}

/* =========================================================
   Nettoyage
   ========================================================= */

/* Vide complètement la liste et libère chaque nœud. */
void cleanupList(list_t *list)
{
    Item *item;
    while (list->first != NULL) {
        item = popFirst(list);
        freeItem(item);
    }
}

/* =========================================================
   Affichage debug
   ========================================================= */

void printList(list_t list)
{
    Item *item = list.first;
    while (item) {
        printf("score=%d [size=%d] -> ", item->score, item->size);
        item = item->next;
    }
    printf("NULL  (nb_elements: %d)\n", list.numElements);
}