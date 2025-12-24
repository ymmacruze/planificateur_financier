#ifndef MULTI_DEVISES_H
#define MULTI_DEVISES_H

#include <string.h>

#define MAX_DEVISE_CODE 4 // EUR, USD, GBP, JPY, etc.

// Types de devises supportées
typedef enum {
    EUR,    // Euro
    USD,    // Dollar américain
    GBP,    // Livre sterling
    JPY,    // Yen japonais
    CHF,    // Franc suisse
    CAD,    // Dollar canadien
    AUD,    // Dollar australien
    CNY     // Yuan chinois
} CodeDevise;

// Structure pour les taux de change
typedef struct {
    CodeDevise from;
    CodeDevise to;
    float taux;
} TauxChange;

// Structure pour une transaction avec devise
typedef struct TransactionDevise {
    int id;
    char date[11];
    char description[100];
    float montant;
    CodeDevise devise;
    struct TransactionDevise *next;
} TransactionDevise;

// Prototypes des fonctions
const char *getCodeDevise(CodeDevise devise);
CodeDevise getDeviseFromCode(const char *code);
float convertirDevise(float montant, CodeDevise from, CodeDevise to);
void definirTauxChange(CodeDevise from, CodeDevise to, float taux);
void afficherDevises();
float getTauxChange(CodeDevise from, CodeDevise to);

// Taux de change par défaut (base EUR = 1.0)
extern TauxChange tauxChanges[8];
extern int nbTauxChanges;

#endif
