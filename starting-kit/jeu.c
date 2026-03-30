#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jeu.h"

/* Directions diagonales : haut-gauche, haut-droit, bas-gauche, bas-droit */
static const int DIRS[4][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};

/*Fonctions*/

/*vérifie si la piece est au joueur ou non*/
int estAuJoueur(int piece, int joueur)
{
    if (joueur == JOUEUR_BLANC)
        return piece == PION_BLANC || piece == DAME_BLANCHE;
    return piece == PION_NOIR || piece == DAME_NOIRE;
}

int estAdversaire(int piece, int joueur)
{
    if (piece == VIDE) return 0;
    return !estAuJoueur(piece, joueur);
}

int estDame(int piece)
{
    return piece == DAME_BLANCHE || piece == DAME_NOIRE;
}

/*compte le nombre de piece restante sur le plateau du joueur, sert a vérifié si la partie est finie*/
int compterPieces(Plateau *p, int joueur)
{
    int count = 0;
    for (int i = 0; i < TAILLE; i++)
        for (int j = 0; j < TAILLE; j++)
            if (estAuJoueur(p->grille[i][j], joueur))
                count++;
    return count;
}

/* Retourne 1 si la case est déjà dans les captures du coup en cours */
static int dejaCapturer(Move *m, int r, int c)
{
    for (int i = 0; i < m->num_captures; i++)
        if (m->captures[i][0] == r && m->captures[i][1] == c)
            return 1;
    return 0;
}

/* Ajoute le coup 'm' dans la liste (copie le contenu, réinitialise les liens) */
static void enregistrerCoup(MoveList *list, Move *m)
{
    Move *nouveau  = moveAlloc();
    *nouveau       = *m;
    nouveau->prev  = NULL;
    nouveau->next  = NULL;
    addMoveLast(list, nouveau);
}

/* Capture de pions (récursive)*/

/*
Explore récursivement toutes les séquences de captures possibles
pour un pion situé en (row, col) sur le plateau p
m         : coup en cours de construction
list      : liste où les coups complets sont ajoutés
si le pion atteint la ligne de promotion pendant la rafle,
la séquence s'arrête et le coup est enregistré*/

static void capturesPionRec(Plateau *p, int row, int col, int joueur,
                             Move *m, MoveList *list)
{
    int promo_row = LIGNE_PROMO(joueur);
    int trouve    = 0;

    for (int d = 0; d < 4; d++) {
        int mr = row + DIRS[d][0];   /* case de la pièce adverse */
        int mc = col + DIRS[d][1];
        int lr = row + 2*DIRS[d][0]; /* case d'atterrissage      */
        int lc = col + 2*DIRS[d][1];

        if (!DANS_PLATEAU(mr, mc) || !DANS_PLATEAU(lr, lc)) continue;    /*vérifie si la case adverse et d'atterrissage est dans le plateau*/
        if (!estAdversaire(p->grille[mr][mc], joueur))       continue;   /*vérifie si la piece qu'on souhaite manger est bien une piece adverse*/
        if (dejaCapturer(m, mr, mc))                         continue;   /*vérifie si la piece n'a pas déjà été capturé dans une rafle*/
        if (p->grille[lr][lc] != VIDE)                       continue;   /*la case d'atterrissage doit être vide*/

        trouve = 1;   /*on a trouver une prise*/

        /* Construire le coup partiel */
        Move copieMove = *m;
        copieMove.to_row = lr;
        copieMove.to_col = lc;
        copieMove.captures[copieMove.num_captures][0] = mr;
        copieMove.captures[copieMove.num_captures][1] = mc;
        copieMove.num_captures++;

        /* Simuler le coup sur une copie du plateau */
        Plateau copieBoard = *p;
        copieBoard.grille[lr][lc]  = copieBoard.grille[row][col];
        copieBoard.grille[row][col] = VIDE;
        copieBoard.grille[mr][mc]   = VIDE; /* pièce capturée retirée */

        /* Si le pion atteint la ligne de promotion : arrêt de la rafle */
        if (lr == promo_row) {
            enregistrerCoup(list, &copieMove);
        } else {
            capturesPionRec(&copieBoard, lr, lc, joueur, &copieMove, list);
        }
    }

    /* Aucune capture supplémentaire trouvée : enregistrer le coup complet */
    if (!trouve && m->num_captures > 0)
        enregistrerCoup(list, m);
}

/* Captures récursives  DAME */
/*
Explore récursivement toutes les séquences de captures possibles
pour une dame en (row, col).
Une dame peut sauter à n'importe quelle distance et atterrir
n'importe où au-delà de la pièce capturée (dame volante).
 */
static void capturesDameRec(Plateau *p, int row, int col, int joueur,
                              Move *m, MoveList *list)
{
    int trouve = 0;

    for (int d = 0; d < 4; d++) {
        int dr = DIRS[d][0], dc = DIRS[d][1];

        /* Chercher la première pièce dans cette direction */
        int dist = 1;
        int ar = -1, ac = -1; /* position de l'adversaire trouvé */

        while (DANS_PLATEAU(row + dist*dr, col + dist*dc)) {
            int r = row + dist*dr;
            int c = col + dist*dc;

            if (p->grille[r][c] != VIDE) {
                /* Première pièce rencontrée */
                if (estAdversaire(p->grille[r][c], joueur) &&
                    !dejaCapturer(m, r, c)) {
                    ar = r;
                    ac = c;
                }
                break; /* on s'arrête à la première pièce (amie ou ennemie) */
            }
            dist++;
        }

        if (ar < 0) continue; /* pas d'adversaire dans cette direction */

        /* Essayer toutes les cases d'atterrissage au-delà de l'adversaire */
        int lr = ar + dr;
        int lc = ac + dc;

        while (DANS_PLATEAU(lr, lc) && p->grille[lr][lc] == VIDE) {
            trouve = 1;

            Move copieMove = *m;
            copieMove.to_row = lr;
            copieMove.to_col = lc;
            copieMove.captures[copieMove.num_captures][0] = ar;
            copieMove.captures[copieMove.num_captures][1] = ac;
            copieMove.num_captures++;

            Plateau copieBoard = *p;
            copieBoard.grille[lr][lc]   = copieBoard.grille[row][col];
            copieBoard.grille[row][col]  = VIDE;
            copieBoard.grille[ar][ac]    = VIDE;

            capturesDameRec(&copieBoard, lr, lc, joueur, &copieMove, list);

            lr += dr;
            lc += dc;
        }
    }

    if (!trouve && m->num_captures > 0)
        enregistrerCoup(list, m);
}

