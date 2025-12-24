#include "epargnes.h"
#include <stdio.h>
#include <math.h>

// Crée une nouvelle épargne
EpargneEtendue *creerEpargne(const char *date, const char *description, float montant, 
                            float taux_interet, int duree_jours, TypeEpargne type) {
    if (!isDateValid(date)) {
        printf("Date invalide. Format attendu: YYYY-MM-DD\n");
        return NULL;
    }

    EpargneEtendue *nouvelleEpargne = (EpargneEtendue *)malloc(sizeof(EpargneEtendue));
    if (nouvelleEpargne == NULL) {
        perror("Erreur d'allocation mémoire");
        return NULL;
    }

    nouvelleEpargne->epargne.id = genererIdUnique();
    strncpy(nouvelleEpargne->epargne.date, date, MAX_DATE);
    strncpy(nouvelleEpargne->epargne.description, description, MAX_DESC);
    nouvelleEpargne->epargne.montant = montant;
    nouvelleEpargne->epargne.taux_interet = taux_interet;
    nouvelleEpargne->epargne.duree_jours = duree_jours;
    nouvelleEpargne->epargne.next = NULL;
    nouvelleEpargne->type = type;

    return nouvelleEpargne;
}

// Ajoute une épargne à la liste
void ajouterEpargne(Epargne **tete, EpargneEtendue *nouvelleEpargne) {
    if (nouvelleEpargne == NULL) return;

    // Conversion de EpargneEtendue* en Epargne*
    Epargne *nouveauEpargne = (Epargne *)nouvelleEpargne;

    nouveauEpargne->next = *tete;
    *tete = nouveauEpargne;
}

// Affiche toutes les épargnes
void afficherEpargnes(Epargne *tete) {
    if (tete == NULL) {
        printf("Aucune épargne enregistrée.\n");
        return;
    }

    printf("=== Liste des épargnes ===\n");
    printf("ID\tDate\t\tDescription\t\tMontant\tTaux\tDurée\tType\n");
    printf("----------------------------------------------------------------\n");

    Epargne *current = tete;
    while (current != NULL) {
        EpargneEtendue *epargne = (EpargneEtendue *)current;
        const char *typeStr;
        
        switch (epargne->type) {
            case COMPTE_EPARGNE: typeStr = "Compte épargne"; break;
            case LIVRET_A: typeStr = "Livret A"; break;
            case PLAN_EPARGNE_LOGEMENT: typeStr = "PEL"; break;
            case ASSURANCE_VIE: typeStr = "Assurance vie"; break;
            default: typeStr = "Autre";
        }

        printf("%d\t%s\t%-20s\t%.2f\t%.2f%%\t%dj\t%s\n", 
               current->id, 
               current->date, 
               current->description, 
               current->montant,
               current->taux_interet,
               current->duree_jours,
               typeStr);
        current = current->next;
    }
}

// Calcule le total des épargnes
float totalEpargnes(Epargne *tete) {
    float total = 0.0;
    Epargne *current = tete;

    while (current != NULL) {
        total += current->montant;
        current = current->next;
    }

    return total;
}

// Affiche les épargnes par type
void afficherEpargnesParType(Epargne *tete, TypeEpargne type) {
    if (tete == NULL) {
        printf("Aucune épargne de ce type.\n");
        return;
    }

    printf("=== Épargnes pour le type: ");
    switch (type) {
        case COMPTE_EPARGNE: printf("Compte épargne"); break;
        case LIVRET_A: printf("Livret A"); break;
        case PLAN_EPARGNE_LOGEMENT: printf("Plan Épargne Logement"); break;
        case ASSURANCE_VIE: printf("Assurance vie"); break;
        default: printf("Autre");
    }
    printf(" ===\n");

    printf("ID\tDate\t\tDescription\t\tMontant\tTaux\tDurée\n");
    printf("--------------------------------------------------------\n");

    Epargne *current = tete;
    int trouve = 0;

    while (current != NULL) {
        EpargneEtendue *epargne = (EpargneEtendue *)current;
        if (epargne->type == type) {
            printf("%d\t%s\t%-20s\t%.2f\t%.2f%%\t%dj\n", 
                   current->id, 
                   current->date, 
                   current->description, 
                   current->montant,
                   current->taux_interet,
                   current->duree_jours);
            trouve = 1;
        }
        current = current->next;
    }

    if (!trouve) {
        printf("Aucune épargne trouvée de ce type.\n");
    }
}

// Calcule les intérêts pour une épargne
float calculerInterets(Epargne *epargne) {
    if (epargne == NULL) return 0.0;
    
    // Calcul simple des intérêts (pour une approximation)
    // Dans la réalité, le calcul dépend du type de produit et de la périodicité des intérêts
    float interets = epargne->montant * (epargne->taux_interet / 100) * (epargne->duree_jours / 365.0);
    return interets;
}

// Libère la mémoire de la liste des épargnes
void libererListeEpargnes(Epargne *tete) {
    Epargne *current = tete;
    while (current != NULL) {
        Epargne *temp = current;
        current = current->next;
        free(temp);
    }
}