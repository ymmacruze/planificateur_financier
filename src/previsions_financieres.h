#ifndef PREVISIONS_FINANCIERES_H
#define PREVISIONS_FINANCIERES_H

#include "database.h"

// Types de modèles de prévision
typedef enum {
    MODELE_SIMPLE,        // Moyenne mobile simple
    MODELE_EXPONENTIEL,   // Moyenne mobile exponentielle
    MODELE_TENDANCE,      // Analyse de tendance linéaire
    MODELE_SAISONNIER     // Prise en compte de la saisonnalité
} ModeleProvision;

// Structure pour les prévisions
typedef struct {
    float montant_prevu;
    float intervalle_confiance_bas;
    float intervalle_confiance_haut;
    float probabilite_confiance;
    int fiabilite; // 0-100
} PrevisionMontant;

// Prototypes des fonctions
PrevisionMontant previsionMois(Transaction *tete, ModeleProvision modele);
PrevisionMontant previsionTrimestre(Transaction *tete, ModeleProvision modele);
PrevisionMontant previsionAnnee(Transaction *tete, ModeleProvision modele);

// Modèles spécifiques
float calculerMoyenneSimple(Transaction *tete, int nb_periodes);
float calculerMoyenneExponentielle(Transaction *tete, float alpha);
float calculerTendanceLineaire(Transaction *tete, int *pente);
float previsionParSaisonnalite(Transaction *tete);

// Prévisions de cashflow
void afficherPrevisionsCashflow(Transaction *revenus, Transaction *depenses, int mois_avance);
float previsionSolde(Transaction *revenus, Transaction *depenses, int mois);

// Analyse de scénarios
void analyserScenario_pessimiste(Transaction *revenus, Transaction *depenses);
void analyserScenario_optimiste(Transaction *revenus, Transaction *depenses);
void analyserScenario_realiste(Transaction *revenus, Transaction *depenses);

// Affichage
void afficherPrevisions(PrevisionMontant prevision, const char *periode);
void afficherIntervalleConfiance(PrevisionMontant prevision);

#endif
