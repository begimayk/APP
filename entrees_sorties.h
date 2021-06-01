#ifndef _FICHIERS_DOT_PNG_H_
#define _FICHIERS_DOT_PNG_H_

#include <stdio.h>
#include "arbres.h"


void concat(char *dest, char *src, int lg);

void ecrire_fichier_dot(FILE *f, arbre racine);

void creer_fichier_dot(char *nom_fichier, arbre racine);

int creer_fichier_png(char *nom_fichier, int lg_fichier);

char* lire_entree(void);

#endif
