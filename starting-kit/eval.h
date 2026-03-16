#ifndef EVAL_H
#define EVAL_H

#include "plateau.h"
#include "jeu.h"

/* =========================================================
   Valeurs des pièces
   ========================================================= */

#define VAL_PION    100   /* valeur d'un pion                  */
#define VAL_DAME    350   /* valeur d'une dame (3.5x un pion)  */

/* Score de victoire / défaite (bornes du Minimax) */
#define SCORE_VICTOIRE  100000
#define SCORE_DEFAITE  -100000

/* =========================================================
   Bonus positionnels
   ========================================================= */

/* Bonus pour contrôler le centre du plateau */
#define BONUS_CENTRE        15

/* Bonus pour les pions en position avancée (vers la promotion) */
#define BONUS_AVANCE         5

/* Bonus pour une pièce protégée par une autre pièce amie */
#define BONUS_PROTECTION    10

/* Bonus pour un pion sur la rangée arrière (empêche l'adversaire
   d'avoir une dame facilement, sécurité défensive)            */
#define BONUS_ARRIERE        8

/* =========================================================
   Prototype principal
   ========================================================= */

/*
 * Évalue statiquement le plateau du point de vue de 'joueur'.
 * Retourne un score positif si la position est favorable,
 * négatif si elle est défavorable.
 *
 * Critères pris en compte :
 *   1. Matériel      : différence de pions et de dames
 *   2. Centre        : bonus pour les pièces au centre
 *   3. Avancement    : les pions avancés valent plus
 *   4. Protection    : bonus si une pièce est couverte par une amie
 *   5. Rangée arrière: pions sur la dernière rangée = sécurité
 *   6. Mobilité      : bonus selon le nombre de coups disponibles
 */
int evaluerPlateau(Plateau *p, int joueur);

/* Fonctions de détail (exposées pour les tests) */
int evalMateriel(Plateau *p, int joueur);
int evalPositionnel(Plateau *p, int joueur);
int evalMobilite(Plateau *p, int joueur);

#endif