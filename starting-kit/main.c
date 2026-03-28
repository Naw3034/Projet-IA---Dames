#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "plateau.h"
#include "jeu.h"
#include "eval.h"
#include "ia.h"

/* =========================================================
   Constantes d'affichage
   ========================================================= */
#define LIGNE_SEP "  +---+---+---+---+---+---+---+---+"

/* =========================================================
   Affichage amélioré du plateau
   =========================================================
   Cases claires  : fond hachuré ":::"
   Cases sombres  : pièce ou " . "
   Numérotation   : 1-8 pour les lignes, A-H pour les colonnes
   ========================================================= */
static void afficherPlateauJeu(Plateau *p)
{
    printf("\n");
    printf("      A   B   C   D   E   F   G   H\n");
    printf("    %s\n", LIGNE_SEP);

    for (int i = 0; i < TAILLE; i++) {
        printf("  %d |", i + 1);
        for (int j = 0; j < TAILLE; j++) {
            if ((i + j) % 2 == 0) {
                printf(":::|");
            } else {
                switch (p->grille[i][j]) {
                    case VIDE:          printf(" . |"); break;
                    case PION_BLANC:    printf(" b |"); break;
                    case PION_NOIR:     printf(" n |"); break;
                    case DAME_BLANCHE:  printf(" B |"); break;
                    case DAME_NOIRE:    printf(" N |"); break;
                    default:            printf(" ? |"); break;
                }
            }
        }
        printf(" %d\n", i + 1);
        printf("    %s\n", LIGNE_SEP);
    }
    printf("      A   B   C   D   E   F   G   H\n\n");
}

/* =========================================================
   Affichage de la légende
   ========================================================= */
static void afficherLegende(void)
{
    printf("  Legende : b = pion blanc   |  n = pion noir\n");
    printf("            B = dame blanche  |  N = dame noire\n");
    printf("           ::: = case injouable\n\n");
}

/* =========================================================
   Affichage du score matériel
   ========================================================= */
static void afficherScore(Plateau *p)
{
    int nb_pb = 0, nb_db = 0, nb_pn = 0, nb_dn = 0;
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            switch (p->grille[i][j]) {
                case PION_BLANC:   nb_pb++; break;
                case DAME_BLANCHE: nb_db++; break;
                case PION_NOIR:    nb_pn++; break;
                case DAME_NOIRE:   nb_dn++; break;
            }
        }
    }
    printf("  Blancs : %d pion(s)  %d dame(s)   |   "
           "Noirs : %d pion(s)  %d dame(s)\n\n",
           nb_pb, nb_db, nb_pn, nb_dn);
}

/* =========================================================
   Affichage des coups légaux disponibles
   ========================================================= */
static void afficherCoupsDisponibles(MoveList *coups)
{
    printf("  Coups disponibles (%d) :\n", moveListCount(coups));
    Move *m = coups->first;
    int   i = 1;
    while (m != NULL) {
        printf("    %2d.  %c%d -> %c%d",
               i++,
               'A' + m->from_col, m->from_row + 1,
               'A' + m->to_col,   m->to_row   + 1);
        if (m->num_captures > 0) {
            printf("  [capture");
            for (int c = 0; c < m->num_captures; c++)
                printf(" %c%d",
                       'A' + m->captures[c][1],
                       m->captures[c][0] + 1);
            printf("]");
        }
        printf("\n");
        m = m->next;
    }
    printf("\n");
}

/* =========================================================
   Parsing de la saisie humaine
   =========================================================
   Format attendu : "B6 C5"  ou  "b6c5"  (insensible à la casse,
   l'espace ou le tiret entre les deux cases est optionnel).
   Retourne 1 si la saisie est valide et remplit from_x/to_x,
   0 sinon.
   ========================================================= */
