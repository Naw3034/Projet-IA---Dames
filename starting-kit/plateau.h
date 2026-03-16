#ifndef PLATEAU_H
#define PLATEAU_H

/* Taille du damier (8x8 pour les règles anglaises/françaises standard) */
#define TAILLE 8

/* États possibles d'une case */
#define VIDE          0
#define PION_BLANC    1
#define PION_NOIR     2
#define DAME_BLANCHE  3
#define DAME_NOIRE    4

/* Joueurs */
#define JOUEUR_BLANC  PION_BLANC
#define JOUEUR_NOIR   PION_NOIR

/* Structure représentant le plateau de jeu */
typedef struct {
    int grille[TAILLE][TAILLE];
} Plateau;

/* Prototypes */
void initialiserPlateau(Plateau *p);
void afficherPlateau(Plateau *p);

#endif