#include "recherche_filtrage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>

// Convertit une chaîne en minuscules
void toLower(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

// Vérifie si un texte correspond à un motif (recherche partielle, insensible à la casse)
int correspondDes(const char *texte, const char *motif) {
    if (motif == NULL || strlen(motif) == 0) {
        return 1; // Pas de filtre = tout correspond
    }

    char texte_lower[100];
    char motif_lower[100];
    strncpy(texte_lower, texte, 99);
    texte_lower[99] = '\0';
    strncpy(motif_lower, motif, 99);
    motif_lower[99] = '\0';

    toLower(texte_lower);
    toLower(motif_lower);

    return strstr(texte_lower, motif_lower) != NULL;
}

// Recherche les transactions selon les critères
Transaction *rechercherTransactions(Transaction *tete, CritereRecherche criteres) {
    Transaction *resultats = NULL;
    Transaction *dernier = NULL;

    while (tete != NULL) {
        int correspond = 1;

        // Vérifier la date de début
        if (strlen(criteres.date_debut) > 0 && comparerDates(tete->date, criteres.date_debut) < 0) {
            correspond = 0;
        }

        // Vérifier la date de fin
        if (correspond && strlen(criteres.date_fin) > 0 && comparerDates(tete->date, criteres.date_fin) > 0) {
            correspond = 0;
        }

        // Vérifier le montant minimum
        if (correspond && criteres.montant_min > 0 && tete->montant < criteres.montant_min) {
            correspond = 0;
        }

        // Vérifier le montant maximum
        if (correspond && criteres.montant_max > 0 && tete->montant > criteres.montant_max) {
            correspond = 0;
        }

        // Vérifier la description
        if (correspond && strlen(criteres.description) > 0 && !correspondDes(tete->description, criteres.description)) {
            correspond = 0;
        }

        if (correspond) {
            Transaction *copie = (Transaction *)malloc(sizeof(Transaction));
            if (copie != NULL) {
                *copie = *tete;
                copie->next = NULL;

                if (resultats == NULL) {
                    resultats = copie;
                    dernier = copie;
                } else {
                    dernier->next = copie;
                    dernier = copie;
                }
            }
        }

        tete = tete->next;
    }

    return resultats;
}

// Recherche par date
Transaction *rechercherParDate(Transaction *tete, const char *date_debut, const char *date_fin) {
    CritereRecherche criteres = {{0}, {0}, 0, 0, {0}, EUR, -1};
    if (date_debut) strncpy(criteres.date_debut, date_debut, 10);
    if (date_fin) strncpy(criteres.date_fin, date_fin, 10);
    return rechercherTransactions(tete, criteres);
}

// Recherche par montant
Transaction *rechercherParMontant(Transaction *tete, float montant_min, float montant_max) {
    CritereRecherche criteres = {{0}, {0}, montant_min, montant_max, {0}, EUR, -1};
    return rechercherTransactions(tete, criteres);
}

// Recherche par description
Transaction *rechercherParDescription(Transaction *tete, const char *description) {
    CritereRecherche criteres = {{0}, {0}, 0, 0, {0}, EUR, -1};
    if (description) strncpy(criteres.description, description, 99);
    return rechercherTransactions(tete, criteres);
}

// Recherche par devise
Transaction *rechercherParDevise(Transaction *tete, CodeDevise devise) {
    // Cette fonction est pour une utilisation future quand les transactions auront une devise
    // Pour l'instant, retourne simplement les transactions
    Transaction *resultats = NULL;
    Transaction *dernier = NULL;

    while (tete != NULL) {
        Transaction *copie = (Transaction *)malloc(sizeof(Transaction));
        if (copie != NULL) {
            *copie = *tete;
            copie->next = NULL;

            if (resultats == NULL) {
                resultats = copie;
                dernier = copie;
            } else {
                dernier->next = copie;
                dernier = copie;
            }
        }
        tete = tete->next;
    }

    return resultats;
}

// Filtre les transactions selon les critères
Transaction *filtrerTransactions(Transaction *tete, CritereRecherche criteres) {
    return rechercherTransactions(tete, criteres);
}

// Affiche les transactions filtrées
void afficherTransactionsFiltrages(Transaction *transactions) {
    if (transactions == NULL) {
        printf("Aucune transaction ne correspond aux critères de recherche.\n");
        return;
    }

    printf("\n=== RÉSULTATS DE LA RECHERCHE ===\n");
    printf("%-5s | %-12s | %-40s | %12s\n", "ID", "Date", "Description", "Montant");
    printf("-----+--------------+------------------------------------------+----------\n");

    while (transactions != NULL) {
        printf("%-5d | %-12s | %-40s | %10.2f\n",
               transactions->id,
               transactions->date,
               transactions->description,
               transactions->montant);
        transactions = transactions->next;
    }
}

// Obtient un résumé des résultats de recherche
ResumeRecherche obtenirResumeRecherche(Transaction *tete) {
    ResumeRecherche resume = {0, 0.0, 0.0, FLT_MAX, 0.0};

    if (tete == NULL) {
        return resume;
    }

    int count = 0;
    while (tete != NULL) {
        resume.montant_total += tete->montant;
        if (tete->montant < resume.montant_min) {
            resume.montant_min = tete->montant;
        }
        if (tete->montant > resume.montant_max) {
            resume.montant_max = tete->montant;
        }
        count++;
        tete = tete->next;
    }

    resume.nb_resultats = count;
    resume.montant_moyen = (count > 0) ? (resume.montant_total / count) : 0.0;

    return resume;
}

// Affiche le résumé de la recherche
void afficherResumeRecherche(ResumeRecherche resume) {
    printf("\n=== RÉSUMÉ DES RÉSULTATS ===\n");
    printf("Nombre de résultats: %d\n", resume.nb_resultats);
    printf("Montant total: %.2f\n", resume.montant_total);
    printf("Montant moyen: %.2f\n", resume.montant_moyen);
    if (resume.nb_resultats > 0) {
        printf("Montant minimum: %.2f\n", resume.montant_min);
        printf("Montant maximum: %.2f\n", resume.montant_max);
    }
}

// Trie les transactions par date
Transaction *trierParDate(Transaction *tete, int ordre_croissant) {
    if (tete == NULL || tete->next == NULL) {
        return tete;
    }

    // Bubble sort (simple pour une démo)
    Transaction *current, *nextItem, *lastSorted = NULL;
    int swapped = 1;

    while (swapped) {
        swapped = 0;
        current = tete;

        while (current->next != lastSorted) {
            nextItem = current->next;
            int comparison = strcmp(current->date, nextItem->date);

            if ((ordre_croissant && comparison > 0) || (!ordre_croissant && comparison < 0)) {
                // Swap
                current->next = nextItem->next;
                nextItem->next = current;

                if (current == tete) {
                    tete = nextItem;
                } else {
                    Transaction *prev = tete;
                    while (prev->next != current) {
                        prev = prev->next;
                    }
                    prev->next = nextItem;
                }

                current = nextItem;
                swapped = 1;
            }

            current = current->next;
        }

        lastSorted = current;
    }

    return tete;
}

// Trie les transactions par montant
Transaction *trierParMontant(Transaction *tete, int ordre_croissant) {
    if (tete == NULL || tete->next == NULL) {
        return tete;
    }

    // Bubble sort
    Transaction *current, *nextItem, *lastSorted = NULL;
    int swapped = 1;

    while (swapped) {
        swapped = 0;
        current = tete;

        while (current->next != lastSorted) {
            nextItem = current->next;
            int comparison = (current->montant > nextItem->montant) ? 1 : -1;

            if ((ordre_croissant && comparison > 0) || (!ordre_croissant && comparison < 0)) {
                // Swap
                current->next = nextItem->next;
                nextItem->next = current;

                if (current == tete) {
                    tete = nextItem;
                } else {
                    Transaction *prev = tete;
                    while (prev->next != current) {
                        prev = prev->next;
                    }
                    prev->next = nextItem;
                }

                current = nextItem;
                swapped = 1;
            }

            current = current->next;
        }

        lastSorted = current;
    }

    return tete;
}

// Trie les transactions par description
Transaction *trierParDescription(Transaction *tete) {
    if (tete == NULL || tete->next == NULL) {
        return tete;
    }

    // Bubble sort
    Transaction *current, *nextItem, *lastSorted = NULL;
    int swapped = 1;

    while (swapped) {
        swapped = 0;
        current = tete;

        while (current->next != lastSorted) {
            nextItem = current->next;
            int comparison = strcmp(current->description, nextItem->description);

            if (comparison > 0) {
                // Swap
                current->next = nextItem->next;
                nextItem->next = current;

                if (current == tete) {
                    tete = nextItem;
                } else {
                    Transaction *prev = tete;
                    while (prev->next != current) {
                        prev = prev->next;
                    }
                    prev->next = nextItem;
                }

                current = nextItem;
                swapped = 1;
            }

            current = current->next;
        }

        lastSorted = current;
    }

    return tete;
}

// Affiche les transactions de recherche
void afficherTransactionsRecherche(Transaction *tete) {
    afficherTransactionsFiltrages(tete);
}

// Libère la mémoire de la liste des transactions de recherche
void libererListeTransactionsRecherche(Transaction *tete) {
    while (tete != NULL) {
        Transaction *temp = tete;
        tete = tete->next;
        free(temp);
    }
}
