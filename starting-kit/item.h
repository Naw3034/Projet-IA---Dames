#ifndef ITEM_H
#define ITEM_H

struct Item_s;

typedef struct Item_s {
    char  size;               // taille du plateau (ex: 8 pour un 8x8)
    char *board;              // état du plateau aplati en tableau 1D
    int   score;              // valeur d'évaluation statique (pour Minimax)
    int   depth;              // profondeur dans l'arbre de recherche
    struct Item_s *parent;    // nœud parent (pour reconstruire le meilleur coup)
    struct Item_s *prev, *next; // chaînage doublement lié
} Item;

#endif