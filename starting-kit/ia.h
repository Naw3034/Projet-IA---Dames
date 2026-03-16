#ifndef IA_H
#define IA_H

#include "plateau.h"
#include "movelist.h"
#include "eval.h"

/* =========================================================
   Profondeur de recherche par défaut
   =========================================================
   3 = rapide (moins d'une seconde)
   5 = bon niveau (quelques secondes)
   7 = fort     (peut être lent en milieu de partie)
   ========================================================= */
#define PROFONDEUR_IA  5

/* =========================================================
   Structure de résultat du Minimax
   ========================================================= */
typedef struct {
    Move meilleurCoup;   /* coup à jouer                    */
    int  score;          /* score associé à ce coup         */
    int  noeudsExplores; /* statistique : nœuds parcourus   */
} ResultatIA;

/* =========================================================
   Prototypes
   ========================================================= */

/*
 * Point d'entrée principal : calcule le meilleur coup pour
 * 'joueur' à partir du plateau 'p', avec une recherche à
 * 'profondeur' niveaux et élagage alpha-bêta.
 *
 * Retourne un ResultatIA dont le champ 'meilleurCoup' contient
 * le coup à jouer. Si aucun coup n'est possible, meilleurCoup.from_row == -1.
 */
ResultatIA calculerMeilleurCoup(Plateau *p, int joueur, int profondeur);

/*
 * Algorithme Minimax avec élagage Alpha-Bêta (interne).
 * - maximisant == 1 : on cherche le score maximal (tour de l'IA)
 * - maximisant == 0 : on cherche le score minimal (tour de l'adversaire)
 * - alpha : meilleur score garanti pour le maximisant
 * - beta  : meilleur score garanti pour le minimisant
 */
int minimax(Plateau *p, int profondeur, int alpha, int beta,
            int maximisant, int joueurIA, int *noeuds);

#endif