static int parserSaisie(const char *saisie,
                         int *from_row, int *from_col,
                         int *to_row,   int *to_col)
{
    char buf[32];
    strncpy(buf, saisie, sizeof(buf) - 1);
    buf[sizeof(buf)-1] = '\0';

    for (int i = 0; buf[i]; i++)
        buf[i] = (char)toupper((unsigned char)buf[i]);

    char *p = buf;
    while (*p == ' ') p++;

    if (*p < 'A' || *p > 'H') return 0;
    *from_col = *p - 'A'; p++;
    if (*p < '1' || *p > '8') return 0;
    *from_row = *p - '1'; p++;

    while (*p == ' ' || *p == '-') p++;

    if (*p < 'A' || *p > 'H') return 0;
    *to_col = *p - 'A'; p++;
    if (*p < '1' || *p > '8') return 0;
    *to_row = *p - '1';

    return 1;
}

/* =========================================================
   Valider que le coup saisi est dans la liste légale
   ========================================================= */
static Move *validerCoup(MoveList *coups,
                          int from_row, int from_col,
                          int to_row,   int to_col)
{
    Move *m = coups->first;
    while (m != NULL) {
        if (m->from_row == from_row && m->from_col == from_col &&
            m->to_row   == to_row   && m->to_col   == to_col)
            return m;
        m = m->next;
    }
    return NULL;
}

/* =========================================================
   Tour du joueur humain
   ========================================================= */
static Plateau tourHumain(Plateau *p, int joueur)
{
    MoveList coups;
    initMoveList(&coups);
    genererCoups(p, joueur, &coups);

    afficherCoupsDisponibles(&coups);

    char  saisie[32];
    Move *choix = NULL;

    while (choix == NULL) {
        printf("  Votre coup (ex: B6 A5) ou 'q' pour quitter : ");
        fflush(stdout);

        if (fgets(saisie, sizeof(saisie), stdin) == NULL) {
            cleanupMoveList(&coups);
            exit(0);
        }
        saisie[strcspn(saisie, "\n")] = '\0';

        if (strcmp(saisie, "q") == 0 || strcmp(saisie, "quit") == 0) {
            printf("\n  Partie abandonnee.\n");
            cleanupMoveList(&coups);
            exit(0);
        }

        int fr, fc, tr, tc;
        if (!parserSaisie(saisie, &fr, &fc, &tr, &tc)) {
            printf("  Format invalide. Exemple valide : B6 A5\n");
            continue;
        }

        choix = validerCoup(&coups, fr, fc, tr, tc);
        if (choix == NULL)
            printf("  Coup illegal. Choisissez parmi les coups affiches ci-dessus.\n");
    }

    Plateau nouveau = appliquerCoup(p, choix);
    cleanupMoveList(&coups);
    return nouveau;
}

/* =========================================================
   Tour de l'IA
   ========================================================= */
static Plateau tourIA(Plateau *p, int joueur, int profondeur)
{
    printf("  L'IA reflechit");
    fflush(stdout);

    ResultatIA r = calculerMeilleurCoup(p, joueur, profondeur);

    printf(" (%d noeuds explores)\n", r.noeudsExplores);

    if (r.meilleurCoup.from_row == -1) {
        printf("  L'IA n'a aucun coup possible.\n");
        return *p;
    }

    printf("  IA joue : %c%d -> %c%d",
           'A' + r.meilleurCoup.from_col, r.meilleurCoup.from_row + 1,
           'A' + r.meilleurCoup.to_col,   r.meilleurCoup.to_row   + 1);

    if (r.meilleurCoup.num_captures > 0) {
        printf("  [capture");
        for (int i = 0; i < r.meilleurCoup.num_captures; i++)
            printf(" %c%d",
                   'A' + r.meilleurCoup.captures[i][1],
                   r.meilleurCoup.captures[i][0] + 1);
        printf("]");
    }
    printf("\n");

    return appliquerCoup(p, &r.meilleurCoup);
}

/* =========================================================
   Choix du niveau
   ========================================================= */
static int choisirProfondeur(void)
{
    char buf[16];
    int  prof = PROFONDEUR_IA;

    printf("  Niveau IA  (3=facile  5=normal  7=difficile) [defaut=%d] : ",
           PROFONDEUR_IA);
    fflush(stdout);
    if (fgets(buf, sizeof(buf), stdin)) {
        int v = atoi(buf);
        if (v >= 1 && v <= 9) prof = v;
    }
    printf("  Profondeur choisie : %d\n\n", prof);
    return prof;
}

/* =========================================================
   Menu principal
   ========================================================= */
