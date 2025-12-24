#include "rapports_graphiques.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Affiche une barre horizontale
void afficherHistogrammeHorizontal(const char *label, float valeur, float max, char *symbole) {
    if (max == 0) max = 1;
    int barre_length = (int)((valeur / max) * MAX_BAR_WIDTH);
    if (barre_length > MAX_BAR_WIDTH) barre_length = MAX_BAR_WIDTH;

    printf("%-25s | ", label);
    for (int i = 0; i < barre_length; i++) {
        printf("%s", symbole);
    }
    printf(" %.2f\n", valeur);
}

// Affiche un graphique en barres
void afficherGraphiqueBarres(const char *titre, const char *categories[], float valeurs[], int count) {
    printf("\n=== %s ===\n", titre);

    if (count == 0) {
        printf("Aucune donnée à afficher.\n");
        return;
    }

    float max = obtenirMaxValeur(valeurs, count);

    for (int i = 0; i < count; i++) {
        afficherHistogrammeHorizontal(categories[i], valeurs[i], max, "▓");
    }
    printf("\n");
}

// Affiche un graphique camembert en ASCII (simplifié)
void afficherGraphiqueCamembert(const char *titre, const char *categories[], float valeurs[], int count) {
    printf("\n=== %s (Graphique Camembert) ===\n", titre);

    if (count == 0) {
        printf("Aucune donnée à afficher.\n");
        return;
    }

    float total = 0;
    for (int i = 0; i < count; i++) {
        total += valeurs[i];
    }

    if (total == 0) {
        printf("Total est zéro.\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        float pourcentage = (valeurs[i] / total) * 100;
        int largeur = (int)(pourcentage / 2); // 50 caractères max = 100%
        if (largeur == 0 && valeurs[i] > 0) largeur = 1;

        printf("%-25s | ", categories[i]);
        for (int j = 0; j < largeur; j++) {
            printf("█");
        }
        printf(" %.1f%% (%.2f)\n", pourcentage, valeurs[i]);
    }
    printf("\n");
}

// Rapport des dépenses par catégorie
void afficherRapportDepensesParCategorie(Transaction *depenses) {
    const char *categories[] = {
        "Nourriture", "Logement", "Transport", "Loisirs",
        "Santé", "Éducation", "Autre"
    };
    float montants[7] = {0};

    Transaction *current = depenses;
    while (current != NULL) {
        // Parcourir les dépenses et les catégoriser
        // Note: cette implémentation est simplifiée
        // Dans une vraie implémentation, on aurait besoin d'accéder à la catégorie
        current = current->next;
    }

    afficherGraphiqueBarres("DÉPENSES PAR CATÉGORIE", categories, montants, 7);
}

// Rapport des revenus par catégorie
void afficherRapportRevenusParCategorie(Transaction *revenus) {
    const char *categories[] = {
        "Salaire", "Business", "Investissement", "Cadeau", "Autre"
    };
    float montants[5] = {0};

    afficherGraphiqueBarres("REVENUS PAR CATÉGORIE", categories, montants, 5);
}

// Rapport mensuel comparatif
void afficherRapportComparatifMois(Transaction *depenses, Transaction *revenus, const char *mois) {
    float total_depenses = 0, total_revenus = 0;

    Transaction *current = depenses;
    while (current != NULL) {
        if (strncmp(current->date, mois, 7) == 0) {
            total_depenses += current->montant;
        }
        current = current->next;
    }

    current = revenus;
    while (current != NULL) {
        if (strncmp(current->date, mois, 7) == 0) {
            total_revenus += current->montant;
        }
        current = current->next;
    }

    printf("\n=== RAPPORT MENSUEL: %s ===\n", mois);
    printf("Revenus:     %.2f €\n", total_revenus);
    printf("Dépenses:    %.2f €\n", total_depenses);
    printf("Solde:       %.2f €\n", total_revenus - total_depenses);
    printf("Épargne:     %.1f%%\n", total_revenus > 0 ? ((total_revenus - total_depenses) / total_revenus * 100) : 0);

    const char *cat[] = {"Revenus", "Dépenses"};
    float val[] = {total_revenus, total_depenses};
    afficherGraphiqueCamembert("DISTRIBUTION REVENUS/DÉPENSES", cat, val, 2);
}

// Rapport annuel
void afficherRapportAnnuel(Transaction *depenses, Transaction *revenus, const char *annee) {
    float mois_depenses[12] = {0};
    float mois_revenus[12] = {0};
    const char *mois_noms[] = {
        "Jan", "Fév", "Mar", "Avr", "Mai", "Jun",
        "Jul", "Aoû", "Sep", "Oct", "Nov", "Déc"
    };

    // Parcourir les dépenses
    Transaction *current = depenses;
    while (current != NULL) {
        if (strncmp(current->date, annee, 4) == 0) {
            int mois = atoi(current->date + 5) - 1;
            if (mois >= 0 && mois < 12) {
                mois_depenses[mois] += current->montant;
            }
        }
        current = current->next;
    }

    // Parcourir les revenus
    current = revenus;
    while (current != NULL) {
        if (strncmp(current->date, annee, 4) == 0) {
            int mois = atoi(current->date + 5) - 1;
            if (mois >= 0 && mois < 12) {
                mois_revenus[mois] += current->montant;
            }
        }
        current = current->next;
    }

    printf("\n=== RAPPORT ANNUEL: %s ===\n", annee);
    printf("%-5s | %12s | %12s | %12s\n", "Mois", "Revenus", "Dépenses", "Solde");
    printf("-----+--------------+--------------+---------\n");

    float total_rev = 0, total_dep = 0;
    for (int i = 0; i < 12; i++) {
        printf("%-5s | %10.2f € | %10.2f € | %10.2f €\n",
               mois_noms[i],
               mois_revenus[i],
               mois_depenses[i],
               mois_revenus[i] - mois_depenses[i]);
        total_rev += mois_revenus[i];
        total_dep += mois_depenses[i];
    }
    printf("-----+--------------+--------------+---------\n");
    printf("TOTAL | %10.2f € | %10.2f € | %10.2f €\n",
           total_rev, total_dep, total_rev - total_dep);

    afficherGraphiqueBarres("TENDANCE MENSUELLE - DÉPENSES", (const char **)mois_noms, mois_depenses, 12);
    afficherGraphiqueBarres("TENDANCE MENSUELLE - REVENUS", (const char **)mois_noms, mois_revenus, 12);
}

// Calcule les statistiques
StatistiquesFinancieres calculerStatistiques(Transaction *tete) {
    StatistiquesFinancieres stats = {0, 0, 0, 999999, 0, 0};

    if (tete == NULL) {
        return stats;
    }

    float *montants = NULL;
    int count = 0;

    // Compter et accumuler
    Transaction *current = tete;
    while (current != NULL) {
        stats.total += current->montant;
        if (current->montant < stats.min) stats.min = current->montant;
        if (current->montant > stats.max) stats.max = current->montant;
        count++;
        current = current->next;
    }

    if (count == 0) {
        return stats;
    }

    stats.count = count;
    stats.moyenne = stats.total / count;

    // Calculer la médiane
    montants = (float *)malloc(count * sizeof(float));
    if (montants != NULL) {
        int i = 0;
        current = tete;
        while (current != NULL) {
            montants[i++] = current->montant;
            current = current->next;
        }
        stats.median = calculerMediane(montants, count);
        free(montants);
    }

    return stats;
}

// Affiche les statistiques
void afficherStatistiques(const char *titre, StatistiquesFinancieres stats) {
    printf("\n=== STATISTIQUES: %s ===\n", titre);
    printf("Nombre de transactions: %d\n", stats.count);
    printf("Total:                  %.2f €\n", stats.total);
    printf("Moyenne:                %.2f €\n", stats.moyenne);
    printf("Médiane:                %.2f €\n", stats.median);
    printf("Minimum:                %.2f €\n", stats.min);
    printf("Maximum:                %.2f €\n", stats.max);
}

// Comparaison entre deux périodes
void afficherComparaisonPeriodes(Transaction *depenses, const char *date1, const char *date2) {
    float total1 = 0, total2 = 0;

    Transaction *current = depenses;
    while (current != NULL) {
        if (strcmp(current->date, date1) >= 0 && strcmp(current->date, date2) <= 0) {
            total1 += current->montant;
        }
        current = current->next;
    }

    printf("\n=== COMPARAISON PÉRIODE ===\n");
    printf("Période: %s à %s\n", date1, date2);
    printf("Total dépenses: %.2f €\n", total1);
}

// Tendances mensuelles
void afficherTendancesMois(Transaction *depenses, const char *annee) {
    afficherRapportAnnuel(depenses, NULL, annee);
}

// Obtient la valeur maximale
float obtenirMaxValeur(float *valeurs, int count) {
    if (count == 0) return 1;
    float max = valeurs[0];
    for (int i = 1; i < count; i++) {
        if (valeurs[i] > max) max = valeurs[i];
    }
    return max > 0 ? max : 1;
}

// Calcule la médiane
float calculerMediane(float *valeurs, int count) {
    if (count == 0) return 0;

    // Bubble sort simple
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (valeurs[j] > valeurs[j + 1]) {
                float temp = valeurs[j];
                valeurs[j] = valeurs[j + 1];
                valeurs[j + 1] = temp;
            }
        }
    }

    if (count % 2 == 0) {
        return (valeurs[count / 2 - 1] + valeurs[count / 2]) / 2;
    } else {
        return valeurs[count / 2];
    }
}
