#ifndef SYNCHRONISATION_LOCALE_H
#define SYNCHRONISATION_LOCALE_H

#include "database.h"
#include <time.h>

// Structure pour une sauvegarde
typedef struct {
    char nom_backup[100];
    char chemin_backup[256];
    time_t date_creation;
    int taille_fichier;
} SauvegardeLocale;

// Structure pour les statistiques de synchronisation
typedef struct {
    int nombre_fichiers_syncs;
    int nombre_fichiers_modifies;
    time_t derniere_synchro;
    float espace_disque_utilise;
} StatsSynchronisation;

// Prototypes des fonctions
void creerSauvegardeLocale(const char *repertoire_source, const char *repertoire_backup);
void restaurerDepuisBackup(const char *repertoire_backup, const char *repertoire_destination);

// Gestion des versions
void creerPointDeRestauration(const char *label);
void restaurerPointDeRestauration(const char *label);
void afficherListePointsRestauration();

// Synchronisation
void synchroniserFichiers(const char *source, const char *destination);
void synchroniserAvecVerification(const char *source, const char *destination);
StatsSynchronisation obtenirStatsSynchronisation();

// Sauvegarde automatique
void activerSauvegardeAutomatique(int intervalle_secondes);
void desactiverSauvegardeAutomatique();

// Historique
typedef struct {
    char description[256];
    time_t date_modification;
    char ancien_contenu[512];
    char nouveau_contenu[512];
} HistoriqueModification;

void sauvegarderHistorique(HistoriqueModification modification);
void afficherHistoriqueLimite(int nombre_dernieres);

// Nettoyage
void nettoyerAnciennesBackups(int jours_retenus);
void calculerEspaceDisqueUtilise(const char *repertoire);

#endif
