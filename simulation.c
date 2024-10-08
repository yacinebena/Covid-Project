#include "simulation.h"
#include <stdlib.h>

void ajouter_noeuds(Personne *pers1, Personne *pers2, int index_pers1, int index_pers2) {
    if (pers1->nb_noeuds < pers1->nb_noeuds_max) {
        pers1->noeuds[pers1->nb_noeuds++] = index_pers2;
    }
    if (pers2->nb_noeuds < pers2->nb_noeuds_max) {
        pers2->noeuds[pers2->nb_noeuds++] = index_pers1;
    }
}

    void initialiser_population(Personne population[], int taille_population) {
    for (int i = 0; i < taille_population; ++i) {
        population[i].age = rand() % 100 + 1; //on evalue sur une population avec comme age max 100 ans
        population[i].maladie_courante = (rand() % 100) < 30; //30% de la pop francaise a une maladie courante
        population[i].nb_noeuds = 0;
        population[i].nb_noeuds_max = rand() % 21 + 5; // j'ai pris de 5 à 25 noeuds (modifiable )
        population[i].infectee = false;
        population[i].semaines_infection = 0;
        population[i].semaines_immunite = 0;
        population[i].nombre_infections = 0;
    }
    
        for (int i = 0; i < taille_population; ++i) {
            for (int j = 0; j < population[i].nb_noeuds_max;             ++j) {
                int autre = rand() % taille_population;
                if (autre != i) {ajouter_noeuds(&population[i],&population[autre], i, autre);
                 }
             }
    }
}

void mettre_a_jour_deces(Personne population[], int taille_population, int semaine, FILE *fichier_morts) {
    int nombre_morts = 0;
    for (int i = 0; i < taille_population; ++i) {
        if (population[i].infectee && !population[i].est_morte) {
            double probabilite_deces = 0.001;
            if (population[i].maladie_courante) probabilite_deces += 0.1;
            if (population[i].age > 75) probabilite_deces += 0.1;
            if (population[i].maladie_courante && population[i].age > 75) probabilite_deces += 0.30;
            double random = (double)rand() / RAND_MAX;
            if (random < probabilite_deces) population[i].est_morte = true;
            if (population[i].est_morte) nombre_morts++;
        }
    }


    fprintf(fichier_morts, "Semaine %d : Nombre de morts : %d\n", semaine, nombre_morts);
}

bool est_infectee(Personne population[], Personne *pers, double seuil_contamination) {
    bool a_noeuds_infectee = false;
    for (int i = 0; i < pers->nb_noeuds; ++i) {
        int index_noeuds = pers->noeuds[i];
        if (population[index_noeuds].infectee) {
            a_noeuds_infectee = true;
            break;
        }
    }
    if (a_noeuds_infectee) {
        double random = (double)rand() / RAND_MAX;
        return random <= seuil_contamination && pers->semaines_immunite == 0;
    }
    return false;
}

int compter_infectes(Personne population[], int taille_population) {
    int total = 0;
    for (int i = 0; i < taille_population; ++i) if (population[i].infectee) total++;
    return total;
}


void enregistrer_population(Personne population[], int taille_population, const char *fichier) {
    FILE *fichier_details = fopen(fichier, "w");
    if (!fichier_details) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }
    fprintf(fichier_details, "Voici les détails de la population simulée du COVID-19 avec %d personnes\n\n", taille_population);

    fprintf(fichier_details, "%-4s\t%-18s\t%-18s\t%-10s\n", "Age", "Maladie_Courante", "Nombre_Infections", "Décédé?");


    for (int i = 0; i < taille_population; ++i) {
        fprintf(fichier_details, "%-4d\t%-18s\t%-18d\t%-10s\n",
            population[i].age,
            population[i].maladie_courante? "Oui" : "Non",
            population[i].nombre_infections,
            population[i].est_morte ? "Oui" : "Non"
        );
    }

    fclose(fichier_details);
}


void simuler_diffusion(Personne population[], int taille_population, double seuil_contamination, int semaines) {
    for (int i = 0; i < 10; ++i) {
        int index = rand() % taille_population;
        population[index].infectee = true;
        population[index].semaines_infection = DUREE_INFECTION;
        population[index].nombre_infections++;
    }

    FILE *fichier_resultats = fopen("resultats_simulation.txt", "w");
    FILE *fichier_morts = fopen("nombre_morts.txt", "w");

    if (!fichier_resultats || !fichier_morts) {
        perror("Erreur lors de l'ouverture des fichiers");
        if (fichier_resultats) fclose(fichier_resultats);
        if (fichier_morts) fclose(fichier_morts);
        return;
    }
    fprintf(fichier_resultats, "Voici les résultats d'une simulation du COVID-19 avec %d personnes sur %d semaines\n\n", taille_population, semaines);
    fprintf(fichier_morts, "Voici le nombre de deces par semaine d'une simulation du COVID-19 avec %d personnes sur %d semaines\n\n", taille_population, semaines);

    
    bool *nouvel_infecte = (bool *)calloc(taille_population, sizeof(bool));
    if (!nouvel_infecte) {
        perror("Erreur d'allocation mémoire");
        fclose(fichier_resultats);
        fclose(fichier_morts);
        return;
    }

    for (int semaine = 0; semaine < semaines; ++semaine) {
        for (int i = 0; i < taille_population; ++i) {
            nouvel_infecte[i] = false;
            if (!population[i].infectee && est_infectee(population, &population[i], seuil_contamination)) {
                nouvel_infecte[i] = true;
            }
        }

        for (int i = 0; i < taille_population; ++i) {
            if (nouvel_infecte[i]) {
                population[i].infectee = true;
                population[i].semaines_infection = DUREE_INFECTION;
                population[i].nombre_infections++;
            } else if (population[i].semaines_infection > 0) {
                population[i].semaines_infection--;
                if (population[i].semaines_infection == 0) {
                    population[i].infectee = false;
                    population[i].semaines_immunite = DUREE_IMMUNITE;
                }
            } else if (population[i].semaines_immunite > 0) {
                population[i].semaines_immunite--;
            }
        }

        
        mettre_a_jour_deces(population, taille_population, semaine + 1, fichier_morts);
        int total_infectes = compter_infectes(population, taille_population);
        fprintf(fichier_resultats, "Semaine %d : Nombre total de personnes infectées : %d\n", semaine + 1, total_infectes);
    }

    fclose(fichier_resultats);
    fclose(fichier_morts);

    enregistrer_population(population, taille_population, "details_population.txt");

    free(nouvel_infecte);
}
