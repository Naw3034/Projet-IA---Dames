#include <stdio.h>
#include "plateau.h"

/*
 * Dispose les pions en début de partie sur les cases sombres.
 * Aux dames on ne joue que sur les cases sombres, c'est-à-dire
 * celles dont la somme (ligne + colonne) est impaire.
 *
 * Disposition sur un plateau 8×8 :
 *   lignes 0-2  → Noirs  (3 rangées)
 *   lignes 3-4  → Vides  (2 rangées centrales)
 *   lignes 5-7  → Blancs (3 rangées)
 */
void initialiserPlateau(Plateau *p)
{
    int i, j;

    for (i = 0; i < TAILLE; i++) {
        for (j = 0; j < TAILLE; j++) {

            if ((i + j) % 2 != 0) {
                /* Case sombre (jouable) */
                if (i < 3) {
                    p->grille[i][j] = PION_NOIR;      /* 3 premières rangées : Noirs  */
                } else if (i > 4) {
                    p->grille[i][j] = PION_BLANC;     /* 3 dernières rangées : Blancs */
                } else {
                    p->grille[i][j] = VIDE;           /* 2 rangées centrales : vides  */
                }
            } else {
                /* Case claire (injouable) */
                p->grille[i][j] = VIDE;
            }
        }
    }
}

/*
 * Affiche le plateau en console avec :
 *   - les lettres A-H pour les colonnes
 *   - les chiffres 0-7 pour les lignes
 *   - [ ] pour les cases claires (injouables)
 *   - symboles b/n/B/N/. pour les cases sombres
 */
void afficherPlateau(Plateau *p)
{
    int i, j;

    /* En-tête colonnes */
    printf("\n   ");
    for (j = 0; j < TAILLE; j++)
        printf(" %c ", 'A' + j);
    printf("\n");

    for (i = 0; i < TAILLE; i++) {
        printf(" %d ", i + 1);   /* numéro de ligne (1 à 8) */

        for (j = 0; j < TAILLE; j++) {
            if ((i + j) % 2 == 0) {
                /* Case claire : injouable */
                printf("[ ]");
            } else {
                /* Case sombre : afficher la pièce */
                switch (p->grille[i][j]) {
                    case VIDE:          printf(" . "); break;
                    case PION_BLANC:    printf(" b "); break;
                    case PION_NOIR:     printf(" n "); break;
                    case DAME_BLANCHE:  printf(" B "); break;
                    case DAME_NOIRE:    printf(" N "); break;
                    default:            printf(" ? "); break;
                }
            }
        }
        printf("\n");
    }
    printf("\n");
}