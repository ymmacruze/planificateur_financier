#include "bilan_mensuel.h"
#include <stdio.h>
#include <string.h>

// Génère un bilan mensuel
void genererBilanMensuel(Transaction *revenus, Transaction *depenses, Epargne *epargnes, const char *mois) {
    printf("\n=== BILAN MENSUEL: %s ===\n", mois);
    
    float totalRevenus = 0.0;
    float totalDepenses = 0.0;
    float totalEpargnes = 0.0;
    
    // Calcul des totaux pour le mois spécifié
    Transaction *current = revenus;
    while (current != NULL) {
        char moisTransaction[8];
        strncpy(moisTransaction, current->date, 7); // Extraire YYYY-MM
        moisTransaction[7] = '\0';
        
        if (strcmp(moisTransaction, mois) == 0) {
            totalRevenus += current->montant;
        }
        current = current->next;
    }
    
    current = depenses;
    while (current != NULL) {
        char moisTransaction[8];
        strncpy(moisTransaction, current->date, 7);
        moisTransaction[7] = '\0';
        
        if (strcmp(moisTransaction, mois) == 0) {
            totalDepenses += current->montant;
        }
        current = current->next;
    }
    
    Epargne *currentEpargne = epargnes;
    while (currentEpargne != NULL) {
        char moisEpargne[8];
        strncpy(moisEpargne, currentEpargne->date, 7);
        moisEpargne[7] = '\0';
        
        if (strcmp(moisEpargne, mois) == 0) {
            totalEpargnes += currentEpargne->montant;
        }
        currentEpargne = currentEpargne->next;
    }
    
    // Affichage des résultats
    printf("Total Revenus: %.2f\n", totalRevenus);
    printf("Total Dépenses: %.2f\n", totalDepenses);
    printf("Total Épargnes: %.2f\n", totalEpargnes);
    printf("--------------------------------\n");
    printf("Solde: %.2f\n", totalRevenus - totalDepenses - totalEpargnes);
    printf("Taux d'épargne: %.1f%%\n", (totalRevenus > 0) ? (totalEpargnes / totalRevenus * 100) : 0.0);
    
    // Statistiques par catégorie
    printf("\n=== Statistiques par catégorie ===\n");
    afficherStatistiquesParCategorie(revenus, depenses, mois);
}

// Calcule le solde mensuel
float calculerSoldeMensuel(Transaction *revenus, Transaction *depenses, const char *mois) {
    float totalRevenus = 0.0;
    float totalDepenses = 0.0;
    
    Transaction *current = revenus;
    while (current != NULL) {
        char moisTransaction[8];
        strncpy(moisTransaction, current->date, 7);
        moisTransaction[7] = '\0';
        
        if (strcmp(moisTransaction, mois) == 0) {
            totalRevenus += current->montant;
        }
        current = current->next;
    }
    
    current = depenses;
    while (current != NULL) {
        char moisTransaction[8];
        strncpy(moisTransaction, current->date, 7);
        moisTransaction[7] = '\0';
        
        if (strcmp(moisTransaction, mois) == 0) {
            totalDepenses += current->montant;
        }
        current = current->next;
    }
    
    return totalRevenus - totalDepenses;
}

// Calcule l'épargne mensuelle
float calculerEpargneMensuelle(Epargne *epargnes, const char *mois) {
    float total = 0.0;
    
    Epargne *current = epargnes;
    while (current != NULL) {
        char moisEpargne[8];
        strncpy(moisEpargne, current->date, 7);
        moisEpargne[7] = '\0';
        
        if (strcmp(moisEpargne, mois) == 0) {
            total += current->montant;
        }
        current = current->next;
    }
    
    return total;
}

// Affiche les statistiques par catégorie
void afficherStatistiquesParCategorie(Transaction *revenus, Transaction *depenses, const char *mois) {
    // Tableaux pour stocker les totaux par catégorie
    float revenusParCategorie[5] = {0}; // 5 catégories de revenus
    float depensesParCategorie[7] = {0}; // 7 catégories de dépenses
    
    // Calcul des totaux pour les revenus
    Transaction *current = revenus;
    while (current != NULL) {
        char moisTransaction[8];
        strncpy(moisTransaction, current->date, 7);
        moisTransaction[7] = '\0';
        
        if (strcmp(moisTransaction, mois) == 0) {
            Revenu *revenu = (Revenu *)current;
            revenusParCategorie[revenu->categorie] += current->montant;
        }
        current = current->next;
    }
    
    // Calcul des totaux pour les dépenses
    current = depenses;
    while (current != NULL) {
        char moisTransaction[8];
        strncpy(moisTransaction, current->date, 7);
        moisTransaction[7] = '\0';
        
        if (strcmp(moisTransaction, mois) == 0) {
            Depense *depense = (Depense *)current;
            depensesParCategorie[depense->categorie] += current->montant;
        }
        current = current->next;
    }
    
    // Affichage des revenus par catégorie
    printf("\nRevenus par catégorie:\n");
    for (int i = 0; i < 5; i++) {
        if (revenusParCategorie[i] > 0) {
            const char *categorie;
            switch (i) {
                case SALAIRE: categorie = "Salaire"; break;
                case BUSINESS: categorie = "Business"; break;
                case INVESTISSEMENT: categorie = "Investissement"; break;
                case CADEAU: categorie = "Cadeau"; break;
                default: categorie = "Autre";
            }
            printf("- %s: %.2f\n", categorie, revenusParCategorie[i]);
        }
    }
    
    // Affichage des dépenses par catégorie
    printf("\nDépenses par catégorie:\n");
    for (int i = 0; i < 7; i++) {
        if (depensesParCategorie[i] > 0) {
            const char *categorie;
            switch (i) {
                case NOURRITURE: categorie = "Nourriture"; break;
                case LOGEMENT: categorie = "Logement"; break;
                case TRANSPORT: categorie = "Transport"; break;
                case LOISIRS: categorie = "Loisirs"; break;
                case SANTE: categorie = "Santé"; break;
                case EDUCATION: categorie = "Éducation"; break;
                default: categorie = "Autre";
            }
            printf("- %s: %.2f\n", categorie, depensesParCategorie[i]);
        }
    }
}