#include "revenus.h"
#include <stdio.h>

// Crée un nouveau revenu
Revenu *creerRevenu(const char *date, const char *description, float montant, CategorieRevenu categorie) {
    if (!isDateValid(date)) {
        printf("Date invalide. Format attendu: YYYY-MM-DD\n");
        return NULL;
    }

    Revenu *nouveauRevenu = (Revenu *)malloc(sizeof(Revenu));
    if (nouveauRevenu == NULL) {
        perror("Erreur d'allocation mémoire");
        return NULL;
    }

    nouveauRevenu->transaction.id = genererIdUnique();
    strncpy(nouveauRevenu->transaction.date, date, MAX_DATE);
    strncpy(nouveauRevenu->transaction.description, description, MAX_DESC);
    nouveauRevenu->transaction.montant = montant;
    nouveauRevenu->transaction.next = NULL;
    nouveauRevenu->categorie = categorie;

    return nouveauRevenu;
}

// Ajoute un revenu à la liste
void ajouterRevenu(Transaction **tete, Revenu *nouveauRevenu) {
    if (nouveauRevenu == NULL) return;

    // Conversion de Revenu* en Transaction*
    Transaction *nouvelleTransaction = (Transaction *)nouveauRevenu;

    nouvelleTransaction->next = *tete;
    *tete = nouvelleTransaction;
}

// Affiche tous les revenus
void afficherRevenus(Transaction *tete) {
    if (tete == NULL) {
        printf("Aucun revenu enregistré.\n");
        return;
    }

    printf("=== Liste des revenus ===\n");
    printf("ID\tDate\t\tDescription\t\tMontant\tCatégorie\n");
    printf("------------------------------------------------------------\n");

    Transaction *current = tete;
    while (current != NULL) {
        Revenu *revenu = (Revenu *)current;
        const char *categorieStr;
        
        switch (revenu->categorie) {
            case SALAIRE: categorieStr = "Salaire"; break;
            case BUSINESS: categorieStr = "Business"; break;
            case INVESTISSEMENT: categorieStr = "Investissement"; break;
            case CADEAU: categorieStr = "Cadeau"; break;
            default: categorieStr = "Autre";
        }

        printf("%d\t%s\t%-20s\t%.2f\t%s\n", 
               current->id, 
               current->date, 
               current->description, 
               current->montant,
               categorieStr);
        current = current->next;
    }
}

// Calcule le total des revenus
float totalRevenus(Transaction *tete) {
    float total = 0.0;
    Transaction *current = tete;

    while (current != NULL) {
        total += current->montant;
        current = current->next;
    }

    return total;
}

// Affiche les revenus par catégorie
void afficherRevenusParCategorie(Transaction *tete, CategorieRevenu categorie) {
    if (tete == NULL) {
        printf("Aucun revenu dans cette catégorie.\n");
        return;
    }

    printf("=== Revenus pour la catégorie: ");
    switch (categorie) {
        case SALAIRE: printf("Salaire"); break;
        case BUSINESS: printf("Business"); break;
        case INVESTISSEMENT: printf("Investissement"); break;
        case CADEAU: printf("Cadeau"); break;
        default: printf("Autre");
    }
    printf(" ===\n");

    printf("ID\tDate\t\tDescription\t\tMontant\n");
    printf("--------------------------------------------\n");

    Transaction *current = tete;
    int trouve = 0;

    while (current != NULL) {
        Revenu *revenu = (Revenu *)current;
        if (revenu->categorie == categorie) {
            printf("%d\t%s\t%-20s\t%.2f\n", 
                   current->id, 
                   current->date, 
                   current->description, 
                   current->montant);
            trouve = 1;
        }
        current = current->next;
    }

    if (!trouve) {
        printf("Aucun revenu trouvé dans cette catégorie.\n");
    }
}

// Libère la mémoire de la liste des revenus
void libererListeRevenus(Transaction *tete) {
    Transaction *current = tete;
    while (current != NULL) {
        Transaction *temp = current;
        current = current->next;
        free(temp);
    }
}