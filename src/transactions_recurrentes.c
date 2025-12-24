#include "transactions_recurrentes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Crée une nouvelle transaction récurrente
TransactionRecurrente *creerTransactionRecurrente(
    const char *description,
    float montant,
    CodeDevise devise,
    FrequenceRecurrence frequence,
    CategorieRecurrence categorie,
    const char *date_debut,
    const char *date_fin
) {
    TransactionRecurrente *nouvelle = (TransactionRecurrente *)malloc(sizeof(TransactionRecurrente));
    if (nouvelle == NULL) {
        perror("Erreur d'allocation mémoire");
        return NULL;
    }

    nouvelle->id = genererIdUnique();
    strncpy(nouvelle->description, description, MAX_RECURRENCE_DESC - 1);
    nouvelle->description[MAX_RECURRENCE_DESC - 1] = '\0';
    nouvelle->montant = montant;
    nouvelle->devise = devise;
    nouvelle->frequence = frequence;
    nouvelle->categorie = categorie;
    strncpy(nouvelle->date_debut, date_debut, 10);
    nouvelle->date_debut[10] = '\0';
    strncpy(nouvelle->date_fin, date_fin, 10);
    nouvelle->date_fin[10] = '\0';
    nouvelle->active = 1;
    nouvelle->next = NULL;

    return nouvelle;
}

// Ajoute une transaction récurrente à la liste
void ajouterTransactionRecurrente(TransactionRecurrente **tete, TransactionRecurrente *nouvelle) {
    if (*tete == NULL) {
        *tete = nouvelle;
    } else {
        TransactionRecurrente *current = *tete;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = nouvelle;
    }
}

// Affiche toutes les transactions récurrentes
void afficherTransactionsRecurrentes(TransactionRecurrente *tete) {
    if (tete == NULL) {
        printf("Aucune transaction récurrente.\n");
        return;
    }

    printf("\n=== TRANSACTIONS RÉCURRENTES ===\n");
    printf("%-5s | %-30s | %10s | %-15s | %-12s | %-15s | %s\n",
           "ID", "Description", "Montant", "Devise", "Fréquence", "Catégorie", "Active");
    printf("-----+--------------------------------+------------+-----------------+---------------+-----------------+-------\n");

    while (tete != NULL) {
        printf("%-5d | %-30s | %10.2f | %-15s | %-12s | %-15s | %s\n",
               tete->id,
               tete->description,
               tete->montant,
               getCodeDevise(tete->devise),
               getFrequenceString(tete->frequence),
               getCategorieRecurrenceString(tete->categorie),
               tete->active ? "Oui" : "Non");
        tete = tete->next;
    }
}

// Supprime une transaction récurrente par ID
void supprimerTransactionRecurrente(TransactionRecurrente **tete, int id) {
    if (*tete == NULL) return;

    if ((*tete)->id == id) {
        TransactionRecurrente *temp = *tete;
        *tete = (*tete)->next;
        free(temp);
        return;
    }

    TransactionRecurrente *current = *tete;
    while (current->next != NULL) {
        if (current->next->id == id) {
            TransactionRecurrente *temp = current->next;
            current->next = current->next->next;
            free(temp);
            return;
        }
        current = current->next;
    }
}

// Désactive une transaction récurrente
void desactiverTransactionRecurrente(TransactionRecurrente *tete, int id) {
    while (tete != NULL) {
        if (tete->id == id) {
            tete->active = 0;
            return;
        }
        tete = tete->next;
    }
}

// Active une transaction récurrente
void activerTransactionRecurrente(TransactionRecurrente *tete, int id) {
    while (tete != NULL) {
        if (tete->id == id) {
            tete->active = 1;
            return;
        }
        tete = tete->next;
    }
}

// Ajoute des jours à une date
void ajouterJours(char *date, int jours) {
    int annee, mois, jour;
    sscanf(date, "%d-%d-%d", &annee, &mois, &jour);

    time_t t = time(NULL);
    struct tm *date_tm = localtime(&t);
    date_tm->tm_year = annee - 1900;
    date_tm->tm_mon = mois - 1;
    date_tm->tm_mday = jour;

    t = mktime(date_tm);
    t += jours * 24 * 3600;
    date_tm = localtime(&t);

    sprintf(date, "%04d-%02d-%02d",
            date_tm->tm_year + 1900,
            date_tm->tm_mon + 1,
            date_tm->tm_mday);
}

// Compare deux dates (retourne -1 si date1 < date2, 0 si égales, 1 si date1 > date2)
int comparerDates(const char *date1, const char *date2) {
    return strcmp(date1, date2);
}

