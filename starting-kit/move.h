#ifndef MOVE_H
#define MOVE_H

/* Nombre maximum de captures possibles en une seule rafle */
#define MAX_CAPTURES 8

/*
 * Structure représentant un coup joué sur le plateau.
 *
 * Un coup = une pièce qui part de (from_row, from_col)
 *           et arrive en   (to_row,   to_col).
 *
 * En cas de prise multiple (rafle), toutes les cases capturées
 * sont stockées dans le tableau 'captures'.
 */
typedef struct Move_s {
    int from_row, from_col;             /* case source                  */
    int to_row,   to_col;               /* case destination             */
    int captures[MAX_CAPTURES][2];      /* [i][0]=ligne, [i][1]=colonne */
    int num_captures;                   /* nombre de pièces capturées   */

    struct Move_s *prev, *next;         /* chaînage doublement lié      */
} Move;

#endif