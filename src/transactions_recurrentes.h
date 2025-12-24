#ifndef TRANSACTIONS_RECURRENTES_H
#define TRANSACTIONS_RECURRENTES_H

#include "database.h"
#include "multi_devises.h"
#include <time.h>

#define MAX_RECURRENCE_DESC 100

// Types de fréquences de récurrence
typedef enum {
    QUOTIDIENNE,
    HEBDOMADAIRE,
    BIHEBDOMADAIRE,
    MENSUELLE,
    TRIMESTRIELLE,
    SEMESTRIELLE,
    ANNUELLE
} FrequenceRecurrence;

// Catégories étendues pour les récurrences
typedef enum {
    REC_NOURRITURE,
    REC_LOGEMENT,
    REC_TRANSPORT,
    REC_LOISIRS,
    REC_SANTE,
    REC_EDUCATION,
    REC_SALAIRE,
    REC_AUTRE
} CategorieRecurrence;

// Structure pour une transaction récurrente
typedef struct TransactionRecurrente {
    int id;
    char description[MAX_RECURRENCE_DESC];
    float montant;
    CodeDevise devise;
    FrequenceRecurrence frequence;
    CategorieRecurrence categorie;
    char date_debut[11]; // YYYY-MM-DD
    char date_fin[11];   // YYYY-MM-DD (ou "2099-12-31" si pas de fin)
    int active;          // 1 = active, 0 = inactive
    struct TransactionRecurrente *next;
} TransactionRecurrente;

// Prototypes des fonctions
TransactionRecurrente *creerTransactionRecurrente(
    const char *description,
    float montant,
    CodeDevise devise,
    FrequenceRecurrence frequence,
    CategorieRecurrence categorie,
    const char *date_debut,
    const char *date_fin
);

void ajouterTransactionRecurrente(TransactionRecurrente **tete, TransactionRecurrente *nouvelle);
void afficherTransactionsRecurrentes(TransactionRecurrente *tete);
void supprimerTransactionRecurrente(TransactionRecurrente **tete, int id);
void desactiverTransactionRecurrente(TransactionRecurrente *tete, int id);
void activerTransactionRecurrente(TransactionRecurrente *tete, int id);

// Génère les transactions à partir des récurrences
void genererTransactionsRecurrentes(
    TransactionRecurrente *recurrences,
    Transaction **transactions_sortie,
    const char *date_debut,
    const char *date_fin
);

void sauvegarderTransactionsRecurrentes(const char *filename, TransactionRecurrente *tete);
TransactionRecurrente *chargerTransactionsRecurrentes(const char *filename);
void libererListeTransactionsRecurrentes(TransactionRecurrente *tete);

// Fonctions utilitaires
const char *getFrequenceString(FrequenceRecurrence freq);
const char *getCategorieRecurrenceString(CategorieRecurrence cat);
FrequenceRecurrence getFrequenceFromInt(int i);
CategorieRecurrence getCategorieRecurrenceFromInt(int i);

#endif
