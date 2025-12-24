#ifndef RECHERCHE_FILTRAGE_H
#define RECHERCHE_FILTRAGE_H

#include "database.h"
#include "multi_devises.h"
#include <time.h>

// Structure pour les critères de recherche
typedef struct {
    char date_debut[11];      // Format YYYY-MM-DD (vide = pas de limite)
    char date_fin[11];        // Format YYYY-MM-DD (vide = pas de limite)
    float montant_min;        // 0 = pas de limite inférieure
    float montant_max;        // 0 = pas de limite supérieure (utiliser FLT_MAX)
    char description[100];    // Chaîne partielle (vide = pas de filtre)
    CodeDevise devise;        // -1 = pas de filtre
    int categorie;            // -1 = pas de filtre (pour future utilisation)
} CritereRecherche;

// Structure pour résumer les résultats
typedef struct {
    int nb_resultats;
    float montant_total;
    float montant_moyen;
    float montant_min;
    float montant_max;
} ResumeRecherche;

// Prototypes des fonctions de recherche
Transaction *rechercherTransactions(Transaction *tete, CritereRecherche criteres);
Transaction *rechercherParDate(Transaction *tete, const char *date_debut, const char *date_fin);
Transaction *rechercherParMontant(Transaction *tete, float montant_min, float montant_max);
Transaction *rechercherParDescription(Transaction *tete, const char *description);
Transaction *rechercherParDevise(Transaction *tete, CodeDevise devise);

// Filtrages avancés
Transaction *filtrerTransactions(Transaction *tete, CritereRecherche criteres);
void afficherTransactionsFiltrages(Transaction *transactions);

// Statistiques sur les résultats
ResumeRecherche obtenirResumeRecherche(Transaction *tete);
void afficherResumeRecherche(ResumeRecherche resume);

// Tri des résultats
Transaction *trierParDate(Transaction *tete, int ordre_croissant);
Transaction *trierParMontant(Transaction *tete, int ordre_croissant);
Transaction *trierParDescription(Transaction *tete);

// Utilitaires
void afficherTransactionsRecherche(Transaction *tete);
void libererListeTransactionsRecherche(Transaction *tete);
int correspondDes(const char *texte, const char *motif);
int comparerDates(const char *date1, const char *date2);

#endif
