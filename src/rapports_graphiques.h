#ifndef RAPPORTS_GRAPHIQUES_H
#define RAPPORTS_GRAPHIQUES_H

#include "database.h"
#include "depenses.h"
#include "revenus.h"

#define MAX_BAR_WIDTH 50

// Structure pour un rapport statistique
typedef struct {
    float total;
    float moyenne;
    float median;
    float min;
    float max;
    int count;
} StatistiquesFinancieres;

// Prototypes des fonctions de rapport
void afficherHistogrammeHorizontal(const char *label, float valeur, float max, char *symbole);
void afficherGraphiqueBarres(const char *titre, const char *categories[], float valeurs[], int count);
void afficherGraphiqueCamembert(const char *titre, const char *categories[], float valeurs[], int count);

// Rapports détaillés
void afficherRapportDepensesParCategorie(Transaction *depenses);
void afficherRapportRevenusParCategorie(Transaction *revenus);
void afficherRapportComparatifMois(Transaction *depenses, Transaction *revenus, const char *mois);
void afficherRapportAnnuel(Transaction *depenses, Transaction *revenus, const char *annee);

// Statistiques
StatistiquesFinancieres calculerStatistiques(Transaction *tete);
void afficherStatistiques(const char *titre, StatistiquesFinancieres stats);

// Rapports comparatifs
void afficherComparaisonPeriodes(Transaction *depenses, const char *date1, const char *date2);
void afficherTendancesMois(Transaction *depenses, const char *annee);

// Utilitaires
float obtenirMaxValeur(float *valeurs, int count);
float calculerMediane(float *valeurs, int count);

#endif
