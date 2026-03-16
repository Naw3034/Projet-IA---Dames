#ifndef JEU_H
#define JEU_H

#include "plateau.h"
#include "movelist.h"

/* =========================================================
   Macros utilitaires
   ========================================================= */

/* Vérifie qu'une case (r,c) est dans les bornes du plateau */
#define DANS_PLATEAU(r, c) \
    ((r) >= 0 && (r) < TAILLE && (c) >= 0 && (c) < TAILLE)

/* Ligne de promotion selon le joueur */
#define LIGNE_PROMO(joueur) \
    ((joueur) == JOUEUR_BLANC ? 0 : TAILLE - 1)

/* =========================================================
   Fonctions utilitaires
   ========================================================= */

/* Retourne 1 si la pièce appartient au joueur donné */
int estAuJoueur(int piece, int joueur);

/* Retourne 1 si la pièce est une pièce adverse (et non vide) */
int estAdversaire(int piece, int joueur);

/* Retourne 1 si la pièce est une dame */
int estDame(int piece);

/* Compte le nombre de pièces restantes pour un joueur */
int compterPieces(Plateau *p, int joueur);

/* =========================================================
   Génération des coups
   ========================================================= */

/*
 * Génère tous les coups légaux pour le joueur donné.
 * Règle : si des captures sont possibles, seules les captures
 * sont retournées (prise obligatoire).
 * Les coups sont ajoutés dans 'list' (qui doit être initialisée).
 */
void genererCoups(Plateau *p, int joueur, MoveList *list);

/* =========================================================
   Application d'un coup
   ========================================================= */

/*
 * Applique le coup sur une COPIE du plateau et retourne
 * le nouveau plateau (le plateau original n'est pas modifié).
 * Gère automatiquement : déplacement, captures et promotion.
 */
Plateau appliquerCoup(Plateau *p, Move *m);

/* =========================================================
   Promotion
   ========================================================= */

/*
 * Parcourt le plateau et promeut en dame tout pion ayant
 * atteint la dernière rangée adverse.
 */
void verifierPromotion(Plateau *p);

/* =========================================================
   Fin de partie
   ========================================================= */

/*
 * Retourne 1 si le joueur donné a perdu :
 *   - il n'a plus de pièces, OU
 *   - il n'a plus de coup légal disponible.
 */
int partieTerminee(Plateau *p, int joueur);

#endif