#ifndef BUDGET_H
#define BUDGET_H

#include "multi_devises.h"

#define MAX_BUDGET_DESC 100

// Catégories de budget
typedef enum {
    BUDGET_NOURRITURE,
    BUDGET_LOGEMENT,
    BUDGET_TRANSPORT,
    BUDGET_LOISIRS,
    BUDGET_SANTE,
    BUDGET_EDUCATION,
    BUDGET_AUTRE
} CategorieBudget;

// Structure pour un budget
typedef struct Budget {
    int id;
    char description[MAX_BUDGET_DESC];
    float montant_limite;
    CodeDevise devise;
    CategorieBudget categorie;
    char mois[8]; // Format: YYYY-MM
    float montant_depense; // Montant dépensé ce mois-ci
    int alerte_active; // 1 = alerte si dépassement, 0 = pas d'alerte
    struct Budget *next;
} Budget;

// Structure pour un résumé de budget
typedef struct {
    float montant_limite;
    float montant_depense;
    float pourcentage_utilise;
    int budget_respecte;
} ResumeBudget;

// Prototypes des fonctions
Budget *creerBudget(
    const char *description,
    float montant_limite,
    CodeDevise devise,
    CategorieBudget categorie,
    const char *mois,
    int alerte_active
);

void ajouterBudget(Budget **tete, Budget *nouveau);
void supprimerBudget(Budget **tete, int id);
void afficherBudgets(Budget *tete);
void afficherBudgetsParMois(Budget *tete, const char *mois);

// Gestion des dépenses par rapport au budget
void mettreAJourBudget(Budget *tete, const char *categorie_str, float montant_depense, const char *mois);
void verifierBudget(Budget *tete);

// Résumés et statistiques
ResumeBudget obtenirResumeBudget(Budget *tete, int categorie);
float getTotalUtiliseBudget(Budget *tete);
float getTotalLimiteBudget(Budget *tete);
void afficherAlertesDepassement(Budget *tete);

// Sauvegarde et chargement
void sauvegarderBudgets(const char *filename, Budget *tete);
Budget *chargerBudgets(const char *filename);
void libererListeBudgets(Budget *tete);

// Utilitaires
const char *getCategorieBudgetString(CategorieBudget cat);
CategorieBudget getCategorieBudgetFromString(const char *str);
CategorieBudget getCategorieBudgetFromInt(int i);

#endif
