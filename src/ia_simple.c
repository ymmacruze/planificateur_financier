#include "ia_simple.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// Convertit une chaîne en minuscules
void toLowerStr(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

// Catégorisation automatique basée sur mots-clés
char *categoriserAutomatique(const char *description) {
    if (description == NULL) return "Autre";

    char desc_lower[256];
    strncpy(desc_lower, description, 255);
    desc_lower[255] = '\0';
    toLowerStr(desc_lower);

    if (strstr(desc_lower, "restaurant") || strstr(desc_lower, "epicerie") || 
        strstr(desc_lower, "cafe") || strstr(desc_lower, "manger")) {
        return "Nourriture";
    } else if (strstr(desc_lower, "loyer") || strstr(desc_lower, "rent") || 
               strstr(desc_lower, "house") || strstr(desc_lower, "appartement")) {
        return "Logement";
    } else if (strstr(desc_lower, "essence") || strstr(desc_lower, "voiture") || 
               strstr(desc_lower, "transport") || strstr(desc_lower, "bus")) {
        return "Transport";
    } else if (strstr(desc_lower, "cinema") || strstr(desc_lower, "jeu") || 
               strstr(desc_lower, "loisir") || strstr(desc_lower, "divertissement")) {
        return "Loisirs";
    } else if (strstr(desc_lower, "doctor") || strstr(desc_lower, "sante") || 
               strstr(desc_lower, "medecin") || strstr(desc_lower, "pharmacie")) {
        return "Santé";
    } else if (strstr(desc_lower, "ecole") || strstr(desc_lower, "formation") || 
               strstr(desc_lower, "cours") || strstr(desc_lower, "education")) {
        return "Éducation";
    }
    
    return "Autre";
}

// Calcule la confiance de la catégorisation
float calculateur_confiance_categorisation(const char *description) {
    if (description == NULL) return 0.3;

    // Plus la description est longue et détaillée, plus la confiance est élevée
    int len = strlen(description);
    float confiance = 0.5 + (len > 30 ? 0.3 : len * 0.01);
    
    if (confiance > 0.95) confiance = 0.95;
    return confiance;
}

// Détecte les anomalies avancées
AnomalieIA *detecterAnomaliesAvancees(Transaction *tete) {
    AnomalieIA *anomalies = (AnomalieIA *)malloc(100 * sizeof(AnomalieIA));
    if (anomalies == NULL) return NULL;

    int count = 0;

    // Calculer la moyenne et l'écart type
    float total = 0, total_carre = 0;
    int nb = 0;
    Transaction *current = tete;

    while (current != NULL) {
        total += current->montant;
        total_carre += current->montant * current->montant;
        nb++;
        current = current->next;
    }

    if (nb == 0) {
        free(anomalies);
        return NULL;
    }

    float moyenne = total / nb;
    float variance = (total_carre / nb) - (moyenne * moyenne);
    float ecart_type = sqrt(variance);

    // Déterminer les seuils
    float seuil_haut = moyenne + 2 * ecart_type;

    // Parcourir et détecter les anomalies
    current = tete;
    while (current != NULL && count < 100) {
        // Anomalie 1: Dépense anormalement haute
        if (current->montant > seuil_haut) {
            anomalies[count].type = ANOMALIE_DEPENSE_HAUTE;
            anomalies[count].id_transaction = current->id;
            snprintf(anomalies[count].description, 256,
                     "Dépense inhabituelle: %.2f € (moyenne: %.2f €)",
                     current->montant, moyenne);
            anomalies[count].confiance = 0.85;
            count++;
        }

        current = current->next;
    }

    // Ajouter un sentinel
    if (count < 100) {
        anomalies[count].type = -1;
    }

    return anomalies;
}

// Affiche les anomalies
void afficherAnomalies(AnomalieIA *anomalies) {
    if (anomalies == NULL) return;

    printf("\n=== ANOMALIES DÉTECTÉES PAR L'IA ===\n");

    int count = 0;
    while (anomalies[count].type != (TypeAnomalieIA)(-1)) {
        printf("%d. %s\n", count + 1, anomalies[count].description);
        printf("   Confiance: %.0f%%\n", anomalies[count].confiance * 100);
        count++;
    }

    if (count == 0) {
        printf("Aucune anomalie détectée.\n");
    }
}

// Vérifie si deux transactions sont un double paiement
int estDoublePaiement(Transaction *t1, Transaction *t2) {
    if (t1 == NULL || t2 == NULL) return 0;

    // Même montant, même description, dates proches
    if (fabs(t1->montant - t2->montant) < 0.01 &&
        strcmp(t1->description, t2->description) == 0) {
        return 1;
    }
    return 0;
}

// Suggère une réduction de dépenses
Suggestion *suggererReductionDepenses(Transaction *depenses) {
    Suggestion *sugg = (Suggestion *)malloc(sizeof(Suggestion));
    if (sugg == NULL) return NULL;

    sugg->type = SUGGESTION_REDUCTION_DEPENSES;
    strcpy(sugg->titre, "Réduction des dépenses");

    float total = 0;
    int count = 0;
    Transaction *current = depenses;

    while (current != NULL) {
        total += current->montant;
        count++;
        current = current->next;
    }

    float moyenne = (count > 0) ? (total / count) : 0;
    sugg->gain_potentiel = moyenne * 0.1; // Gain potentiel: 10% de la moyenne
    
    snprintf(sugg->description, 256,
             "En réduisant vos dépenses de 10%%, vous économiseriez %.2f €",
             sugg->gain_potentiel);
    
    sugg->priorite = 3;
    return sugg;
}

// Suggère une augmentation de l'épargne
Suggestion *suggererAugmentationEpargne(Transaction *revenus, Transaction *depenses) {
    Suggestion *sugg = (Suggestion *)malloc(sizeof(Suggestion));
    if (sugg == NULL) return NULL;

    sugg->type = SUGGESTION_AUGMENTATION_EPARGNE;
    strcpy(sugg->titre, "Augmenter l'épargne");

    float total_rev = 0, total_dep = 0;
    Transaction *current = revenus;

    while (current != NULL) {
        total_rev += current->montant;
        current = current->next;
    }

    current = depenses;
    while (current != NULL) {
        total_dep += current->montant;
        current = current->next;
    }

    sugg->gain_potentiel = total_rev - total_dep;
    
    snprintf(sugg->description, 256,
             "Vous pouvez épargner jusqu'à %.2f € avec votre solde actuel",
             sugg->gain_potentiel);
    
    sugg->priorite = 4;
    return sugg;
}

// Suggère un budget personnalisé
Suggestion *suggererBudgetPersonnalise(Transaction *depenses) {
    Suggestion *sugg = (Suggestion *)malloc(sizeof(Suggestion));
    if (sugg == NULL) return NULL;

    sugg->type = SUGGESTION_BUDGET;
    strcpy(sugg->titre, "Budget personnalisé recommandé");

    float total = 0;
    int count = 0;
    Transaction *current = depenses;

    while (current != NULL) {
        total += current->montant;
        count++;
        current = current->next;
    }

    sugg->gain_potentiel = (count > 0) ? (total / count) : 0;
    
    snprintf(sugg->description, 256,
             "Budget recommandé par mois: %.2f €",
             sugg->gain_potentiel);
    
    sugg->priorite = 3;
    return sugg;
}

// Détecte les suggestions personnalisées
Suggestion *detecterSuggestionsPersonnalisees(Transaction *revenus, Transaction *depenses) {
    Suggestion *suggestions = (Suggestion *)malloc(5 * sizeof(Suggestion));
    if (suggestions == NULL) return NULL;

    int index = 0;

    // Suggestion 1: Réduction dépenses
    if (depenses != NULL) {
        Suggestion *s1 = suggererReductionDepenses(depenses);
        if (s1 != NULL) {
            suggestions[index++] = *s1;
            free(s1);
        }
    }

    // Suggestion 2: Augmentation épargne
    if (revenus != NULL && depenses != NULL) {
        Suggestion *s2 = suggererAugmentationEpargne(revenus, depenses);
        if (s2 != NULL) {
            suggestions[index++] = *s2;
            free(s2);
        }
    }

    // Suggestion 3: Budget
    if (depenses != NULL) {
        Suggestion *s3 = suggererBudgetPersonnalise(depenses);
        if (s3 != NULL) {
            suggestions[index++] = *s3;
            free(s3);
        }
    }

    return suggestions;
}

// Affiche les recommandations financières
void afficherRecommandationsFinancieres(Transaction *revenus, Transaction *depenses) {
    printf("\n=== RECOMMANDATIONS PERSONNALISÉES DE L'IA ===\n");

    Suggestion *suggs = detecterSuggestionsPersonnalisees(revenus, depenses);
    if (suggs == NULL) return;

    for (int i = 0; i < 3; i++) {
        if (suggs[i].type >= 0) {
            printf("%d. %s\n", i + 1, suggs[i].titre);
            printf("   %s\n", suggs[i].description);
            printf("   Priorité: %d/5\n", suggs[i].priorite);
        }
    }

    free(suggs);
}

// Génère des conseils
char *genererConseils(float taux_epargne, float taux_depenses __attribute__((unused))) {
    static char conseil[256];

    if (taux_epargne > 0.3) {
        strcpy(conseil, "Excellent! Vous avez un bon taux d'épargne.");
    } else if (taux_epargne > 0.2) {
        strcpy(conseil, "Bon taux d'épargne. Essayez d'atteindre 30%.");
    } else if (taux_epargne > 0.1) {
        strcpy(conseil, "Augmentez votre épargne à au moins 20% de vos revenus.");
    } else {
        strcpy(conseil, "⚠️ Très faible taux d'épargne. Action urgente recommandée!");
    }

    return conseil;
}

// Calcule le score financier
ScoreFinancier calculerScoreFinancier(Transaction *revenus, Transaction *depenses) {
    ScoreFinancier score = {50, 50, 3};

    float total_rev = 0, total_dep = 0;
    Transaction *current = revenus;

    while (current != NULL) {
        total_rev += current->montant;
        current = current->next;
    }

    current = depenses;
    while (current != NULL) {
        total_dep += current->montant;
        current = current->next;
    }

    if (total_rev == 0) return score;

    float taux_epargne = (total_rev - total_dep) / total_rev;
    float taux_depenses = total_dep / total_rev;

    // Score de santé financière
    if (taux_epargne > 0.3) score.score_sante_financiere = 85;
    else if (taux_epargne > 0.2) score.score_sante_financiere = 75;
    else if (taux_epargne > 0.1) score.score_sante_financiere = 60;
    else score.score_sante_financiere = 40;

    // Score d'équilibre
    if (taux_depenses > 1.0) score.score_equilibre = 20;
    else if (taux_depenses > 0.9) score.score_equilibre = 40;
    else if (taux_depenses > 0.7) score.score_equilibre = 70;
    else score.score_equilibre = 90;

    // Niveau de risque
    if (taux_depenses > 1.0) score.niveau_risque = 5; // Très risqué
    else if (taux_depenses > 0.9) score.niveau_risque = 4;
    else if (taux_depenses > 0.7) score.niveau_risque = 3;
    else score.niveau_risque = 1;

    return score;
}

// Affiche le score financier
void afficherScoreFinancier(ScoreFinancier score) {
    printf("\n=== SCORE FINANCIER PERSONNEL ===\n");
    printf("Santé financière:    %.0f/100 ", score.score_sante_financiere);
    
    if (score.score_sante_financiere >= 80) printf("✓ Excellent\n");
    else if (score.score_sante_financiere >= 60) printf("→ Bon\n");
    else if (score.score_sante_financiere >= 40) printf("⚠ À améliorer\n");
    else printf("✗ Critique\n");

    printf("Équilibre financier: %.0f/100 ", score.score_equilibre);
    if (score.score_equilibre >= 80) printf("✓ Excellent\n");
    else if (score.score_equilibre >= 60) printf("→ Bon\n");
    else printf("⚠ À améliorer\n");

    printf("Niveau de risque:    %d/5 ", score.niveau_risque);
    if (score.niveau_risque <= 2) printf("(Faible)\n");
    else if (score.niveau_risque <= 3) printf("(Moyen)\n");
    else printf("(Élevé)\n");
}

// Prédit le comportement de dépenses
void predireComportementDepenses(Transaction *depenses) {
    printf("\n=== PRÉDICTION DU COMPORTEMENT ===\n");

    float total = 0;
    int count = 0;
    Transaction *current = depenses;

    while (current != NULL) {
        total += current->montant;
        count++;
        current = current->next;
    }

    if (count == 0) {
        printf("Aucune donnée pour prédire.\n");
        return;
    }

    float moyenne = total / count;
    printf("Dépense moyenne prédite: %.2f €\n", moyenne);
    printf("Tendance: ");

    if (count > 10) {
        float derniere_moitie = 0;
        int count2 = 0;
        current = depenses;
        int skip = count / 2;
        while (current != NULL) {
            if (skip-- <= 0) {
                derniere_moitie += current->montant;
                count2++;
            }
            current = current->next;
        }

        float moyenne_recente = count2 > 0 ? (derniere_moitie / count2) : moyenne;
        if (moyenne_recente > moyenne * 1.1) {
            printf("↑ À la hausse\n");
        } else if (moyenne_recente < moyenne * 0.9) {
            printf("↓ À la baisse\n");
        } else {
            printf("→ Stable\n");
        }
    }
}

// Vérifie si on serait endetté
int seriez_vous_endette(Transaction *revenus, Transaction *depenses, int mois) {
    float total_rev = 0, total_dep = 0;
    Transaction *current = revenus;

    while (current != NULL) {
        total_rev += current->montant;
        current = current->next;
    }

    current = depenses;
    while (current != NULL) {
        total_dep += current->montant;
        current = current->next;
    }

    float solde_mensuel = total_rev - total_dep;
    return (solde_mensuel * mois) < 0;
}
