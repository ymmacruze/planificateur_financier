#include "budget.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Variable statique pour générer des IDs uniques
static int budget_id_counter = 0;

// Génère un ID unique pour un budget
int genererIdBudget() {
    return ++budget_id_counter;
}

// Crée un nouveau budget
Budget *creerBudget(
    const char *description,
    float montant_limite,
    CodeDevise devise,
    CategorieBudget categorie,
    const char *mois,
    int alerte_active
) {
    Budget *nouveau = (Budget *)malloc(sizeof(Budget));
    if (nouveau == NULL) {
        perror("Erreur d'allocation mémoire");
        return NULL;
    }

    nouveau->id = genererIdBudget();
    strncpy(nouveau->description, description, MAX_BUDGET_DESC - 1);
    nouveau->description[MAX_BUDGET_DESC - 1] = '\0';
    nouveau->montant_limite = montant_limite;
    nouveau->devise = devise;
    nouveau->categorie = categorie;
    strncpy(nouveau->mois, mois, 7);
    nouveau->mois[7] = '\0';
    nouveau->montant_depense = 0.0;
    nouveau->alerte_active = alerte_active;
    nouveau->next = NULL;

    return nouveau;
}

// Ajoute un budget à la liste
void ajouterBudget(Budget **tete, Budget *nouveau) {
    if (*tete == NULL) {
        *tete = nouveau;
    } else {
        Budget *current = *tete;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = nouveau;
    }
}

// Supprime un budget par ID
void supprimerBudget(Budget **tete, int id) {
    if (*tete == NULL) return;

    if ((*tete)->id == id) {
        Budget *temp = *tete;
        *tete = (*tete)->next;
        free(temp);
        return;
    }

    Budget *current = *tete;
    while (current->next != NULL) {
        if (current->next->id == id) {
            Budget *temp = current->next;
            current->next = current->next->next;
            free(temp);
            return;
        }
        current = current->next;
    }
}

// Affiche tous les budgets
void afficherBudgets(Budget *tete) {
    if (tete == NULL) {
        printf("Aucun budget défini.\n");
        return;
    }

    printf("\n=== TOUS LES BUDGETS ===\n");
    printf("%-5s | %-30s | %12s | %-6s | %-15s | %12s | %15s | %6s\n",
           "ID", "Description", "Limite", "Devise", "Mois", "Dépensé", "% Utilisé", "Alerte");
    printf("-----+--------------------------------+----------+------+---------+----------+----------+------\n");

    while (tete != NULL) {
        float pourcentage = (tete->montant_limite > 0) ?
            (tete->montant_depense / tete->montant_limite * 100) : 0;

        printf("%-5d | %-30s | %10.2f | %-6s | %15s | %10.2f | %14.1f%% | %s\n",
               tete->id,
               tete->description,
               tete->montant_limite,
               getCodeDevise(tete->devise),
               tete->mois,
               tete->montant_depense,
               pourcentage,
               tete->alerte_active ? "Oui" : "Non");
        tete = tete->next;
    }
}

// Affiche les budgets pour un mois spécifique
void afficherBudgetsParMois(Budget *tete, const char *mois) {
    if (tete == NULL) {
        printf("Aucun budget pour le mois %s.\n", mois);
        return;
    }

    printf("\n=== BUDGETS POUR %s ===\n", mois);
    printf("%-5s | %-30s | %12s | %-6s | %12s | %15s\n",
           "ID", "Description", "Limite", "Devise", "Dépensé", "% Utilisé");
    printf("-----+--------------------------------+----------+------+----------+----------\n");

    int found = 0;
    while (tete != NULL) {
        if (strcmp(tete->mois, mois) == 0) {
            found = 1;
            float pourcentage = (tete->montant_limite > 0) ?
                (tete->montant_depense / tete->montant_limite * 100) : 0;

            printf("%-5d | %-30s | %10.2f | %-6s | %10.2f | %14.1f%%\n",
                   tete->id,
                   tete->description,
                   tete->montant_limite,
                   getCodeDevise(tete->devise),
                   tete->montant_depense,
                   pourcentage);
        }
        tete = tete->next;
    }

    if (!found) {
        printf("Aucun budget trouvé pour le mois %s.\n", mois);
    }
}

// Met à jour le budget avec une nouvelle dépense
void mettreAJourBudget(Budget *tete, const char *categorie_str, float montant_depense, const char *mois) {
    CategorieBudget categorie = getCategorieBudgetFromString(categorie_str);

    while (tete != NULL) {
        if (tete->categorie == categorie && strcmp(tete->mois, mois) == 0) {
            tete->montant_depense += montant_depense;
        }
        tete = tete->next;
    }
}

// Vérifie les budgets et affiche les dépassements
void verifierBudget(Budget *tete) {
    printf("\n=== VÉRIFICATION DES BUDGETS ===\n");

    int depassement_trouve = 0;
    while (tete != NULL) {
        if (tete->montant_depense > tete->montant_limite && tete->alerte_active) {
            depassement_trouve = 1;
            float depassement = tete->montant_depense - tete->montant_limite;
            float pourcentage = (tete->montant_depense / tete->montant_limite) * 100;
            printf("⚠️  ALERTE: %s\n", tete->description);
            printf("   Budget: %.2f %s | Dépensé: %.2f %s | Dépassement: %.2f %s (%.1f%%)\n",
                   tete->montant_limite, getCodeDevise(tete->devise),
                   tete->montant_depense, getCodeDevise(tete->devise),
                   depassement, getCodeDevise(tete->devise),
                   pourcentage);
        }
        tete = tete->next;
    }

    if (!depassement_trouve) {
        printf("Tous les budgets sont respectés!\n");
    }
}