static void afficherMenu(void)
{
    printf("+=================================+\n");
    printf("|       JEU DE DAMES  -  C        |\n");
    printf("+=================================+\n");
    printf("|  1.  Joueur (Blancs) vs IA      |\n");
    printf("|  2.  Joueur (Noirs)  vs IA      |\n");
    printf("|  3.  IA vs IA  (demonstration)  |\n");
    printf("|  4.  Quitter                    |\n");
    printf("+=================================+\n");
    printf("  Votre choix : ");
}

/* =========================================================
   Règle de nulle : coups sans prise
   =========================================================
   Si aucune capture n'a lieu pendant MAX_COUPS_SANS_PRISE
   demi-coups consécutifs, la partie est déclarée nulle.
   ========================================================= */
#define MAX_COUPS_SANS_PRISE  80

/* =========================================================
   Boucle de jeu
   =========================================================
   modeHumain = JOUEUR_BLANC ou JOUEUR_NOIR => ce joueur est
                joué par l'humain, l'autre par l'IA.
   modeHumain = -1 => IA vs IA.
   ========================================================= */
static void jouer(int modeHumain, int profondeur)
{
    Plateau p;
    initialiserPlateau(&p);

    int coupsCansPrise = 0;
    int joueur = JOUEUR_NOIR;
    int tour   = 1;

    while (1) {
        printf("\n======  Tour %-3d  |  %s  ======\n",
               tour,
               joueur == JOUEUR_BLANC ? "BLANCS (b/B)" : "NOIRS  (n/N)");

        afficherPlateauJeu(&p);
        afficherScore(&p);

        /* --- Vérification nulle : coups sans prise --- */
        if (coupsCansPrise >= MAX_COUPS_SANS_PRISE) {
            printf("  *** NULLE ***\n");
            printf("  Aucune prise depuis %d coups consecutifs.\n\n",
                   MAX_COUPS_SANS_PRISE);
            break;
        }

        /* --- Fin de partie par victoire --- */
        if (partieTerminee(&p, joueur)) {
            int gagnant = (joueur == JOUEUR_BLANC) ? JOUEUR_NOIR : JOUEUR_BLANC;
            printf("  *** PARTIE TERMINEE ***\n");
            printf("  Les %s ont gagne !\n\n",
                   gagnant == JOUEUR_BLANC ? "BLANCS" : "NOIRS");
            break;
        }

        /* --- Compter les pièces avant le coup --- */
        int pieces_avant = compterPieces(&p, JOUEUR_BLANC)
                         + compterPieces(&p, JOUEUR_NOIR);

        /* --- Jouer le coup --- */
        if (modeHumain == joueur)
            p = tourHumain(&p, joueur);
        else
            p = tourIA(&p, joueur, profondeur);

        /* --- Mettre à jour le compteur de coups sans prise --- */
        int pieces_apres = compterPieces(&p, JOUEUR_BLANC)
                         + compterPieces(&p, JOUEUR_NOIR);

        if (pieces_apres < pieces_avant)
            coupsCansPrise = 0;
        else
            coupsCansPrise++;

        joueur = (joueur == JOUEUR_BLANC) ? JOUEUR_NOIR : JOUEUR_BLANC;
        tour++;
    }

    printf("  Plateau final :\n");
    afficherPlateauJeu(&p);
    afficherScore(&p);
}

/* =========================================================
   main
   ========================================================= */
int main(void)
{
    char buf[16];
    int  continuer = 1;

    printf("\n");
    afficherLegende();

    while (continuer) {
        afficherMenu();
        fflush(stdout);

        if (!fgets(buf, sizeof(buf), stdin)) break;
        int choix = atoi(buf);

        switch (choix) {
            case 1: { int p = choisirProfondeur(); jouer(JOUEUR_BLANC, p); break; }
            case 2: { int p = choisirProfondeur(); jouer(JOUEUR_NOIR,  p); break; }
            case 3: { int p = choisirProfondeur(); jouer(-1,           p); break; }
            case 4: continuer = 0; break;
            default: printf("  Choix invalide (1-4).\n\n");
        }
    }

    printf("  Au revoir !\n\n");
    return 0;
}