#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "arbres.h"
#include "arbresphylo.h"
#include "listes.h"


/* ACTE I */
/* Analyse un arbre: doit mettre à jour le nombre d'espèces et de
 * caractéristiques.
 */
void analyse_arbre(arbre racine, int* nb_esp, int* nb_carac) {
    if (racine == NULL) {
        *nb_esp = 0;
        *nb_carac = 0;
        return;
    }

    if (racine->gauche == NULL && racine->droit == NULL) {
        (*nb_esp)++;
        return;
    }

    int esp_g = 0, carac_g = 0;
    analyse_arbre(racine->gauche, &esp_g, &carac_g);

    int esp_d = 0, carac_d = 0;
    analyse_arbre(racine->droit, &esp_d, &carac_d);

    *nb_esp = esp_g + esp_d;
    *nb_carac = 1 + carac_g + carac_d;
}


/* ACTE II */
/* Recherche l'espece dans l'arbre. Modifie la liste passée en paramètre pour y mettre les
 * caractéristiques.
 * Retourne 0 si l'espèce a été retrouvée, 1 sinon.
 */
int rechercher_espece(arbre racine, char *espece, liste_t *seq) {
    assert(seq != NULL && espece != NULL);

    if (racine == NULL) {
        return 1;
    }

    if (!strcmp(racine->valeur, espece)) {
        return 0;
    }

    int g = rechercher_espece(racine->gauche, espece, seq);
    if (g == 0) {
        return 0;
    }

    int d = rechercher_espece(racine->droit, espece, seq);
    if (d == 0) {
        ajouter_tete(seq, racine->valeur);
        return 0;
    }

    return 1;
}


/* ACTE III-A */
/* Doit renvoyer 0 si l'espece a bien ete ajoutee, 1 sinon, et ecrire un
 * message d'erreur.
 */
int ajouter_espece(arbre *a, char *espece, cellule_t *seq) {
    assert(a != NULL && espece != NULL);

    if (seq == NULL) {
        if (*a == NULL) {
            *a = nouveau_noeud();
            (*a)->valeur = strdup(espece);
            return 0;
        }
        if ((*a)->gauche == NULL && (*a)->droit == NULL) {
            fprintf(stderr, "Ne peut ajouter %s: possède les mêmes caractères "
                    "que %s\n", espece, (*a)->valeur);
            return 1;
        }
        if ((*a)->gauche == NULL) {
            (*a)->gauche = nouveau_noeud();
            (*a)->gauche->valeur = strdup(espece);
            return 0;
        }
        return ajouter_espece(&(*a)->gauche, espece, seq);
    }

    if (*a == NULL) {
        *a = nouveau_noeud();
        (*a)->valeur = strdup(seq->val);
        return ajouter_espece(&(*a)->droit, espece, seq->suivant);
    }
    if ((*a)->gauche == NULL && (*a)->droit == NULL) {
        arbre esp = *a;
        *a = nouveau_noeud();
        (*a)->valeur = strdup(seq->val);
        (*a)->gauche = esp;
        return ajouter_espece(&(*a)->droit, espece, seq->suivant);
    }
    if (strcmp((*a)->valeur, seq->val) == 0) {
        return ajouter_espece(&(*a)->droit, espece, seq->suivant);
    }
    return ajouter_espece(&(*a)->gauche, espece, seq);
}


/* ACTE III-B */
int profondeur_arbre(arbre a) {
    if (a == NULL) {
        return 0;
    }

    int g = profondeur_arbre(a->gauche);
    int d = profondeur_arbre(a->droit);

    return (g > d) ? 1 + g : 1 + d;
}

void afficher_niveau(arbre a, int niveau, int niveau_attendu, FILE *f) {
    if (a == NULL) {
        return;
    }
    if (a->gauche == NULL && a->droit == NULL) {
        return;
    }
    if (niveau == niveau_attendu) {
        fprintf(f, "%s ", a->valeur);
        return;
    }

    afficher_niveau(a->gauche, niveau+1, niveau_attendu, f);
    afficher_niveau(a->droit, niveau+1, niveau_attendu, f);
}

void afficher_par_niveau(arbre a, FILE *f) {
    int profondeur = profondeur_arbre(a);

    for (int i = 1; i <= profondeur; i++) {
        afficher_niveau(a, 1, i, f);
        fprintf(f, "\n");
    }
}


/* ACTE IV */
int ajouter_carac(arbre *a, char *carac, cellule_t *seq) {
    assert(a != NULL && carac != NULL);

    if (*a == NULL) {
        fprintf(stderr, "Ne peut ajouter %s: l'arbre est vide.\n", carac);
        return 1;
    }

    if (seq == NULL) {
        fprintf(stderr, "Ne peut ajouter %s: la séquence d'espèces est vide.\n", carac);
        return 1;
    }

    if ((*a)->gauche == NULL && (*a)->droit == NULL) {
        while (seq != NULL && strcmp((*a)->valeur, seq->val) != 0) {
            seq = seq->suivant;
        }
        if (seq == NULL) {
            return 1;
        }
        seq->val[0] = '\0';
        return 0;
    }

    int g = ajouter_carac(&(*a)->gauche, carac, seq);
    int d = ajouter_carac(&(*a)->droit, carac, seq);

    if (g == 0 && d == 0) {
        return 0;
    }

    int tous_trouves = 1;
    if (g == 0 || d == 0) {
        while (seq != NULL) {
            if (seq->val[0] != '\0') {
                tous_trouves = 0;
            }
            seq = seq->suivant;
        }
    }

    if (g == 0 && tous_trouves) {
        arbre t = (*a)->gauche;
        (*a)->gauche = nouveau_noeud();
        (*a)->gauche->valeur = strdup(carac);
        (*a)->gauche->droit = t;
        return 1;
    }
    if (d == 0 && tous_trouves) {
        arbre t = (*a)->droit;
        (*a)->droit = nouveau_noeud();
        (*a)->droit->valeur = strdup(carac);
        (*a)->droit->droit = t;
        return 1;
    }

    if (g == 0 || d == 0) {
        fprintf(stderr, "Ne peut ajouter %s: ne forme pas un sous-arbre.\n", carac);
    }

    return 1;
}
