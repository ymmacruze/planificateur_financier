#include "analyse_tendances.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Calcule l'évolution entre deux valeurs
TendanceEvolution calculerEvolution(float ancien, float nouveau) {
    TendanceEvolution evolution;
    evolution.montant_precedent = ancien;
    evolution.montant_courant = nouveau;
    evolution.variation_absolue = nouveau - ancien;

    if (ancien != 0) {
        evolution.variation_pourcentage = (evolution.variation_absolue / ancien) * 100;
    } else {
        evolution.variation_pourcentage = nouveau > 0 ? 100 : 0;
    }

    if (evolution.variation_absolue > 0.1) {
        evolution.tendance = 1; // Hausse
    } else if (evolution.variation_absolue < -0.1) {
        evolution.tendance = -1; // Baisse
    } else {
        evolution.tendance = 0; // Stable
    }

    return evolution;
}

// Affiche l'évolution
void afficherEvolution(const char *label, TendanceEvolution evolution) {
    const char *tendance_str;
    if (evolution.tendance == 1) {
        tendance_str = "↑ HAUSSE";
    } else if (evolution.tendance == -1) {
        tendance_str = "↓ BAISSE";
    } else {
        tendance_str = "→ STABLE";
    }

    printf("%-30s | Avant: %.2f € | Après: %.2f € | %s (%.1f%%)\n",
           label,
           evolution.montant_precedent,
           evolution.montant_courant,
           tendance_str,
           evolution.variation_pourcentage);
}

// Analyser les tendances mensuelles
void analyserTendancesMois(Transaction *tete, const char *annee) {
    float mois_montants[12] = {0};
    const char *mois_noms[] = {
        "Janvier", "Février", "Mars", "Avril", "Mai", "Juin",
        "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre"
    };

    Transaction *current = tete;
    while (current != NULL) {
        if (strncmp(current->date, annee, 4) == 0) {
            int mois = atoi(current->date + 5) - 1;
            if (mois >= 0 && mois < 12) {
                mois_montants[mois] += current->montant;
            }
        }
        current = current->next;
    }

    printf("\n=== ANALYSE DES TENDANCES MENSUELLES: %s ===\n", annee);
    for (int i = 1; i < 12; i++) {
        if (mois_montants[i-1] > 0) {
            TendanceEvolution evolution = calculerEvolution(mois_montants[i-1], mois_montants[i]);
            afficherEvolution(mois_noms[i], evolution);
        }
    }
}

// Analyser les tendances par catégorie
void analyserTendancesCategories(Transaction *tete) {
    printf("\n=== ANALYSE PAR CATÉGORIE ===\n");
    // À implémenter avec accès aux catégories
}

// Obtient la moyenne mensuelle
float obtenirMoyenneMensuelle(Transaction *tete) {
    float total = 0;
    int count = 0;

    Transaction *current = tete;
    while (current != NULL) {
        total += current->montant;
        count++;
        current = current->next;
    }

    return (count > 0) ? (total / count) : 0;
}

// Obtient l'écart type
float obtenirEcartType(Transaction *tete) {
    float moyenne = obtenirMoyenneMensuelle(tete);
    float somme_carres = 0;
    int count = 0;

    Transaction *current = tete;
    while (current != NULL) {
        float diff = current->montant - moyenne;
        somme_carres += diff * diff;
        count++;
        current = current->next;
    }

    if (count == 0) return 0;
    return sqrt(somme_carres / count);
}

// Vérifie si une valeur est anormale
int estAnomalique(float valeur, float moyenne, float ecart_type) {
    if (ecart_type == 0) return 0;
    float z_score = fabs((valeur - moyenne) / ecart_type);
    return z_score > 3; // Plus de 3 écarts types = anomalique
}

