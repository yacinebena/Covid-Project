#include "simulation.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main() {
    srand(time(NULL)); 

    
    int taille_population,  semaines;
    double seuil_contamination;

    
    printf("Entrez la taille de la population (entre 2000 et 15000) : ");
    if (scanf("%d", &taille_population) != 1 || taille_population < 2000 || taille_population > 15000) {
        printf("Erreur d'entrée. La taille de la population doit être un nombre entier compris entre 2000 et 15000.\n");
        return 1;
    }

    printf("Entrez le nombre de semaines d'expérience : ");
    if (scanf("%d", &semaines) != 1 || semaines <= 0) {
        printf("Erreur d'entrée. Le nombre de semaines doit être un nombre entier positif.\n");
        return 1;
    }

    do {
        printf("Entrez le seuil de contamination (entre 0 et 1) : ");
        if (scanf("%lf", &seuil_contamination) != 1 || seuil_contamination < 0 || seuil_contamination > 1) {
            printf("Erreur d'entrée. Le seuil doit être un nombre entre 0 et 1.\n");
            while (getchar() != '\n'); 
        } else {
            break;
        }
    } while (1);

    
    Personne *population = (Personne *)malloc(taille_population * sizeof(Personne));

    if (!population) {
        perror("Erreur d'allocation mémoire");
        return 1;
    }

    initialiser_population(population, taille_population);
    simuler_diffusion(population, taille_population, seuil_contamination, semaines);

    free(population);
    return 0;
}
