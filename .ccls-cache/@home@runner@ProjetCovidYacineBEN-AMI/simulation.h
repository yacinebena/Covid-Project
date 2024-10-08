#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdbool.h>
#include <stdio.h>

#define MAX_NOEUDS 150
#define DUREE_INFECTION 2 //en semaine (2 semaines d'infection)
#define DUREE_IMMUNITE 8  // en semaine (j'ai pris 8 semaines d'immunite on peut le modifier)

typedef struct {
    int age;
    bool maladie_courante;
    int noeuds[MAX_NOEUDS];
    int nb_noeuds;
    int nb_noeuds_max; 
    bool infectee;
    bool est_morte;
    int semaines_infection;
    int semaines_immunite;
    int nombre_infections;
} Personne;

void ajouter_noeuds(Personne *pers1, Personne *pers2, int index_pers1, int index_pers2);

void initialiser_population(Personne population[], int taille_population); 

void mettre_a_jour_deces(Personne population[], int taille_population, int semaine, FILE *fichier_morts);

bool est_infectee(Personne population[], Personne *pers, double seuil_contamination);

int compter_infectes(Personne population[], int taille_population);

void enregistrer_population(Personne population[], int taille_population, const char *fichier);

void simuler_diffusion(Personne population[], int taille_population, double seuil_contamination, int semaines);

#endif