// Détecte les anomalies
void detecterAnomalies(Transaction *tete) {
    float moyenne = obtenirMoyenneMensuelle(tete);
    float ecart_type = obtenirEcartType(tete);

    printf("\n=== DÉTECTION D'ANOMALIES ===\n");
    printf("Moyenne: %.2f €\n", moyenne);
    printf("Écart type: %.2f €\n", ecart_type);
    printf("\nTransactions anormales:\n");

    int anomalies_count = 0;
    Transaction *current = tete;
    while (current != NULL) {
        if (estAnomalique(current->montant, moyenne, ecart_type)) {
            anomalies_count++;
            printf("⚠️  ID %d - %s: %.2f € (écart significatif)\n",
                   current->id,
                   current->date,
                   current->montant);
        }
        current = current->next;
    }

    if (anomalies_count == 0) {
        printf("Aucune anomalie détectée.\n");
    }
}

// Obtient le taux de croissance
float obtenirTaux_croissance(Transaction *tete, const char *periode1, const char *periode2) {
    float total1 = 0, total2 = 0;

    Transaction *current = tete;
    while (current != NULL) {
        if (strncmp(current->date, periode1, 7) == 0) {
            total1 += current->montant;
        }
        if (strncmp(current->date, periode2, 7) == 0) {
            total2 += current->montant;
        }
        current = current->next;
    }

    if (total1 == 0) return 0;
    return ((total2 - total1) / total1) * 100;
}

// Affiche l'analyse de croissance
void afficherAnalyse_croissance(Transaction *tete, const char *annee) {
    const char *mois_codes[] = {
        "01", "02", "03", "04", "05", "06",
        "07", "08", "09", "10", "11", "12"
    };
    const char *mois_noms[] = {
        "Janvier", "Février", "Mars", "Avril", "Mai", "Juin",
        "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre"
    };

    printf("\n=== ANALYSE DE CROISSANCE: %s ===\n", annee);

    for (int i = 1; i < 12; i++) {
        char periode1[8], periode2[8];
        sprintf(periode1, "%s-%s", annee, mois_codes[i-1]);
        sprintf(periode2, "%s-%s", annee, mois_codes[i]);

        float taux = obtenirTaux_croissance(tete, periode1, periode2);
        printf("%s → %s: %+.1f%%\n", mois_noms[i-1], mois_noms[i], taux);
    }
}

// Analyser la saisonnalité
void analyserSaisonnalite(Transaction *tete, const char *annee) {
    printf("\n=== ANALYSE SAISONNIÈRE: %s ===\n", annee);

    float trimestre1 = 0, trimestre2 = 0, trimestre3 = 0, trimestre4 = 0;
    int count1 = 0, count2 = 0, count3 = 0, count4 = 0;

    Transaction *current = tete;
    while (current != NULL) {
        if (strncmp(current->date, annee, 4) == 0) {
            int mois = atoi(current->date + 5) - 1;

            if (mois >= 0 && mois < 3) {
                trimestre1 += current->montant;
                count1++;
            } else if (mois >= 3 && mois < 6) {
                trimestre2 += current->montant;
                count2++;
            } else if (mois >= 6 && mois < 9) {
                trimestre3 += current->montant;
                count3++;
            } else if (mois >= 9 && mois < 12) {
                trimestre4 += current->montant;
                count4++;
            }
        }
        current = current->next;
    }

    float moy1 = (count1 > 0) ? (trimestre1 / count1) : 0;
    float moy2 = (count2 > 0) ? (trimestre2 / count2) : 0;
    float moy3 = (count3 > 0) ? (trimestre3 / count3) : 0;
    float moy4 = (count4 > 0) ? (trimestre4 / count4) : 0;

    printf("T1 (Jan-Mar): %.2f € (Moyenne: %.2f €)\n", trimestre1, moy1);
    printf("T2 (Avr-Jun): %.2f € (Moyenne: %.2f €)\n", trimestre2, moy2);
    printf("T3 (Jul-Sep): %.2f € (Moyenne: %.2f €)\n", trimestre3, moy3);
    printf("T4 (Oct-Déc): %.2f € (Moyenne: %.2f €)\n", trimestre4, moy4);
}

// Affiche les moyennes saisonnières
void afficherMoyennesSaisonnières(Transaction *tete, const char *annee) {
    analyserSaisonnalite(tete, annee);
}

// Sauvegarde l'analyse
void sauvegarderAnalyseTendances(const char *filename, const char *contenu) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    fprintf(file, "%s", contenu);
    fclose(file);
}
