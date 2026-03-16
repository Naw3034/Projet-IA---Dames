# ============================================================
#  Makefile — Jeu de Dames en C
# ============================================================

# Exécutable principal
EXEC      = dames

# Exécutable de tests unitaires
TEST_EXEC = test_list

# Compilateur et options
CC        = gcc
CFLAGS    = -Wall -Wextra -g

# Fichiers objets du jeu principal
#   main.o sera ajouté dès que main.c sera créé
OBJ       = plateau.o list.o movelist.o jeu.o eval.o ia.o

# Fichiers objets pour les tests de la liste
TEST_OBJ  = list.o test_list.o

# ============================================================
#  Règle par défaut
# ============================================================
all: $(EXEC)

# ============================================================
#  Lien de l'exécutable principal
#   (nécessite main.o — sera disponible à la phase 5)
# ============================================================
$(EXEC): main.o $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# ============================================================
#  Lien de l'exécutable de tests
# ============================================================
$(TEST_EXEC): $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# ============================================================
#  Règle générique : compilation de chaque .c en .o
# ============================================================
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================================
#  Dépendances explicites (pour la recompilation automatique)
# ============================================================
main.o:      main.c      plateau.h list.h item.h move.h movelist.h jeu.h eval.h ia.h
plateau.o:   plateau.c   plateau.h
list.o:      list.c      list.h item.h
movelist.o: movelist.c movelist.h move.h
jeu.o:       jeu.c      jeu.h plateau.h movelist.h move.h
eval.o:      eval.c     eval.h jeu.h plateau.h movelist.h
ia.o:        ia.c       ia.h eval.h jeu.h plateau.h movelist.h
test_list.o: test_list.c list.h item.h

# ============================================================
#  Cibles utilitaires
# ============================================================

# Compiler et lancer le jeu
run: $(EXEC)
	./$(EXEC)

# Compiler et lancer les tests unitaires
test: $(TEST_EXEC)
	./$(TEST_EXEC)

# Supprimer tous les fichiers générés
clean:
	rm -f $(OBJ) main.o test_list.o $(EXEC) $(TEST_EXEC)

.PHONY: all run test clean