#ifndef DEPENSES_H
#define DEPENSES_H

#include "database.h"

// Catégories de dépenses
typedef enum {
    NOURRITURE,
    LOGEMENT,
    TRANSPORT,
    LOISIRS,
    SANTE,
    EDUCATION,
    AUTRE
} CategorieDepense;

// Structure étendue pour les dépenses
typedef struct {
    Transaction transaction; // Structure de base
    CategorieDepense categorie;
} Depense;

// Prototypes des fonctions
Depense *creerDepense(const char *date, const char *description, float montant, CategorieDepense categorie);
void ajouterDepense(Transaction **tete, Depense *nouvelleDepense);
void afficherDepenses(Transaction *tete);
float totalDepenses(Transaction *tete);
void afficherDepensesParCategorie(Transaction *tete, CategorieDepense categorie);
void libererListeDepenses(Transaction *tete);

#endif