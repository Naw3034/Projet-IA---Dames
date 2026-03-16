#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ia.h"
#include "jeu.h"

/* =========================================================
   Utilitaires internes
   ========================================================= */

static int max(int a, int b) { return a > b ? a : b; }
static int min(int a, int b) { return a < b ? a : b; }

/* Retourne le joueur adverse */
static int adversaire(int joueur)
{
    return (joueur == JOUEUR_BLANC) ? JOUEUR_NOIR : JOUEUR_BLANC;
}

/* =========================================================
   Minimax avec élagage Alpha-Bêta
   =========================================================
   Principe :
     - À chaque nœud MAX (tour de l'IA), on cherche le score
       le plus élevé parmi tous les coups possibles.
     - À chaque nœud MIN (tour de l'adversaire), on cherche
       le score le plus bas.
     - Alpha-Bêta : on coupe les branches qui ne peuvent pas
       améliorer le résultat déjà trouvé, ce qui réduit
       drastiquement l'arbre exploré.

   Paramètres :
     p          : état courant du plateau
     profondeur : niveaux restants à explorer
     alpha      : meilleur score garanti pour MAX (commence à -∞)
     beta       : meilleur score garanti pour MIN (commence à +∞)
     maximisant : 1 si c'est le tour de l'IA, 0 sinon
     joueurIA   : identifiant du joueur IA (constant pendant toute la recherche)
     noeuds     : compteur de nœuds explorés (statistique)
   ========================================================= */

int minimax(Plateau *p, int profondeur, int alpha, int beta,
            int maximisant, int joueurIA, int *noeuds)
{
    (*noeuds)++;

    int joueurCourant = maximisant ? joueurIA : adversaire(joueurIA);

    /* --- Cas terminal : profondeur 0 ou partie terminée --- */
    if (profondeur == 0 || partieTerminee(p, joueurCourant)) {
        return evaluerPlateau(p, joueurIA);
    }

    /* --- Générer tous les coups légaux --- */
    MoveList coups;
    initMoveList(&coups);
    genererCoups(p, joueurCourant, &coups);

    /* Aucun coup disponible = position perdue pour le joueur courant */
    if (moveListIsEmpty(&coups)) {
        cleanupMoveList(&coups);
        return evaluerPlateau(p, joueurIA);
    }

    int scoreRetour;

    if (maximisant) {
        /* ---- Nœud MAX : on veut le score le plus élevé ---- */
        scoreRetour = SCORE_DEFAITE - 1;

        Move *coup = coups.first;
        while (coup != NULL) {
            Plateau nouveau = appliquerCoup(p, coup);

            int score = minimax(&nouveau, profondeur - 1,
                                alpha, beta, 0, joueurIA, noeuds);

            scoreRetour = max(scoreRetour, score);
            alpha       = max(alpha, scoreRetour);

            /* Coupure bêta : l'adversaire n'aurait pas permis cette branche */
            if (beta <= alpha) break;

            coup = coup->next;
        }
    } else {
        /* ---- Nœud MIN : on veut le score le plus bas ---- */
        scoreRetour = SCORE_VICTOIRE + 1;

        Move *coup = coups.first;
        while (coup != NULL) {
            Plateau nouveau = appliquerCoup(p, coup);

            int score = minimax(&nouveau, profondeur - 1,
                                alpha, beta, 1, joueurIA, noeuds);

            scoreRetour = min(scoreRetour, score);
            beta        = min(beta, scoreRetour);

            /* Coupure alpha : l'IA n'aurait pas permis cette branche */
            if (beta <= alpha) break;

            coup = coup->next;
        }
    }

    cleanupMoveList(&coups);
    return scoreRetour;
}

/* =========================================================
   Point d'entrée : calculerMeilleurCoup
   =========================================================
   Explore tous les coups de premier niveau, appelle minimax
   pour chacun, et retourne celui qui obtient le meilleur score.
   ========================================================= */

ResultatIA calculerMeilleurCoup(Plateau *p, int joueur, int profondeur)
{
    ResultatIA resultat;
    resultat.score          = SCORE_DEFAITE - 1;
    resultat.noeudsExplores = 0;
    resultat.meilleurCoup.from_row = -1; /* sentinelle : pas de coup */
    resultat.meilleurCoup.from_col = -1;
    resultat.meilleurCoup.to_row   = -1;
    resultat.meilleurCoup.to_col   = -1;
    resultat.meilleurCoup.num_captures = 0;

    /* Générer les coups légaux du premier niveau */
    MoveList coups;
    initMoveList(&coups);
    genererCoups(p, joueur, &coups);

    if (moveListIsEmpty(&coups)) {
        cleanupMoveList(&coups);
        return resultat; /* aucun coup possible */
    }

    int alpha = SCORE_DEFAITE - 1;
    int beta  = SCORE_VICTOIRE + 1;

    Move *coup = coups.first;
    while (coup != NULL) {
        Plateau nouveau = appliquerCoup(p, coup);

        /* L'adversaire joue ensuite (minimisant) */
        int score = minimax(&nouveau, profondeur - 1,
                            alpha, beta, 0, joueur,
                            &resultat.noeudsExplores);

        if (score > resultat.score) {
            resultat.score      = score;
            resultat.meilleurCoup = *coup; /* copie du coup */
            alpha               = score;
        }

        coup = coup->next;
    }

    cleanupMoveList(&coups);
    return resultat;
}