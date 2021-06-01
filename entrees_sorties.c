#include <stdlib.h>
#include "entrees_sorties.h"
#include "common_tests.h"


void concat(char *dest, char *src, int lg) {
    int i;

    for (i = 0; src[i] != '\0'; i++) {
        dest[lg + i] = src[i];
    }
    dest[lg + i] = '\0';
}


void ecrire_fichier_dot(FILE *f, arbre racine) {
    if (racine == NULL) {
        return;
    }

    if (racine->gauche != NULL) {
        fprintf(f, "\t%s -> %s [label = \"non\"]\n", racine->valeur,
                racine->gauche->valeur);
        ecrire_fichier_dot(f, racine->gauche);
    }

    if (racine->droit != NULL) {
        fprintf(f, "\t%s -> %s [label = \"oui\"]\n", racine->valeur,
                racine->droit->valeur);
        ecrire_fichier_dot(f, racine->droit);
    }
}

void creer_fichier_dot(char *nom_fichier, arbre racine) {
    FILE *f = fopen(nom_fichier, "w");

    fprintf(f, "digraph arbre {\n");
    ecrire_fichier_dot(f, racine);
    fprintf(f, "}\n");

    fclose(f);
}


int creer_fichier_png(char *nom_fichier, int lg_fichier) {
    char *commande = malloc(23 + 2 * lg_fichier * sizeof(char));

    concat(commande, "dot -Tpng -o ", 0);
    concat(commande, nom_fichier, 13);
    concat(commande, ".png ", 13 + lg_fichier);
    concat(commande, nom_fichier, 18 + lg_fichier);
    concat(commande, ".dot", 18 + 2 * lg_fichier);

    return system(commande);
}


char* lire_entree(void) {
    char *str = NULL;
    size_t taille = 0;
    getline(&str, &taille, stdin);
    trimwhitespace(str, taille, str);
    return str;
}
