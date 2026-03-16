#include <stdlib.h>
#include "eval.h"

/* =========================================================
   Tables positionnelles
   =========================================================
   Bonus accordé à une pièce selon sa position sur le plateau.
   Le tableau est défini du point de vue des BLANCS (qui montent
   vers les lignes 0). Il est retourné pour les Noirs.

   Lecture : plus la valeur est élevée, meilleure est la case.
   ========================================================= */

static const int TABLE_PION[TAILLE][TAILLE] = {
/*        A    B    C    D    E    F    G    H  */
/* 1 */ {  0,  0,  0,  0,  0,  0,  0,  0 },
/* 2 */ {  4,  4,  4,  4,  4,  4,  4,  4 },
/* 3 */ {  8,  8, 10, 10, 10, 10,  8,  8 },
/* 4 */ {  4,  8, 12, 12, 12, 12,  8,  4 },
/* 5 */ {  2,  4,  8,  8,  8,  8,  4,  2 },
/* 6 */ {  2,  2,  4,  4,  4,  4,  2,  2 },
/* 7 */ {  2,  2,  2,  2,  2,  2,  2,  2 },
/* 8 */ { 10, 10, 10, 10, 10, 10, 10, 10 }, /* rangée arrière (défense) */
};

static const int TABLE_DAME[TAILLE][TAILLE] = {
/*        A    B    C    D    E    F    G    H  */
/* 1 */ {  0,  5,  0,  5,  0,  5,  0,  5 },
/* 2 */ {  5,  8,  8,  8,  8,  8,  8,  5 },
/* 3 */ {  0,  8, 12, 12, 12, 12,  8,  0 },
/* 4 */ {  5,  8, 12, 15, 15, 12,  8,  5 },
/* 5 */ {  5,  8, 12, 15, 15, 12,  8,  5 },
/* 6 */ {  0,  8, 12, 12, 12, 12,  8,  0 },
/* 7 */ {  5,  8,  8,  8,  8,  8,  8,  5 },
/* 8 */ {  0,  5,  0,  5,  0,  5,  0,  5 },
};

/* =========================================================
   Utilitaires internes
   ========================================================= */

/*
 * Retourne la valeur positionnelle d'une case (r,c)
 * pour le joueur donné, en tenant compte de son sens de jeu.
 * Les Blancs lisent la table normalement (de bas en haut).
 * Les Noirs lisent la table en miroir vertical.
 */
static int bonusPosition(int r, int c, int piece, int joueur)
{
    int row = (joueur == JOUEUR_BLANC) ? r : (TAILLE - 1 - r);

    if (estDame(piece))
        return TABLE_DAME[row][c];
    return TABLE_PION[row][c];
}

/*
 * Retourne 1 si la pièce en (r,c) est protégée par une pièce
 * amie derrière elle (diagonale arrière occupée par une pièce
 * du même joueur).
 */
static int estProtegee(Plateau *p, int r, int c, int joueur)
{
    /* Direction "arrière" selon le joueur */
    int dr = (joueur == JOUEUR_BLANC) ? 1 : -1;
    int dcols[2] = {-1, 1};

    for (int d = 0; d < 2; d++) {
        int pr = r + dr;
        int pc = c + dcols[d];
        if (DANS_PLATEAU(pr, pc) && estAuJoueur(p->grille[pr][pc], joueur))
            return 1;
    }
    return 0;
}

/* =========================================================
   Évaluation du matériel
   =========================================================
   Score = (pions_joueur × VAL_PION + dames_joueur × VAL_DAME)
         - (pions_adv   × VAL_PION + dames_adv   × VAL_DAME)
   ========================================================= */

int evalMateriel(Plateau *p, int joueur)
{
    int adversaire = (joueur == JOUEUR_BLANC) ? JOUEUR_NOIR : JOUEUR_BLANC;
    int score = 0;

    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            int piece = p->grille[i][j];
            if (piece == VIDE) continue;

            int valeur = estDame(piece) ? VAL_DAME : VAL_PION;

            if (estAuJoueur(piece, joueur))
                score += valeur;
            else if (estAuJoueur(piece, adversaire))
                score -= valeur;
        }
    }
    return score;
}

/* =========================================================
   Évaluation positionnelle
   =========================================================
   Parcourt chaque pièce et ajoute :
     - le bonus de la table positionnelle
     - un bonus si la pièce est protégée
   ========================================================= */

int evalPositionnel(Plateau *p, int joueur)
{
    int adversaire = (joueur == JOUEUR_BLANC) ? JOUEUR_NOIR : JOUEUR_BLANC;
    int score = 0;

    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            int piece = p->grille[i][j];
            if (piece == VIDE) continue;

            int bonus = bonusPosition(i, j, piece, joueur);
            int prot  = estProtegee(p, i, j, joueur) ? BONUS_PROTECTION : 0;

            if (estAuJoueur(piece, joueur))
                score += bonus + prot;
            else if (estAuJoueur(piece, adversaire))
                score -= bonusPosition(i, j, piece, adversaire)
                       + (estProtegee(p, i, j, adversaire) ? BONUS_PROTECTION : 0);
        }
    }
    return score;
}

/* =========================================================
   Évaluation de la mobilité
   =========================================================
   Un joueur avec plus de coups disponibles a plus d'options
   tactiques. On compare le nombre de coups légaux de chaque
   camp et on récompense l'avantage de mobilité.
   ========================================================= */

int evalMobilite(Plateau *p, int joueur)
{
    int adversaire = (joueur == JOUEUR_BLANC) ? JOUEUR_NOIR : JOUEUR_BLANC;

    MoveList coups_j, coups_a;
    initMoveList(&coups_j);
    initMoveList(&coups_a);

    genererCoups(p, joueur,     &coups_j);
    genererCoups(p, adversaire, &coups_a);

    int nb_j = moveListCount(&coups_j);
    int nb_a = moveListCount(&coups_a);

    cleanupMoveList(&coups_j);
    cleanupMoveList(&coups_a);

    /* Chaque coup supplémentaire vaut 3 points */
    return (nb_j - nb_a) * 3;
}

/* =========================================================
   Fonction d'évaluation principale
   ========================================================= */

int evaluerPlateau(Plateau *p, int joueur)
{
    int adversaire = (joueur == JOUEUR_BLANC) ? JOUEUR_NOIR : JOUEUR_BLANC;

    /* --- Cas terminaux : victoire / défaite --- */
    if (partieTerminee(p, adversaire)) return  SCORE_VICTOIRE;
    if (partieTerminee(p, joueur))     return  SCORE_DEFAITE;

    /* --- Combinaison des critères --- */
    int score = 0;

    score += evalMateriel(p, joueur);    /* critère principal     */
    score += evalPositionnel(p, joueur); /* bonus de position     */
    score += evalMobilite(p, joueur);    /* bonus de mobilité     */

    return score;
}