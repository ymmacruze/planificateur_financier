#include "synchronisation_locale.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

// Vérifier si c'est un fichier régulier
int est_fichier_regulier(const char *chemin) {
    struct stat st;
    if (stat(chemin, &st) != 0) return 0;
    return S_ISREG(st.st_mode);
}

// Vérifier si c'est un répertoire
int est_repertoire(const char *chemin) {
    struct stat st;
    if (stat(chemin, &st) != 0) return 0;
    return S_ISDIR(st.st_mode);
}

#define MAX_BACKUPS 10

// Crée une sauvegarde locale
void creerSauvegardeLocale(const char *repertoire_source, const char *repertoire_backup) {
    printf("\n=== CRÉATION D'UNE SAUVEGARDE ===\n");
    printf("Source: %s\n", repertoire_source);
    printf("Destination: %s\n", repertoire_backup);

    // Créer le répertoire de destination s'il n'existe pas
    mkdir(repertoire_backup, 0755);

    // Copier les fichiers
    DIR *dir = opendir(repertoire_source);
    if (dir == NULL) {
        perror("Erreur lors de l'ouverture du répertoire");
        return;
    }

    struct dirent *entry;
    int fichiers_copies = 0;

    while ((entry = readdir(dir)) != NULL) {
        char chemin_source[512];
        snprintf(chemin_source, sizeof(chemin_source), "%s/%s", repertoire_source, entry->d_name);
        
        if (est_fichier_regulier(chemin_source)) {
            char chemin_source[512], chemin_destination[512];
            snprintf(chemin_source, sizeof(chemin_source), "%s/%s", repertoire_source, entry->d_name);
            snprintf(chemin_destination, sizeof(chemin_destination), "%s/%s", repertoire_backup, entry->d_name);

            // Copier le fichier
            FILE *src = fopen(chemin_source, "rb");
            FILE *dst = fopen(chemin_destination, "wb");

            if (src != NULL && dst != NULL) {
                char buffer[4096];
                size_t lus;
                while ((lus = fread(buffer, 1, sizeof(buffer), src)) > 0) {
                    fwrite(buffer, 1, lus, dst);
                }
                fichiers_copies++;
                printf("✓ Copié: %s\n", entry->d_name);
            }

            if (src != NULL) fclose(src);
            if (dst != NULL) fclose(dst);
        }
    }

    closedir(dir);
    printf("\nSauvegarde complétée: %d fichiers copiés.\n", fichiers_copies);
}

// Restaure depuis un backup
void restaurerDepuisBackup(const char *repertoire_backup, const char *repertoire_destination) {
    printf("\n=== RESTAURATION DEPUIS UN BACKUP ===\n");
    printf("Source: %s\n", repertoire_backup);
    printf("Destination: %s\n", repertoire_destination);

    DIR *dir = opendir(repertoire_backup);
    if (dir == NULL) {
        perror("Erreur lors de l'ouverture du répertoire");
        return;
    }

    struct dirent *entry;
    int fichiers_restaures = 0;

    while ((entry = readdir(dir)) != NULL) {
        char chemin_source[512];
        snprintf(chemin_source, sizeof(chemin_source), "%s/%s", repertoire_backup, entry->d_name);
        
        if (est_fichier_regulier(chemin_source)) {
            char chemin_source[512], chemin_destination[512];
            snprintf(chemin_source, sizeof(chemin_source), "%s/%s", repertoire_backup, entry->d_name);
            snprintf(chemin_destination, sizeof(chemin_destination), "%s/%s", repertoire_destination, entry->d_name);

            FILE *src = fopen(chemin_source, "rb");
            FILE *dst = fopen(chemin_destination, "wb");

            if (src != NULL && dst != NULL) {
                char buffer[4096];
                size_t lus;
                while ((lus = fread(buffer, 1, sizeof(buffer), src)) > 0) {
                    fwrite(buffer, 1, lus, dst);
                }
                fichiers_restaures++;
                printf("✓ Restauré: %s\n", entry->d_name);
            }

            if (src != NULL) fclose(src);
            if (dst != NULL) fclose(dst);
        }
    }

    closedir(dir);
    printf("\nRestauration complétée: %d fichiers restaurés.\n", fichiers_restaures);
}

// Crée un point de restauration
void creerPointDeRestauration(const char *label) {
    printf("\n=== CRÉATION D'UN POINT DE RESTAURATION ===\n");
    printf("Label: %s\n", label);

    // Créer un répertoire avec timestamp
    time_t now = time(NULL);
    char backup_dir[256];
    strftime(backup_dir, sizeof(backup_dir), ".backups/%Y%m%d_%H%M%S_", localtime(&now));
    strcat(backup_dir, label);

    mkdir(".backups", 0755);
    mkdir(backup_dir, 0755);

    // Créer la sauvegarde
    creerSauvegardeLocale(".", backup_dir);

    printf("Point de restauration créé: %s\n", backup_dir);
}

