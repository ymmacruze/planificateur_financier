#ifndef EXPORT_IMPORT_H
#define EXPORT_IMPORT_H

#include "database.h"
#include "depenses.h"
#include "revenus.h"

// Formats supportés
typedef enum {
    FORMAT_TXT,
    FORMAT_CSV,
    FORMAT_JSON,
    FORMAT_XML
} FormatExport;

// Prototypes des fonctions d'export
void exporterTransactionsCSV(const char *filename, Transaction *tete);
void exporterTransactionsJSON(const char *filename, Transaction *tete);
void exporterTransactionsXML(const char *filename, Transaction *tete);
void exporterTransactionsTXT(const char *filename, Transaction *tete);

// Prototypes des fonctions d'import
Transaction *importerTransactionsCSV(const char *filename);
Transaction *importerTransactionsJSON(const char *filename);
Transaction *importerTransactionsXML(const char *filename);
Transaction *importerTransactionsTXT(const char *filename);

// Fonction générique
void exporterTransactions(const char *filename, Transaction *tete, FormatExport format);
Transaction *importerTransactions(const char *filename, FormatExport format);

// Rapport d'export
typedef struct {
    int nombre_lignes;
    float total_montants;
    char date_export[11];
} RapportExport;

RapportExport obtenirRapportExport(Transaction *tete);
void afficherRapportExport(RapportExport rapport);

// Validation
int validerCSV(const char *filename);
int validerJSON(const char *filename);

#endif
