#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common_tests.h"
#include "listes.h"
#include "arbres.h"
#include "arbresphylo.h"
#include "entrees_sorties.h"

#define GREEN "\033[32m"
#define RED   "\033[31m"
#define RESET "\033[0m"

int DEBUG = 0;

int main(int argc, char* argv[]) {
    char *fichier = NULL;

    if (argc < 2) {
        fprintf (stderr, "Usage:  %s [options] <fichier>\n", argv[0]);
        fprintf (stderr, "\n");
        fprintf (stderr, "Options:\n");
        fprintf (stderr, "\t-d\tmode debug\n");
        fprintf (stderr, "\t--dot\tgénère l'arbre au format GraphViz (.dot)\n");
        fprintf (stderr, "\t--png\tgénère l'arbre au format png\n");
        exit(1);
    }
    int arg = 1;
    int dot = 0;
    int png = 0;

    while (arg < argc) {
        if (!strncmp(argv[arg], "-d", 2)) {
            DEBUG = 1;
            arg++;
            continue;
        }
        if (!strncmp(argv[arg], "--dot", 5)) {
            dot = 1;
            arg++;
            continue;
        }
        if (!strncmp(argv[arg], "--png", 5)) {
            png = 1;
            arg++;
            continue;
        }
        if (argv[arg][0] == '-') {
            fprintf (stderr, "Option inconnue : '%s'\n", argv[arg]);
            exit(1);
        }
        if (fichier == NULL) {
            fichier = argv[arg];
            arg++;
            continue;
        }
        else {
            fprintf (stderr, "Trop de fichiers sur la ligne de commande : '%s'\n", argv[arg]);
            exit(1);
        }
    }

    debug ("Ouverture de %s\n", fichier);
    FILE* f = fopen (fichier, "r");
    if (!f) {
        fprintf (stderr, "Erreur à l'ouverture du fichier `%s'\n", fichier);
        perror (fichier);
        exit(1);
    }

    arbre mon_arbre = lire_arbre(f);
    fclose(f);

    int nombre_especes; /* Ne pas initialiser ici, c'est analyse_arbre */
    int nombre_caract;  /* qui doit s'en charger */

    affiche_arbre(mon_arbre);


    printf("\n\n---------- " GREEN "ACTE I-A" RESET " ----------\n\n");

    analyse_arbre(mon_arbre, &nombre_especes, &nombre_caract);

    printf ("Nombre d'especes de l'arbre: %d\n", nombre_especes);
    printf ("Nombre de caractéristiques de l'arbre: %d\n", nombre_caract);


    printf("\n\n---------- " GREEN "ACTE I-B" RESET " ----------\n\n");

    // Gestion des options --dot et --png
    if (dot || png) {
        int i;
        for (i = 0; fichier[i] != '.'; i++);
        fichier[i] = '\0';

        char *fichier_dot = malloc(5 + i * sizeof(char));
        concat(fichier_dot, fichier, 0);
        concat(fichier_dot, ".dot", i);
        creer_fichier_dot(fichier_dot, mon_arbre);

        if (dot) {
            printf("Création du fichier %s : ", fichier_dot);
            printf(GREEN "OK\n" RESET);
        }
        if (png) {
            int ret = creer_fichier_png(fichier, i);
            printf("Création du fichier %s.png : ", fichier);
            if (ret != 0) {
                printf(RED "Erreur\n" RESET);
            }
            else {
                printf(GREEN "OK\n" RESET);
            }
            if (!dot) {
                remove(fichier_dot);
            }
        }

        free(fichier_dot);
    }
    else {
        printf("Aucun fichier .dot ou .png créé\n");
        printf("Pour les créer : relancer le programme avec l'option --dot ou --png\n");
    }


    printf("\n\n---------- " GREEN "ACTE II" RESET " ----------\n\n");

    printf("Entrer un nom d'espèce :\n");
    char *espece = lire_entree();

    liste_t L;
    init_liste_vide(&L);
    int ret = rechercher_espece(mon_arbre, espece, &L);

    if (ret == 0) {
        afficher_liste(&L);
    }
    else {
        printf("Espèce non trouvée\n");
    }

    free(espece);
    detruire_liste(&L);


    printf("\n\n---------- " GREEN "ACTE III-A" RESET " ----------\n\n");

    printf("Entrer un nom d'espèce (-1 pour ne pas ajouter d'espèce et passer à l'acte III-B) :\n");
    espece = lire_entree();

    if (strcmp(espece, "-1") != 0) {
        printf("Entrer les caractéristiques de cette espèce (-1 pour terminer) :\n");
        init_liste_vide(&L);
        L.tete = nouvelle_cellule();
        L.tete->val = lire_entree();

        cellule_t *cel = NULL;
        cellule_t *cel_prec = L.tete;
        ret = strcmp(cel_prec->val, "-1");

        if (ret == 0) {
            free(cel_prec->val);
            free(cel_prec);
            L.tete = NULL;
        }

        while (ret != 0) {
            cel = nouvelle_cellule();
            cel->val = lire_entree();

            ret = strcmp(cel->val, "-1");

            if (ret != 0) {
                cel_prec->suivant = cel;
                cel_prec = cel;
            }
            else {
                cel_prec->suivant = NULL;
                free(cel->val);
                free(cel);
            }
        }

        ret = ajouter_espece(&mon_arbre, espece, L.tete);
        if (ret == 0) {
            affiche_arbre(mon_arbre);
            printf("L'espèce a bien été ajoutée\n");
        }

        detruire_liste(&L);
    }
    free(espece);


    printf("\n\n---------- " GREEN "ACTE III-B" RESET " ----------\n\n");

    f = fopen("carac_par_niveau.txt", "w");
    afficher_par_niveau(mon_arbre, f);
    printf("Ecriture des caractéristiques pour chaque niveau de l'arbre dans "
           "le fichier carac_par_niveau.txt : ");
    printf(GREEN "OK\n" RESET);


    printf("\n\n---------- " GREEN "ACTE IV" RESET " ----------\n\n");

    printf("Entrer une caractéristique (-1 pour ne pas ajouter de "
           "caractéristique et passer à l'acte V) :\n");
    char *carac = lire_entree();

    if (strcmp(carac, "-1") != 0) {
        printf("Entrer les espèces qui ont cette caractéristique (-1 pour terminer) :\n");
        init_liste_vide(&L);
        L.tete = nouvelle_cellule();
        L.tete->val = lire_entree();

        cellule_t *cel = NULL;
        cellule_t *cel_prec = L.tete;
        ret = strcmp(cel_prec->val, "-1");

        if (ret == 0) {
            free(cel_prec->val);
            free(cel_prec);
            L.tete = NULL;
        }

        while (ret != 0) {
            cel = nouvelle_cellule();
            cel->val = lire_entree();

            ret = strcmp(cel->val, "-1");

            if (ret != 0) {
                cel_prec->suivant = cel;
                cel_prec = cel;
            }
            else {
                cel_prec->suivant = NULL;
                free(cel->val);
                free(cel);
            }
        }

        ajouter_carac(&mon_arbre, carac, L.tete);

        detruire_liste(&L);
    }
    free(carac);

    affiche_arbre(mon_arbre);

    printf("\n");
    return 0;
}