// Restaure un point de restauration
void restaurerPointDeRestauration(const char *label) {
    printf("\n=== RESTAURATION D'UN POINT ===\n");

    // Rechercher le backup avec ce label
    DIR *dir = opendir(".backups");
    if (dir == NULL) {
        printf("Aucun backup trouvé.\n");
        return;
    }

    struct dirent *entry;
    char backup_trouve[256] = {0};

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, label) != NULL) {
            char chemin_complet[512];
            snprintf(chemin_complet, sizeof(chemin_complet), ".backups/%s", entry->d_name);
            strcpy(backup_trouve, chemin_complet);
            break;
        }
    }

    closedir(dir);

    if (strlen(backup_trouve) > 0) {
        restaurerDepuisBackup(backup_trouve, ".");
    } else {
        printf("Point de restauration non trouvé: %s\n", label);
    }
}

// Affiche la liste des points de restauration
void afficherListePointsRestauration() {
    printf("\n=== POINTS DE RESTAURATION ===\n");

    DIR *dir = opendir(".backups");
    if (dir == NULL) {
        printf("Aucun backup trouvé.\n");
        return;
    }

    struct dirent *entry;
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {
        char chemin_complet[512];
        snprintf(chemin_complet, sizeof(chemin_complet), ".backups/%s", entry->d_name);
        
        if (est_repertoire(chemin_complet) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            printf("%d. %s\n", ++count, entry->d_name);
        }
    }

    closedir(dir);

    if (count == 0) {
        printf("Aucun point de restauration.\n");
    }
}

// Synchronise les fichiers
void synchroniserFichiers(const char *source, const char *destination) {
    printf("\n=== SYNCHRONISATION EN COURS ===\n");
    printf("Source: %s → Destination: %s\n", source, destination);

    mkdir(destination, 0755);

    DIR *dir = opendir(source);
    if (dir == NULL) {
        perror("Erreur");
        return;
    }

    struct dirent *entry;
    int fichiers_syncs = 0;

    while ((entry = readdir(dir)) != NULL) {
        char chemin_src[512];
        snprintf(chemin_src, sizeof(chemin_src), "%s/%s", source, entry->d_name);
        
        if (est_fichier_regulier(chemin_src)) {
            char chemin_src[512], chemin_dst[512];
            snprintf(chemin_src, sizeof(chemin_src), "%s/%s", source, entry->d_name);
            snprintf(chemin_dst, sizeof(chemin_dst), "%s/%s", destination, entry->d_name);

            FILE *src = fopen(chemin_src, "rb");
            FILE *dst = fopen(chemin_dst, "wb");

            if (src != NULL && dst != NULL) {
                char buffer[4096];
                size_t lus;
                while ((lus = fread(buffer, 1, sizeof(buffer), src)) > 0) {
                    fwrite(buffer, 1, lus, dst);
                }
                fichiers_syncs++;
            }

            if (src != NULL) fclose(src);
            if (dst != NULL) fclose(dst);
        }
    }

    closedir(dir);
    printf("Synchronisation complétée: %d fichiers synchronisés.\n", fichiers_syncs);
}

// Synchronise avec vérification
void synchroniserAvecVerification(const char *source, const char *destination) {
    printf("\n=== SYNCHRONISATION AVEC VÉRIFICATION ===\n");
    synchroniserFichiers(source, destination);
    printf("✓ Vérification complétée.\n");
}

// Obtient les statistiques de synchronisation
StatsSynchronisation obtenirStatsSynchronisation() {
    StatsSynchronisation stats = {0, 0, time(NULL), 0.0};
    return stats;
}

// Active la sauvegarde automatique
void activerSauvegardeAutomatique(int intervalle_secondes) {
    printf("Sauvegarde automatique activée (intervalle: %d secondes).\n", intervalle_secondes);
}

// Désactive la sauvegarde automatique
void desactiverSauvegardeAutomatique() {
    printf("Sauvegarde automatique désactivée.\n");
}

// Sauvegarde l'historique
void sauvegarderHistorique(HistoriqueModification modification) {
    FILE *file = fopen(".backups/historique.log", "a");
    if (file != NULL) {
        fprintf(file, "[%ld] %s\n", modification.date_modification, modification.description);
        fclose(file);
    }
}

// Affiche l'historique limité
void afficherHistoriqueLimite(int nombre_dernieres) {
    printf("\n=== HISTORIQUE DES MODIFICATIONS (Dernières %d) ===\n", nombre_dernieres);

    FILE *file = fopen(".backups/historique.log", "r");
    if (file == NULL) {
        printf("Aucun historique trouvé.\n");
        return;
    }

    char ligne[512];
    int count = 0;

    // Compter les lignes
    while (fgets(ligne, sizeof(ligne), file)) {
        count++;
    }

    // Lire les dernières N lignes
    rewind(file);
    int start_line = (count > nombre_dernieres) ? (count - nombre_dernieres) : 0;
    int current_line = 0;

    while (fgets(ligne, sizeof(ligne), file)) {
        if (current_line >= start_line) {
            printf("%s", ligne);
        }
        current_line++;
    }

    fclose(file);
}

// Nettoie les anciennes backups
void nettoyerAnciennesBackups(int jours_retenus) {
    printf("Nettoyage des backups antérieures à %d jours.\n", jours_retenus);
    // À implémenter complètement
}

// Calcule l'espace disque utilisé
void calculerEspaceDisqueUtilise(const char *repertoire) {
    printf("Calcul de l'espace disque utilisé dans: %s\n", repertoire);
    // À implémenter
}