// Génère les transactions à partir des récurrences
void genererTransactionsRecurrentes(
    TransactionRecurrente *recurrences,
    Transaction **transactions_sortie,
    const char *date_debut,
    const char *date_fin
) {
    while (recurrences != NULL) {
        if (!recurrences->active) {
            recurrences = recurrences->next;
            continue;
        }

        char date_courante[11];
        strcpy(date_courante, recurrences->date_debut);

        int intervalle;
        switch (recurrences->frequence) {
            case QUOTIDIENNE: intervalle = 1; break;
            case HEBDOMADAIRE: intervalle = 7; break;
            case BIHEBDOMADAIRE: intervalle = 14; break;
            case MENSUELLE: intervalle = 30; break;
            case TRIMESTRIELLE: intervalle = 90; break;
            case SEMESTRIELLE: intervalle = 180; break;
            case ANNUELLE: intervalle = 365; break;
            default: intervalle = 1;
        }

        while (comparerDates(date_courante, recurrences->date_fin) <= 0 &&
               comparerDates(date_courante, date_fin) <= 0) {

            if (comparerDates(date_courante, date_debut) >= 0) {
                Transaction *nouvelle = (Transaction *)malloc(sizeof(Transaction));
                if (nouvelle != NULL) {
                    nouvelle->id = genererIdUnique();
                    strcpy(nouvelle->date, date_courante);
                    strcpy(nouvelle->description, recurrences->description);
                    nouvelle->montant = recurrences->montant;
                    nouvelle->next = NULL;

                    if (*transactions_sortie == NULL) {
                        *transactions_sortie = nouvelle;
                    } else {
                        Transaction *current = *transactions_sortie;
                        while (current->next != NULL) {
                            current = current->next;
                        }
                        current->next = nouvelle;
                    }
                }
            }

            ajouterJours(date_courante, intervalle);
        }

        recurrences = recurrences->next;
    }
}

// Sauvegarde les transactions récurrentes dans un fichier
void sauvegarderTransactionsRecurrentes(const char *filename, TransactionRecurrente *tete) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    while (tete != NULL) {
        fprintf(file, "%d|%s|%.2f|%s|%d|%d|%s|%s|%d\n",
                tete->id,
                tete->description,
                tete->montant,
                getCodeDevise(tete->devise),
                tete->frequence,
                tete->categorie,
                tete->date_debut,
                tete->date_fin,
                tete->active);
        tete = tete->next;
    }

    fclose(file);
}

// Charge les transactions récurrentes depuis un fichier
TransactionRecurrente *chargerTransactionsRecurrentes(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return NULL;
    }

    TransactionRecurrente *tete = NULL;
    TransactionRecurrente *dernier = NULL;
    char ligne[512];

    while (fgets(ligne, sizeof(ligne), file)) {
        TransactionRecurrente *nouvelle = (TransactionRecurrente *)malloc(sizeof(TransactionRecurrente));
        if (nouvelle == NULL) break;

        char devise_code[4];
        int freq, cat, active;

        sscanf(ligne, "%d|%99[^|]|%f|%3[^|]|%d|%d|%10[^|]|%10[^|]|%d",
               &nouvelle->id,
               nouvelle->description,
               &nouvelle->montant,
               devise_code,
               &freq,
               &cat,
               nouvelle->date_debut,
               nouvelle->date_fin,
               &active);

        nouvelle->devise = getDeviseFromCode(devise_code);
        nouvelle->frequence = (FrequenceRecurrence)freq;
        nouvelle->categorie = (CategorieRecurrence)cat;
        nouvelle->active = active;
        nouvelle->next = NULL;

        if (tete == NULL) {
            tete = nouvelle;
            dernier = nouvelle;
        } else {
            dernier->next = nouvelle;
            dernier = nouvelle;
        }
    }

    fclose(file);
    return tete;
}

// Libère la mémoire de la liste des transactions récurrentes
void libererListeTransactionsRecurrentes(TransactionRecurrente *tete) {
    while (tete != NULL) {
        TransactionRecurrente *temp = tete;
        tete = tete->next;
        free(temp);
    }
}

// Retourne le string de la fréquence
const char *getFrequenceString(FrequenceRecurrence freq) {
    switch (freq) {
        case QUOTIDIENNE: return "Quotidienne";
        case HEBDOMADAIRE: return "Hebdomadaire";
        case BIHEBDOMADAIRE: return "Bihebdomadaire";
        case MENSUELLE: return "Mensuelle";
        case TRIMESTRIELLE: return "Trimestrielle";
        case SEMESTRIELLE: return "Semestrielle";
        case ANNUELLE: return "Annuelle";
        default: return "Inconnue";
    }
}

// Retourne le string de la catégorie
const char *getCategorieRecurrenceString(CategorieRecurrence cat) {
    switch (cat) {
        case REC_NOURRITURE: return "Nourriture";
        case REC_LOGEMENT: return "Logement";
        case REC_TRANSPORT: return "Transport";
        case REC_LOISIRS: return "Loisirs";
        case REC_SANTE: return "Santé";
        case REC_EDUCATION: return "Éducation";
        case REC_SALAIRE: return "Salaire";
        case REC_AUTRE: return "Autre";
        default: return "Inconnue";
    }
}

// Retourne la fréquence à partir d'un int
FrequenceRecurrence getFrequenceFromInt(int i) {
    return (FrequenceRecurrence)i;
}

// Retourne la catégorie à partir d'un int
CategorieRecurrence getCategorieRecurrenceFromInt(int i) {
    return (CategorieRecurrence)i;
}
