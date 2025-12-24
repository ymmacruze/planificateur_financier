#include "depenses.h"
#include <stdio.h>

// Crée une nouvelle dépense
Depense *creerDepense(const char *date, const char *description, float montant, CategorieDepense categorie) {
    if (!isDateValid(date)) {
        printf("Date invalide. Format attendu: YYYY-MM-DD\n");
        return NULL;
    }

    Depense *nouvelleDepense = (Depense *)malloc(sizeof(Depense));
    if (nouvelleDepense == NULL) {
        perror("Erreur d'allocation mémoire");
        return NULL;
    }

    nouvelleDepense->transaction.id = genererIdUnique();
    strncpy(nouvelleDepense->transaction.date, date, MAX_DATE);
    strncpy(nouvelleDepense->transaction.description, description, MAX_DESC);
    nouvelleDepense->transaction.montant = montant;
    nouvelleDepense->transaction.next = NULL;
    nouvelleDepense->categorie = categorie;

    return nouvelleDepense;
}

// Ajoute une dépense à la liste
void ajouterDepense(Transaction **tete, Depense *nouvelleDepense) {
    if (nouvelleDepense == NULL) return;

    // Conversion de Depense* en Transaction*
    Transaction *nouvelleTransaction = (Transaction *)nouvelleDepense;

    nouvelleTransaction->next = *tete;
    *tete = nouvelleTransaction;
}

// Affiche toutes les dépenses
void afficherDepenses(Transaction *tete) {
    if (tete == NULL) {
        printf("Aucune dépense enregistrée.\n");
        return;
    }

    printf("=== Liste des dépenses ===\n");
    printf("ID\tDate\t\tDescription\t\tMontant\tCatégorie\n");
    printf("------------------------------------------------------------\n");

    Transaction *current = tete;
    while (current != NULL) {
        Depense *depense = (Depense *)current;
        const char *categorieStr;
        
        switch (depense->categorie) {
            case NOURRITURE: categorieStr = "Nourriture"; break;
            case LOGEMENT: categorieStr = "Logement"; break;
            case TRANSPORT: categorieStr = "Transport"; break;
            case LOISIRS: categorieStr = "Loisirs"; break;
            case SANTE: categorieStr = "Santé"; break;
            case EDUCATION: categorieStr = "Éducation"; break;
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

// Calcule le total des dépenses
float totalDepenses(Transaction *tete) {
    float total = 0.0;
    Transaction *current = tete;

    while (current != NULL) {
        total += current->montant;
        current = current->next;
    }

    return total;
}

// Affiche les dépenses par catégorie
void afficherDepensesParCategorie(Transaction *tete, CategorieDepense categorie) {
    if (tete == NULL) {
        printf("Aucune dépense dans cette catégorie.\n");
        return;
    }

    printf("=== Dépenses pour la catégorie: ");
    switch (categorie) {
        case NOURRITURE: printf("Nourriture"); break;
        case LOGEMENT: printf("Logement"); break;
        case TRANSPORT: printf("Transport"); break;
        case LOISIRS: printf("Loisirs"); break;
        case SANTE: printf("Santé"); break;
        case EDUCATION: printf("Éducation"); break;
        default: printf("Autre");
    }
    printf(" ===\n");

    printf("ID\tDate\t\tDescription\t\tMontant\n");
    printf("--------------------------------------------\n");

    Transaction *current = tete;
    int trouve = 0;

    while (current != NULL) {
        Depense *depense = (Depense *)current;
        if (depense->categorie == categorie) {
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
        printf("Aucune dépense trouvée dans cette catégorie.\n");
    }
}

// Libère la mémoire de la liste des dépenses
void libererListeDepenses(Transaction *tete) {
    Transaction *current = tete;
    while (current != NULL) {
        Transaction *temp = current;
        current = current->next;
        free(temp);
    }
}