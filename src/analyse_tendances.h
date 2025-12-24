#ifndef ANALYSE_TENDANCES_H
#define ANALYSE_TENDANCES_H

#include "database.h"

// Structure pour une analyse de tendance
typedef struct {
    float montant_precedent;
    float montant_courant;
    float variation_absolue;
    float variation_pourcentage;
    int tendance; // 1 = hausse, -1 = baisse, 0 = stable
} TendanceEvolution;

// Structure pour les prévisions
typedef struct {
    float depenses_prevues_mois_prochain;
    float revenus_prevus_mois_prochain;
    float solde_prevu;
    float trend_force; // Force de la tendance (0-1)
} Previsions;

// Prototypes des fonctions
void analyserTendancesMois(Transaction *tete, const char *annee);
void analyserTendancesCategories(Transaction *tete);
TendanceEvolution calculerEvolution(float ancien, float nouveau);
void afficherEvolution(const char *label, TendanceEvolution evolution);

// Détection d'anomalies
void detecterAnomalies(Transaction *tete);
float obtenirMoyenneMensuelle(Transaction *tete);
float obtenirEcartType(Transaction *tete);
int estAnomalique(float valeur, float moyenne, float ecart_type);

// Analyse des croissances
float obtenirTaux_croissance(Transaction *tete, const char *periode1, const char *periode2);
void afficherAnalyse_croissance(Transaction *tete, const char *annee);

// Tendances saisonnières
void analyserSaisonnalite(Transaction *tete, const char *annee);
void afficherMoyennesSaisonnières(Transaction *tete, const char *annee);

// Sauvegarde et chargement
void sauvegarderAnalyseTendances(const char *filename, const char *contenu);

#endif