// Obtient un résumé de budget pour une catégorie
ResumeBudget obtenirResumeBudget(Budget *tete, int categorie) {
    ResumeBudget resume = {0.0, 0.0, 0.0, 1};

    while (tete != NULL) {
        if (tete->categorie == categorie) {
            resume.montant_limite += tete->montant_limite;
            resume.montant_depense += tete->montant_depense;
        }
        tete = tete->next;
    }

    if (resume.montant_limite > 0) {
        resume.pourcentage_utilise = (resume.montant_depense / resume.montant_limite) * 100;
        resume.budget_respecte = (resume.montant_depense <= resume.montant_limite) ? 1 : 0;
    }

    return resume;
}

// Obtient le total utilisé de tous les budgets
float getTotalUtiliseBudget(Budget *tete) {
    float total = 0.0;
    while (tete != NULL) {
        total += tete->montant_depense;
        tete = tete->next;
    }
    return total;
}

// Obtient le total limité de tous les budgets
float getTotalLimiteBudget(Budget *tete) {
    float total = 0.0;
    while (tete != NULL) {
        total += tete->montant_limite;
        tete = tete->next;
    }
    return total;
}

// Affiche les alertes de dépassement
void afficherAlertesDepassement(Budget *tete) {
    printf("\n=== ALERTES DE DÉPASSEMENT ===\n");
    int alerte_count = 0;

    while (tete != NULL) {
        if (tete->montant_depense > tete->montant_limite && tete->alerte_active) {
            alerte_count++;
            printf("%d. %s: %.2f %s dépassé (%.1f%% du budget)\n",
                   alerte_count,
                   tete->description,
                   tete->montant_depense - tete->montant_limite,
                   getCodeDevise(tete->devise),
                   (tete->montant_depense / tete->montant_limite) * 100);
        }
        tete = tete->next;
    }

    if (alerte_count == 0) {
        printf("Aucune alerte de dépassement.\n");
    }
}

// Sauvegarde les budgets dans un fichier
void sauvegarderBudgets(const char *filename, Budget *tete) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    while (tete != NULL) {
        fprintf(file, "%d|%s|%.2f|%s|%d|%s|%.2f|%d\n",
                tete->id,
                tete->description,
                tete->montant_limite,
                getCodeDevise(tete->devise),
                tete->categorie,
                tete->mois,
                tete->montant_depense,
                tete->alerte_active);
        tete = tete->next;
    }

    fclose(file);
}

// Charge les budgets depuis un fichier
Budget *chargerBudgets(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return NULL;
    }

    Budget *tete = NULL;
    Budget *dernier = NULL;
    char ligne[512];

    while (fgets(ligne, sizeof(ligne), file)) {
        Budget *nouveau = (Budget *)malloc(sizeof(Budget));
        if (nouveau == NULL) break;

        char devise_code[4];
        int cat, alerte;

        sscanf(ligne, "%d|%99[^|]|%f|%3[^|]|%d|%7[^|]|%f|%d",
               &nouveau->id,
               nouveau->description,
               &nouveau->montant_limite,
               devise_code,
               &cat,
               nouveau->mois,
               &nouveau->montant_depense,
               &alerte);

        nouveau->devise = getDeviseFromCode(devise_code);
        nouveau->categorie = (CategorieBudget)cat;
        nouveau->alerte_active = alerte;
        nouveau->next = NULL;

        if (tete == NULL) {
            tete = nouveau;
            dernier = nouveau;
        } else {
            dernier->next = nouveau;
            dernier = nouveau;
        }

        if (nouveau->id >= budget_id_counter) {
            budget_id_counter = nouveau->id;
        }
    }

    fclose(file);
    return tete;
}

// Libère la mémoire de la liste des budgets
void libererListeBudgets(Budget *tete) {
    while (tete != NULL) {
        Budget *temp = tete;
        tete = tete->next;
        free(temp);
    }
}

// Retourne le string de la catégorie de budget
const char *getCategorieBudgetString(CategorieBudget cat) {
    switch (cat) {
        case BUDGET_NOURRITURE: return "Nourriture";
        case BUDGET_LOGEMENT: return "Logement";
        case BUDGET_TRANSPORT: return "Transport";
        case BUDGET_LOISIRS: return "Loisirs";
        case BUDGET_SANTE: return "Santé";
        case BUDGET_EDUCATION: return "Éducation";
        case BUDGET_AUTRE: return "Autre";
        default: return "Inconnue";
    }
}

// Retourne la catégorie de budget à partir du string
CategorieBudget getCategorieBudgetFromString(const char *str) {
    if (strcmp(str, "Nourriture") == 0) return BUDGET_NOURRITURE;
    if (strcmp(str, "Logement") == 0) return BUDGET_LOGEMENT;
    if (strcmp(str, "Transport") == 0) return BUDGET_TRANSPORT;
    if (strcmp(str, "Loisirs") == 0) return BUDGET_LOISIRS;
    if (strcmp(str, "Santé") == 0) return BUDGET_SANTE;
    if (strcmp(str, "Éducation") == 0) return BUDGET_EDUCATION;
    if (strcmp(str, "Autre") == 0) return BUDGET_AUTRE;
    return BUDGET_AUTRE;
}

// Retourne la catégorie de budget à partir d'un int
CategorieBudget getCategorieBudgetFromInt(int i) {
    return (CategorieBudget)i;
}
