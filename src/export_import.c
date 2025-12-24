#include "export_import.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Export en CSV
void exporterTransactionsCSV(const char *filename, Transaction *tete) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    // Écrire les en-têtes
    fprintf(file, "ID,Date,Description,Montant\n");

    // Écrire les données
    while (tete != NULL) {
        fprintf(file, "%d,%s,\"%s\",%.2f\n",
                tete->id,
                tete->date,
                tete->description,
                tete->montant);
        tete = tete->next;
    }

    fclose(file);
    printf("Transactions exportées en CSV: %s\n", filename);
}

// Export en JSON
void exporterTransactionsJSON(const char *filename, Transaction *tete) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    fprintf(file, "{\n  \"transactions\": [\n");

    int first = 1;
    while (tete != NULL) {
        if (!first) fprintf(file, ",\n");
        fprintf(file, "    {\n");
        fprintf(file, "      \"id\": %d,\n", tete->id);
        fprintf(file, "      \"date\": \"%s\",\n", tete->date);
        fprintf(file, "      \"description\": \"%s\",\n", tete->description);
        fprintf(file, "      \"montant\": %.2f\n", tete->montant);
        fprintf(file, "    }");
        first = 0;
        tete = tete->next;
    }

    fprintf(file, "\n  ]\n}\n");
    fclose(file);
    printf("Transactions exportées en JSON: %s\n", filename);
}

// Export en XML
void exporterTransactionsXML(const char *filename, Transaction *tete) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(file, "<transactions>\n");

    while (tete != NULL) {
        fprintf(file, "  <transaction>\n");
        fprintf(file, "    <id>%d</id>\n", tete->id);
        fprintf(file, "    <date>%s</date>\n", tete->date);
        fprintf(file, "    <description>%s</description>\n", tete->description);
        fprintf(file, "    <montant>%.2f</montant>\n", tete->montant);
        fprintf(file, "  </transaction>\n");
        tete = tete->next;
    }

    fprintf(file, "</transactions>\n");
    fclose(file);
    printf("Transactions exportées en XML: %s\n", filename);
}

// Export en TXT (format personnalisé)
void exporterTransactionsTXT(const char *filename, Transaction *tete) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    fprintf(file, "=== EXPORT DES TRANSACTIONS ===\n");
    time_t now = time(NULL);
    fprintf(file, "Date d'export: %s\n", ctime(&now));
    fprintf(file, "=====================================\n\n");

    int count = 0;
    float total = 0;

    while (tete != NULL) {
        fprintf(file, "Transaction #%d\n", ++count);
        fprintf(file, "  ID: %d\n", tete->id);
        fprintf(file, "  Date: %s\n", tete->date);
        fprintf(file, "  Description: %s\n", tete->description);
        fprintf(file, "  Montant: %.2f €\n\n", tete->montant);
        total += tete->montant;
        tete = tete->next;
    }

    fprintf(file, "=====================================\n");
    fprintf(file, "Total: %.2f €\n", total);
    fprintf(file, "Nombre de transactions: %d\n", count);
    fclose(file);
    printf("Transactions exportées en TXT: %s\n", filename);
}

// Import CSV
Transaction *importerTransactionsCSV(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return NULL;
    }

    Transaction *tete = NULL;
    Transaction *dernier = NULL;
    char ligne[256];

    // Ignorer la première ligne (en-têtes)
    fgets(ligne, sizeof(ligne), file);

    while (fgets(ligne, sizeof(ligne), file)) {
        Transaction *nouvelle = (Transaction *)malloc(sizeof(Transaction));
        if (nouvelle == NULL) break;

        char description_temp[MAX_DESC];
        int id;
        float montant;
        char date[MAX_DATE];

        // Parse CSV (format: ID,Date,"Description",Montant)
        int parsed = sscanf(ligne, "%d,%10[^,],\"%99[^\"]\"%*[,]%f",
                            &id, date, description_temp, &montant);

        if (parsed >= 3) {
            nouvelle->id = id;
            strncpy(nouvelle->date, date, MAX_DATE - 1);
            nouvelle->date[MAX_DATE - 1] = '\0';
            strncpy(nouvelle->description, description_temp, MAX_DESC - 1);
            nouvelle->description[MAX_DESC - 1] = '\0';
            nouvelle->montant = montant;
            nouvelle->next = NULL;

            if (tete == NULL) {
                tete = nouvelle;
                dernier = nouvelle;
            } else {
                dernier->next = nouvelle;
                dernier = nouvelle;
            }
        } else {
            free(nouvelle);
        }
    }

    fclose(file);
    printf("Transactions importées depuis CSV: %s\n", filename);
    return tete;
}

