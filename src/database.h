#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_DESC 100
#define MAX_DATE 11 // format: YYYY-MM-DD

// Structure pour une transaction (base commune pour dépenses et revenus)
typedef struct Transaction {
    int id;
    char date[MAX_DATE];
    char description[MAX_DESC];
    float montant;
    struct Transaction *next;
} Transaction;

// Structure pour l'épargne
typedef struct Epargne {
    int id;
    char date[MAX_DATE];
    char description[MAX_DESC];
    float montant;
    float taux_interet;
    int duree_jours;
    struct Epargne *next;
} Epargne;

// Prototypes des fonctions
void sauvegarderTransactions(const char *filename, Transaction *tete);
Transaction *chargerTransactions(const char *filename);
void sauvegarderEpargnes(const char *filename, Epargne *tete);
Epargne *chargerEpargnes(const char *filename);
int genererIdUnique();
int isDateValid(const char *date);

#endif