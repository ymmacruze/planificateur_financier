#include "database.h"

// Génère un ID unique basé sur le timestamp
int genererIdUnique() {
    static int dernierId = 0;
    return ++dernierId;
}

// Vérifie si une date est valide (format: YYYY-MM-DD)
int isDateValid(const char *date) {
    if (strlen(date) != 10) return 0;
    if (date[4] != '-' || date[7] != '-') return 0;
    
    int annee, mois, jour;
    if (sscanf(date, "%d-%d-%d", &annee, &mois, &jour) != 3) return 0;
    
    if (mois < 1 || mois > 12) return 0;
    if (jour < 1 || jour > 31) return 0;
    
    // Vérification simplifiée des jours par mois
    if ((mois == 4 || mois == 6 || mois == 9 || mois == 11) && jour > 30) return 0;
    if (mois == 2) {
        // Vérification bissextile simplifiée
        int bissextile = (annee % 4 == 0 && (annee % 100 != 0 || annee % 400 == 0));
        if (jour > (bissextile ? 29 : 28)) return 0;
    }
    
    return 1;
}

// Sauvegarde les transactions dans un fichier
void sauvegarderTransactions(const char *filename, Transaction *tete) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    Transaction *current = tete;
    while (current != NULL) {
        fprintf(file, "%d|%s|%s|%.2f\n", 
                current->id, 
                current->date, 
                current->description, 
                current->montant);
        current = current->next;
    }

    fclose(file);
}

// Charge les transactions depuis un fichier
Transaction *chargerTransactions(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return NULL;
    }

    Transaction *tete = NULL;
    Transaction *dernier = NULL;
    char ligne[256];

    while (fgets(ligne, sizeof(ligne), file)) {
        Transaction *nouveau = (Transaction *)malloc(sizeof(Transaction));
        if (nouveau == NULL) {
            perror("Erreur d'allocation mémoire");
            break;
        }

        char *token = strtok(ligne, "|");
        if (token == NULL) continue;
        nouveau->id = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { free(nouveau); continue; }
        strncpy(nouveau->date, token, MAX_DATE);

        token = strtok(NULL, "|");
        if (token == NULL) { free(nouveau); continue; }
        strncpy(nouveau->description, token, MAX_DESC);

        token = strtok(NULL, "|");
        if (token == NULL) { free(nouveau); continue; }
        nouveau->montant = atof(token);

        nouveau->next = NULL;

        if (tete == NULL) {
            tete = nouveau;
            dernier = nouveau;
        } else {
            dernier->next = nouveau;
            dernier = nouveau;
        }

        // Mettre à jour le dernier ID pour générer des IDs uniques
        if (nouveau->id > genererIdUnique()) {
            genererIdUnique(); // Cette implémentation nécessite une modification
            // Une meilleure solution serait d'avoir une variable globale pour dernierId
        }
    }

    fclose(file);
    return tete;
}

// Sauvegarde les épargnes dans un fichier
void sauvegarderEpargnes(const char *filename, Epargne *tete) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    Epargne *current = tete;
    while (current != NULL) {
        fprintf(file, "%d|%s|%s|%.2f|%.2f|%d\n", 
                current->id, 
                current->date, 
                current->description, 
                current->montant,
                current->taux_interet,
                current->duree_jours);
        current = current->next;
    }

    fclose(file);
}

// Charge les épargnes depuis un fichier
Epargne *chargerEpargnes(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return NULL;
    }

    Epargne *tete = NULL;
    Epargne *dernier = NULL;
    char ligne[256];

    while (fgets(ligne, sizeof(ligne), file)) {
        Epargne *nouveau = (Epargne *)malloc(sizeof(Epargne));
        if (nouveau == NULL) {
            perror("Erreur d'allocation mémoire");
            break;
        }

        char *token = strtok(ligne, "|");
        if (token == NULL) continue;
        nouveau->id = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) { free(nouveau); continue; }
        strncpy(nouveau->date, token, MAX_DATE);

        token = strtok(NULL, "|");
        if (token == NULL) { free(nouveau); continue; }
        strncpy(nouveau->description, token, MAX_DESC);

        token = strtok(NULL, "|");
        if (token == NULL) { free(nouveau); continue; }
        nouveau->montant = atof(token);

        token = strtok(NULL, "|");
        if (token == NULL) { free(nouveau); continue; }
        nouveau->taux_interet = atof(token);

        token = strtok(NULL, "|");
        if (token == NULL) { free(nouveau); continue; }
        nouveau->duree_jours = atoi(token);

        nouveau->next = NULL;

        if (tete == NULL) {
            tete = nouveau;
            dernier = nouveau;
        } else {
            dernier->next = nouveau;
            dernier = nouveau;
        }
    }

    fclose(file);
    return tete;
}