// Import JSON (simplifié)
Transaction *importerTransactionsJSON(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return NULL;
    }

    Transaction *tete = NULL;
    Transaction *dernier = NULL;
    char ligne[256];

    while (fgets(ligne, sizeof(ligne), file)) {
        // Chercher les lignes avec "id"
        if (strstr(ligne, "\"id\"") != NULL) {
            Transaction *nouvelle = (Transaction *)malloc(sizeof(Transaction));
            if (nouvelle == NULL) break;

            sscanf(ligne, " \"id\": %d", &nouvelle->id);

            // Lire les prochaines lignes
            fgets(ligne, sizeof(ligne), file);
            sscanf(ligne, " \"date\": \"%10[^\"]\"", nouvelle->date);

            fgets(ligne, sizeof(ligne), file);
            sscanf(ligne, " \"description\": \"%99[^\"]\"", nouvelle->description);

            fgets(ligne, sizeof(ligne), file);
            sscanf(ligne, " \"montant\": %f", &nouvelle->montant);

            nouvelle->next = NULL;

            if (tete == NULL) {
                tete = nouvelle;
                dernier = nouvelle;
            } else {
                dernier->next = nouvelle;
                dernier = nouvelle;
            }
        }
    }

    fclose(file);
    printf("Transactions importées depuis JSON: %s\n", filename);
    return tete;
}

// Import XML (simplifié)
Transaction *importerTransactionsXML(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return NULL;
    }

    Transaction *tete = NULL;
    Transaction *dernier = NULL;
    char ligne[256];

    while (fgets(ligne, sizeof(ligne), file)) {
        // Chercher les balises <transaction>
        if (strstr(ligne, "<id>") != NULL) {
            Transaction *nouvelle = (Transaction *)malloc(sizeof(Transaction));
            if (nouvelle == NULL) break;

            sscanf(ligne, "    <id>%d</id>", &nouvelle->id);

            fgets(ligne, sizeof(ligne), file);
            sscanf(ligne, "    <date>%10[^<]", nouvelle->date);

            fgets(ligne, sizeof(ligne), file);
            sscanf(ligne, "    <description>%99[^<]", nouvelle->description);

            fgets(ligne, sizeof(ligne), file);
            sscanf(ligne, "    <montant>%f", &nouvelle->montant);

            nouvelle->next = NULL;

            if (tete == NULL) {
                tete = nouvelle;
                dernier = nouvelle;
            } else {
                dernier->next = nouvelle;
                dernier = nouvelle;
            }
        }
    }

    fclose(file);
    printf("Transactions importées depuis XML: %s\n", filename);
    return tete;
}

// Import TXT
Transaction *importerTransactionsTXT(const char *filename) {
    // Utilise le même format que le chargement standard
    return NULL; // À implémenter
}

// Export générique
void exporterTransactions(const char *filename, Transaction *tete, FormatExport format) {
    switch (format) {
        case FORMAT_CSV:
            exporterTransactionsCSV(filename, tete);
            break;
        case FORMAT_JSON:
            exporterTransactionsJSON(filename, tete);
            break;
        case FORMAT_XML:
            exporterTransactionsXML(filename, tete);
            break;
        case FORMAT_TXT:
            exporterTransactionsTXT(filename, tete);
            break;
        default:
            printf("Format non supporté.\n");
    }
}

// Import générique
Transaction *importerTransactions(const char *filename, FormatExport format) {
    switch (format) {
        case FORMAT_CSV:
            return importerTransactionsCSV(filename);
        case FORMAT_JSON:
            return importerTransactionsJSON(filename);
        case FORMAT_XML:
            return importerTransactionsXML(filename);
        case FORMAT_TXT:
            return importerTransactionsTXT(filename);
        default:
            printf("Format non supporté.\n");
            return NULL;
    }
}

// Obtient un rapport d'export
RapportExport obtenirRapportExport(Transaction *tete) {
    RapportExport rapport = {0, 0.0, {0}};

    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    strftime(rapport.date_export, 11, "%Y-%m-%d", timeinfo);

    while (tete != NULL) {
        rapport.nombre_lignes++;
        rapport.total_montants += tete->montant;
        tete = tete->next;
    }

    return rapport;
}

// Affiche le rapport d'export
void afficherRapportExport(RapportExport rapport) {
    printf("\n=== RAPPORT D'EXPORT ===\n");
    printf("Date d'export:       %s\n", rapport.date_export);
    printf("Nombre de lignes:    %d\n", rapport.nombre_lignes);
    printf("Total montants:      %.2f €\n", rapport.total_montants);
}

// Valide un fichier CSV
int validerCSV(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) return 0;

    char ligne[256];
    int first = 1;

    while (fgets(ligne, sizeof(ligne), file)) {
        if (first) {
            // Vérifier les en-têtes
            if (strstr(ligne, "ID") == NULL || strstr(ligne, "Date") == NULL) {
                fclose(file);
                return 0;
            }
            first = 0;
        }
    }

    fclose(file);
    return 1;
}

// Valide un fichier JSON
int validerJSON(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) return 0;

    char ligne[256];
    int found_transactions = 0;

    while (fgets(ligne, sizeof(ligne), file)) {
        if (strstr(ligne, "\"transactions\"") != NULL) {
            found_transactions = 1;
            break;
        }
    }

    fclose(file);
    return found_transactions;
}
