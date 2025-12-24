#ifndef IA_SIMPLE_H
#define IA_SIMPLE_H

#include "database.h"

// Types de suggestions
typedef enum {
    SUGGESTION_REDUCTION_DEPENSES,
    SUGGESTION_AUGMENTATION_EPARGNE,
    SUGGESTION_BUDGET,
    SUGGESTION_INVESTISSEMENT,
    SUGGESTION_ECONOMIE
} TypeSuggestion;

// Structure pour une suggestion
typedef struct {
    TypeSuggestion type;
    char titre[100];
    char description[256];
    float gain_potentiel;
    int priorite; // 1-5
} Suggestion;

// Catégories détectées par IA
typedef enum {
    ANOMALIE_DEPENSE_HAUTE,
    ANOMALIE_PATTERN_CHANGE,
    ANOMALIE_NOUVEAU_TYPE,
    ANOMALIE_DOUBLE_PAIEMENT
} TypeAnomalieIA;

// Structure pour une anomalie
typedef struct {
    TypeAnomalieIA type;
    int id_transaction;
    char description[256];
    float confiance; // 0-1
} AnomalieIA;

// Prototypes des fonctions
Suggestion *detecterSuggestionsPersonnalisees(Transaction *revenus, Transaction *depenses);
Suggestion *suggererReductionDepenses(Transaction *depenses);
Suggestion *suggererAugmentationEpargne(Transaction *revenus, Transaction *depenses);
Suggestion *suggererBudgetPersonnalise(Transaction *depenses);

// Catégorisation automatique
char *categoriserAutomatique(const char *description);
float calculateur_confiance_categorisation(const char *description);

// Détection d'anomalies avancée
AnomalieIA *detecterAnomaliesAvancees(Transaction *tete);
void afficherAnomalies(AnomalieIA *anomalies);
int estDoublePaiement(Transaction *t1, Transaction *t2);

// Recommandations financières
void afficherRecommandationsFinancieres(Transaction *revenus, Transaction *depenses);
char *genererConseils(float taux_epargne, float taux_depenses);

// Scoring financier
typedef struct {
    float score_sante_financiere; // 0-100
    float score_equilibre; // 0-100
    int niveau_risque; // 1-5
} ScoreFinancier;

ScoreFinancier calculerScoreFinancier(Transaction *revenus, Transaction *depenses);
void afficherScoreFinancier(ScoreFinancier score);

// Prédictions comportementales
void predireComportementDepenses(Transaction *depenses);
int seriez_vous_endette(Transaction *revenus, Transaction *depenses, int mois);

#endif
