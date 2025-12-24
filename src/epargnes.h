#ifndef EPARGNES_H
#define EPARGNES_H

#include "database.h"

// Types d'épargne
typedef enum {
    COMPTE_EPARGNE,
    LIVRET_A,
    PLAN_EPARGNE_LOGEMENT,
    ASSURANCE_VIE,
    AUTRE_EPARGNE
} TypeEpargne;

// Structure étendue pour l'épargne
typedef struct {
    Epargne epargne; // Structure de base
    TypeEpargne type;
} EpargneEtendue;

// Prototypes des fonctions
EpargneEtendue *creerEpargne(const char *date, const char *description, float montant, 
                            float taux_interet, int duree_jours, TypeEpargne type);
void ajouterEpargne(Epargne **tete, EpargneEtendue *nouvelleEpargne);
void afficherEpargnes(Epargne *tete);
float totalEpargnes(Epargne *tete);
void afficherEpargnesParType(Epargne *tete, TypeEpargne type);
float calculerInterets(Epargne *epargne);
void libererListeEpargnes(Epargne *tete);

#endif