/*Coups simples de pion (sans capture)*/

/*
 * Génère les déplacements simples (sans capture) pour un pion
 * en (row, col). Les pions ne peuvent avancer que dans leur
 * direction de jeu (1 case en diagonale vers l'avant).
 */
static void mouvementsPion(Plateau *p, int row, int col, int joueur,
                            MoveList *list)
{
    /* Direction vers l'avant selon le joueur */
    int dr = (joueur == JOUEUR_BLANC) ? -1 : 1;
    int dcols[2] = {-1, 1};

    for (int d = 0; d < 2; d++) {
        int nr = row + dr;
        int nc = col + dcols[d];

        if (!DANS_PLATEAU(nr, nc))       continue;  /*vérifie si le coup est sur le plateau*/
        if (p->grille[nr][nc] != VIDE)   continue;  /*vérifie si la case est libre */

        Move *move      = moveAlloc();
        move->from_row  = row;
        move->from_col  = col;
        move->to_row    = nr;
        move->to_col    = nc;
        addMoveLast(list, move);
    }
}

/* Coups simples de DAME (sans capture)*/

/*La dame se déplace qur toute la diagonale en avant et en arrière*/
static void mouvementsDame(Plateau *p, int row, int col, MoveList *list)
{
    for (int d = 0; d < 4; d++) {
        int dr = DIRS[d][0], dc = DIRS[d][1];
        int nr = row + dr;
        int nc = col + dc;

        while (DANS_PLATEAU(nr, nc) && p->grille[nr][nc] == VIDE) {
            Move *move      = moveAlloc();
            move->from_row  = row;
            move->from_col  = col;
            move->to_row    = nr;
            move->to_col    = nc;
            addMoveLast(list, move);

            nr += dr;
            nc += dc;
        }
    }
}

/*gnération des coups légaux*/

void genererCoups(Plateau *p, int joueur, MoveList *list)
{
    MoveList captures;
    initMoveList(&captures);

    /* chercher toutes les captures disponibles*/
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            if (!estAuJoueur(p->grille[i][j], joueur)) continue;

            /* Initialiser un coup vide depuis (i,j) */
            Move depart;
            depart.from_row     = i;
            depart.from_col     = j;
            depart.to_row       = i;
            depart.to_col       = j;
            depart.num_captures = 0;
            depart.prev         = NULL;
            depart.next         = NULL;

            if (estDame(p->grille[i][j]))
                capturesDameRec(p, i, j, joueur, &depart, &captures);
            else
                capturesPionRec(p, i, j, joueur, &depart, &captures);
        }
    }

    /* si des captures existent : prise obligatoire */
    if (!moveListIsEmpty(&captures)) {
        /* transférer les captures dans la liste de sortie */
        Move *m;
        while ((m = popMoveFirst(&captures)) != NULL)
            addMoveLast(list, m);
        return;
    }

    /* sinon : générer les déplacements simples*/
    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) {
            if (!estAuJoueur(p->grille[i][j], joueur)) continue;

            if (estDame(p->grille[i][j]))
                mouvementsDame(p, i, j, list);
            else
                mouvementsPion(p, i, j, joueur, list);
        }
    }
}

/* Promotion*/

void verifierPromotion(Plateau *p)
{
    for (int j = 0; j < TAILLE; j++) {
        /* Ligne 0 : promotion des Blancs */
        if (p->grille[0][j] == PION_BLANC)
            p->grille[0][j] = DAME_BLANCHE;

        /* Ligne TAILLE-1 : promotion des Noirs */
        if (p->grille[TAILLE-1][j] == PION_NOIR)
            p->grille[TAILLE-1][j] = DAME_NOIRE;
    }
}

/*= Application d'un coup*/

Plateau appliquerCoup(Plateau *p, Move *m)
{
    Plateau nouveau = *p; /* copie du plateau */

    /* déplace la pièce */
    nouveau.grille[m->to_row][m->to_col]     = nouveau.grille[m->from_row][m->from_col];
    nouveau.grille[m->from_row][m->from_col] = VIDE;

    /* retirer les pièces capturées */
    for (int i = 0; i < m->num_captures; i++)
        nouveau.grille[m->captures[i][0]][m->captures[i][1]] = VIDE;

    /* vérifie les promotions */
    verifierPromotion(&nouveau);

    return nouveau;
}

/* Fin de partie*/

int partieTerminee(Plateau *p, int joueur)
{
    /* plus de pièces */
    if (compterPieces(p, joueur) == 0)
        return 1;

    /* plus de coup légal */
    MoveList coups;
    initMoveList(&coups);
    genererCoups(p, joueur, &coups);
    int aucunCoup = moveListIsEmpty(&coups);
    cleanupMoveList(&coups);
    return aucunCoup;
}
