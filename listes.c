#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "listes.h"

/* fichier à compléter au besoin */

void init_liste_vide(liste_t *L) {
    L->tete = NULL;
}

cellule_t* nouvelle_cellule(void) {
    cellule_t *cel = malloc(sizeof(cellule_t));
    cel->val = NULL;
    cel->suivant = NULL;
    return cel;
}

int ajouter_tete(liste_t *L, string c) { /* retourne 0 si OK, 1 sinon  */
    if (L == NULL || c == NULL) {
        return 1;
    }

    cellule_t *cel = nouvelle_cellule();
    cel->val = strdup(c);
    cel->suivant = L->tete;

    L->tete = cel;
    return 0;
}


void detruire_liste(liste_t *L) {
    cellule_t *cel = NULL;
    cellule_t *cel_prec = L->tete;

    while (cel_prec != NULL) {
        cel = cel_prec->suivant;
        free(cel);
        cel_prec = cel;
    }
}


void afficher_liste(liste_t *L) {
    cellule_t *cel = L->tete;

    while (cel != NULL) {
        printf("%s ", cel->val);
        cel = cel->suivant;
    }
    printf("\n");
}
