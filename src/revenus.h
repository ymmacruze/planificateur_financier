#ifndef REVENUS_H
#define REVENUS_H

#include "database.h"

// Catégories de revenus
typedef enum {
    SALAIRE,
    BUSINESS,
    INVESTISSEMENT,
    CADEAU,
    AUTRE_REVENU
} CategorieRevenu;

// Structure étendue pour les revenus
typedef struct {
    Transaction transaction; // Structure de base
    CategorieRevenu categorie;
} Revenu;

// Prototypes des fonctions
Revenu *creerRevenu(const char *date, const char *description, float montant, CategorieRevenu categorie);
void ajouterRevenu(Transaction **tete, Revenu *nouveauRevenu);
void afficherRevenus(Transaction *tete);
float totalRevenus(Transaction *tete);
void afficherRevenusParCategorie(Transaction *tete, CategorieRevenu categorie);
void libererListeRevenus(Transaction *tete);

#endif