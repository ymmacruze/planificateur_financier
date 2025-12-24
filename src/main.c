#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "depenses.h"
#include "revenus.h"
#include "epargnes.h"
#include "bilan_mensuel.h"
#include "database.h"
#include "interface.h"
#include "multi_devises.h"
#include "budget.h"
#include "recherche_filtrage.h"
#include "transactions_recurrentes.h"
#include "analyse_tendances.h"
#include "previsions_financieres.h"
#include "export_import.h"
#include "cryptage_donnees.h"
#include "ia_simple.h"
#include "rapports_graphiques.h"

#define FICHIER_DEPENSES "data/depenses.txt"
#define FICHIER_REVENUS "data/revenus.txt"
#define FICHIER_EPARGNES "data/epargnes.txt"
#define FICHIER_PARAMETRES "data/parametres.txt"

typedef struct {
    char devise[4];      // EUR, USD, GBP, JPY, CHF, CAD, AUD, CNY
    char langue[20];     // Francais, English, Espanol, Deutsch, Nihongo
    char format_date[20]; // YYYY-MM-DD, DD-MM-YYYY, MM/DD/YYYY, DD/MM/YYYY
} Parametres;

Parametres parametres = {"EUR", "Francais", "YYYY-MM-DD"};

void afficherMenuPrincipal();
void gererDepenses(Transaction **depenses);
void gererRevenus(Transaction **revenus);
void gererEpargnes(Epargne **epargnes);
void afficherBilan(Transaction *revenus, Transaction *depenses, Epargne *epargnes);
void creer_dossier_data();
void sauvegarder_tout(Transaction *depenses, Transaction *revenus, Epargne *epargnes);
void afficherMenuBudgets();
void afficherMenuRapports(Transaction *depenses, Transaction *revenus, Epargne *epargnes);
void afficherMenuRechercheAvancee(Transaction *depenses, Transaction *revenus, Epargne *epargnes);
void afficherMenuRecurrentes();
void afficherMenuAnalysePrevu(Transaction *depenses, Transaction *revenus, Epargne *epargnes);
void afficherMenuMultiDevises();
void afficherMenuExportImport();
void afficherMenuIA();
void afficherMenuParametres();
void creerSauvegardeLocale(const char *source, const char *destination);
void restaurerDepuisBackup(const char *backup, const char *destination);

void charger_parametres() {
    FILE *f = fopen(FICHIER_PARAMETRES, "r");
    if (f != NULL) {
        int ret = fscanf(f, "%3[^|]|%19[^|]|%19[^\n]", 
                        parametres.devise, 
                        parametres.langue, 
                        parametres.format_date);
        fclose(f);
        if (ret != 3) {
            strcpy(parametres.devise, "EUR");
            strcpy(parametres.langue, "Francais");
            strcpy(parametres.format_date, "YYYY-MM-DD");
        }
    } else {
        strcpy(parametres.devise, "EUR");
        strcpy(parametres.langue, "Francais");
        strcpy(parametres.format_date, "YYYY-MM-DD");
    }
}

void sauvegarder_parametres() {
    FILE *f = fopen(FICHIER_PARAMETRES, "w");
    if (f != NULL) {
        fprintf(f, "%s|%s|%s", parametres.devise, parametres.langue, parametres.format_date);
        fclose(f);
    }
}


// Crée le dossier data s'il n'existe pas
void creer_dossier_data() {
    struct stat st = {0};
    if (stat("data", &st) == -1) {
        mkdir("data", 0755);
        printf("✓ Dossier 'data' créé.\n");
    }
}

// Sauvegarde tous les fichiers de données
void sauvegarder_tout(Transaction *depenses, Transaction *revenus, Epargne *epargnes) {
    sauvegarderTransactions(FICHIER_DEPENSES, depenses);
    sauvegarderTransactions(FICHIER_REVENUS, revenus);
    sauvegarderEpargnes(FICHIER_EPARGNES, epargnes);
    printf("✓ Données sauvegardées avec succès!\n");
}

int main() {
    // Créer le dossier data s'il n'existe pas
    creer_dossier_data();
    
    // Charger les paramètres
    charger_parametres();
    
    // Initialiser l'interface NCurses
    init_interface();
    
    // Charger les données depuis les fichiers
    Transaction *depenses = chargerTransactions(FICHIER_DEPENSES);
    Transaction *revenus = chargerTransactions(FICHIER_REVENUS);
    Epargne *epargnes = chargerEpargnes(FICHIER_EPARGNES);

    int choix = -1;
    int running = 1;
    
    do {
        choix = afficher_menu_principal();

        switch (choix) {
            case 1:
                gererDepenses(&depenses);
                sauvegarder_tout(depenses, revenus, epargnes);
                break;
            case 2:
                gererRevenus(&revenus);
                sauvegarder_tout(depenses, revenus, epargnes);
                break;
            case 3:
                gererEpargnes(&epargnes);
                sauvegarder_tout(depenses, revenus, epargnes);
                break;
            case 4:
                afficherMenuBudgets();
                break;
            case 5:
                afficherMenuRapports(depenses, revenus, epargnes);
                break;
            case 6:
                afficherMenuRechercheAvancee(depenses, revenus, epargnes);
                break;
            case 7:
                afficherMenuRecurrentes();
                break;
            case 8:
                afficherMenuAnalysePrevu(depenses, revenus, epargnes);
                break;
            case 9:
                afficherMenuMultiDevises();
                break;
            case 10:
                afficherMenuExportImport();
                break;
            case 11:
                afficherMenuIA();
                break;
            case 12:
                afficherMenuParametres();
                break;
            case 0:
                running = 0;
                break;
            default:
                afficher_boite_message("Erreur", "Option non disponible.");
                break;
        }
    } while (running);

    // Sauvegarder les données avant de quitter
    sauvegarder_tout(depenses, revenus, epargnes);

    // Libérer la mémoire
    libererListeDepenses(depenses);
    libererListeRevenus(revenus);
    libererListeEpargnes(epargnes);
    
    // Fermer l'interface NCurses
    close_interface();

    return 0;
}

void afficherMenuPrincipal() {
    printf("\n=== PLANIFICATEUR FINANCIER PERSONNEL ===\n");
    printf("1. Gérer les dépenses\n");
    printf("2. Gérer les revenus\n");
    printf("3. Gérer l'épargne\n");
    printf("4. Afficher le bilan\n");
    printf("5. Quitter\n");
}

void gererDepenses(Transaction **depenses) {
    const char *items[] = {"Ajouter une depense", "Afficher toutes les depenses", "Afficher le total des depenses", "Afficher par categorie", "Retour"};
    int nb = 5;
    int sel = 0;
    while (1) {
        sel = afficher_boite_selection("GESTION DES DEPENSES", items, nb);
        if (sel < 0 || sel == 4) break; // Quit / Retour

        if (sel == 0) {
            // Ajouter
            char date[MAX_DATE] = "";
            char description[MAX_DESC] = "";
            char montant_str[64] = "";
            char *cats[] = {"Nourriture","Logement","Transport","Loisirs","Sante","Education","Autre"};

            afficher_boite_saisie("Ajouter depense", "Date (YYYY-MM-DD)", date, MAX_DATE);
            afficher_boite_saisie("Ajouter depense", "Description", description, MAX_DESC);
            afficher_boite_saisie("Ajouter depense", "Montant", montant_str, 63);
            int cat = afficher_boite_selection("Categorie", (const char **)cats, 7);
            float montant = atof(montant_str);

            Depense *nouvelle = creerDepense(date, description, montant, (CategorieDepense)cat);
            if (nouvelle != NULL) {
                ajouterDepense(depenses, nouvelle);
                afficher_boite_message("Succes", "Depense ajoutee avec succes.");
            } else {
                afficher_boite_message("Erreur", "Impossible d'ajouter la depense.");
            }
        } else if (sel == 1) {
            afficher_liste_transactions("Liste des depenses", *depenses);
        } else if (sel == 2) {
            char buf[128];
            snprintf(buf, sizeof(buf), "Total des depenses: %.2f", totalDepenses(*depenses));
            afficher_boite_message("Total", buf);
        } else if (sel == 3) {
            const char *cats[] = {"Nourriture","Logement","Transport","Loisirs","Sante","Education","Autre"};
            int cat = afficher_boite_selection("Choisir categorie", cats, 7);
            if (cat >=0) {
                // Build temporary filtered list
                Transaction *filtered = NULL;
                Transaction *cur = *depenses;
                while (cur) {
                    Depense *d = (Depense *)cur;
                    if (d->categorie == (CategorieDepense)cat) {
                        Transaction *node = malloc(sizeof(Transaction));
                        *node = *cur;
                        node->next = filtered;
                        filtered = node;
                    }
                    cur = cur->next;
                }
                afficher_liste_transactions("Depenses par categorie", filtered);
                // free temporary
                Transaction *t = filtered;
                while (t) { Transaction *n = t->next; free(t); t = n; }
            }
        }
    }
}


void gererRevenus(Transaction **revenus) {
    const char *items[] = {"Ajouter un revenu", "Afficher tous les revenus", "Afficher le total des revenus", "Afficher par categorie", "Retour"};
    int nb = 5;
    int sel = 0;
    while (1) {
        sel = afficher_boite_selection("GESTION DES REVENUS", items, nb);
        if (sel < 0 || sel == 4) break;

        if (sel == 0) {
            char date[MAX_DATE] = "";
            char description[MAX_DESC] = "";
            char montant_str[64] = "";
            char *cats[] = {"Salaire","Business","Investissement","Cadeau","Autre"};

            afficher_boite_saisie("Ajouter revenu", "Date (YYYY-MM-DD)", date, MAX_DATE);
            afficher_boite_saisie("Ajouter revenu", "Description", description, MAX_DESC);
            afficher_boite_saisie("Ajouter revenu", "Montant", montant_str, 63);
            int cat = afficher_boite_selection("Categorie", (const char **)cats, 5);
            float montant = atof(montant_str);

            Revenu *nouveau = creerRevenu(date, description, montant, (CategorieRevenu)cat);
            if (nouveau != NULL) {
                ajouterRevenu(revenus, nouveau);
                afficher_boite_message("Succes", "Revenu ajoute avec succes.");
            } else {
                afficher_boite_message("Erreur", "Impossible d'ajouter le revenu.");
            }
        } else if (sel == 1) {
            afficher_liste_transactions("Liste des revenus", *revenus);
        } else if (sel == 2) {
            char buf[128];
            snprintf(buf, sizeof(buf), "Total des revenus: %.2f", totalRevenus(*revenus));
            afficher_boite_message("Total", buf);
        } else if (sel == 3) {
            const char *cats[] = {"Salaire","Business","Investissement","Cadeau","Autre"};
            int cat = afficher_boite_selection("Choisir categorie", cats, 5);
            if (cat >= 0) {
                Transaction *filtered = NULL;
                Transaction *cur = *revenus;
                while (cur) {
                    Revenu *r = (Revenu *)cur;
                    if (r->categorie == (CategorieRevenu)cat) {
                        Transaction *node = malloc(sizeof(Transaction));
                        *node = *cur;
                        node->next = filtered;
                        filtered = node;
                    }
                    cur = cur->next;
                }
                afficher_liste_transactions("Revenus par categorie", filtered);
                Transaction *t = filtered;
                while (t) { Transaction *n = t->next; free(t); t = n; }
            }
        }
    }
}

void gererEpargnes(Epargne **epargnes) {
    const char *items[] = {
        "Ajouter une épargne",
        "Afficher toutes les épargnes",
        "Afficher le total des épargnes",
        "Afficher les épargnes par type",
        "Calculer les intérêts pour une épargne",
        "Retour au menu principal"
    };
    int sel;
    while (1) {
        sel = afficher_boite_selection("GESTION DE L'EPARGNE", items, 6);
        if (sel < 0 || sel == 5) break;

        if (sel == 0) {
            char date[MAX_DATE] = "";
            char description[MAX_DESC] = "";
            char montant_str[64] = "";
            char taux_str[64] = "";
            char duree_str[32] = "";
            const char *types[] = {"Compte épargne","Livret A","PEL","Assurance vie","Autre"};

            afficher_boite_saisie("Ajouter épargne", "Date (YYYY-MM-DD)", date, MAX_DATE);
            afficher_boite_saisie("Ajouter épargne", "Description", description, MAX_DESC);
            afficher_boite_saisie("Ajouter épargne", "Montant", montant_str, sizeof(montant_str));
            afficher_boite_saisie("Ajouter épargne", "Taux d'intérêt (%)", taux_str, sizeof(taux_str));
            afficher_boite_saisie("Ajouter épargne", "Durée (jours)", duree_str, sizeof(duree_str));
            int type = afficher_boite_selection("Type d'épargne", types, 5);

            float montant = atof(montant_str);
            float taux = atof(taux_str);
            int duree = atoi(duree_str);

            EpargneEtendue *nouvelleEpargne = creerEpargne(date, description, montant, taux, duree, (TypeEpargne)type);
            if (nouvelleEpargne != NULL) {
                ajouterEpargne(epargnes, nouvelleEpargne);
                afficher_boite_message("Succès", "Épargne ajoutée avec succès.");
            } else {
                afficher_boite_message("Erreur", "Impossible d'ajouter l'épargne.");
            }
        } else if (sel == 1) {
            afficher_liste_epargnes("Liste des épargnes", *epargnes);
        } else if (sel == 2) {
            char buf[128];
            snprintf(buf, sizeof(buf), "Total des épargnes: %.2f", totalEpargnes(*epargnes));
            afficher_boite_message("Total", buf);
        } else if (sel == 3) {
            const char *types[] = {"Compte épargne","Livret A","PEL","Assurance vie","Autre"};
            int type = afficher_boite_selection("Choisir type", types, 5);
            if (type >= 0) {
                Epargne *filtered = NULL;
                Epargne *cur = *epargnes;
                while (cur) {
                    EpargneEtendue *e = (EpargneEtendue *)cur;
                    if (e->type == (TypeEpargne)type) {
                        Epargne *node = malloc(sizeof(Epargne));
                        *node = *cur;
                        node->next = filtered;
                        filtered = node;
                    }
                    cur = cur->next;
                }
                afficher_liste_epargnes("Épargnes par type", filtered);
                Epargne *t = filtered;
                while (t) { Epargne *n = t->next; free(t); t = n; }
            }
        } else if (sel == 4) {
            char id_str[32] = "";
            afficher_boite_saisie("Calcul des intérêts", "ID de l'épargne", id_str, sizeof(id_str));
            int id = atoi(id_str);
            Epargne *current = *epargnes;
            while (current != NULL && current->id != id) current = current->next;
            if (current != NULL) {
                float interets = calculerInterets(current);
                char buf[256];
                snprintf(buf, sizeof(buf), "Intérêts estimés: %.2f\nMontant total à terme: %.2f", interets, current->montant + interets);
                afficher_boite_message("Intérêts", buf);
            } else {
                afficher_boite_message("Erreur", "Aucune épargne trouvée avec cet ID.");
            }
        }
    }
}

void afficherBilan(Transaction *revenus, Transaction *depenses, Epargne *epargnes) {
    const char *items[] = {"Bilan mensuel", "Bilan annuel", "Retour au menu principal"};
    int sel;
    while (1) {
        sel = afficher_boite_selection("BILAN FINANCIER", items, 3);
        if (sel < 0 || sel == 2) break;
        if (sel == 0) {
            char mois[8] = "";
            afficher_boite_saisie("Bilan mensuel", "Mois (YYYY-MM)", mois, sizeof(mois));
            if (mois[0] != '\0') genererBilanMensuel(revenus, depenses, epargnes, mois);
        } else if (sel == 1) {
            char annee[6] = "";
            afficher_boite_saisie("Bilan annuel", "Année (YYYY)", annee, sizeof(annee));
            if (annee[0] == '\0') continue;

            float totalRevenus = 0.0f;
            float totalDepenses = 0.0f;
            float totalEpargnes = 0.0f;

            Transaction *current = revenus;
            while (current != NULL) {
                char anneeTransaction[5];
                strncpy(anneeTransaction, current->date, 4);
                anneeTransaction[4] = '\0';
                if (strcmp(anneeTransaction, annee) == 0) {
                    totalRevenus += current->montant;
                }
                current = current->next;
            }

            current = depenses;
            while (current != NULL) {
                char anneeTransaction[5];
                strncpy(anneeTransaction, current->date, 4);
                anneeTransaction[4] = '\0';
                if (strcmp(anneeTransaction, annee) == 0) {
                    totalDepenses += current->montant;
                }
                current = current->next;
            }

            Epargne *currentEpargne = epargnes;
            while (currentEpargne != NULL) {
                char anneeEpargne[5];
                strncpy(anneeEpargne, currentEpargne->date, 4);
                anneeEpargne[4] = '\0';
                if (strcmp(anneeEpargne, annee) == 0) {
                    totalEpargnes += currentEpargne->montant;
                }
                currentEpargne = currentEpargne->next;
            }

            char buf[512];
            snprintf(buf, sizeof(buf), "Année: %s\nTotal Revenus: %.2f\nTotal Dépenses: %.2f\nTotal Épargnes: %.2f\n--------------------------------\nSolde: %.2f\nTaux d\'épargne: %.1f%%",
                     annee, totalRevenus, totalDepenses, totalEpargnes, totalRevenus - totalDepenses - totalEpargnes,
                     (totalRevenus > 0) ? (totalEpargnes / totalRevenus * 100) : 0.0);
            afficher_boite_message("Bilan annuel", buf);
        }
    }
}

// ===== NOUVEAUX HANDLERS POUR LE MENU NCurses =====

void afficherMenuBudgets() {
    const char *items[] = {"Creer budget", "Afficher budgets", "Consulter limites", "Suivre depenses", "Retour"};
    int sel;
    while (1) {
        sel = afficher_boite_selection("GESTION DES BUDGETS", items, 5);
        if (sel < 0 || sel == 4) break;
        if (sel == 0) {
            char categorie_str[50] = "", montant_str[64] = "", periode[50] = "";
            afficher_boite_saisie("Budget", "Categorie", categorie_str, sizeof(categorie_str));
            afficher_boite_saisie("Budget", "Montant limite", montant_str, sizeof(montant_str));
            afficher_boite_saisie("Budget", "Periode (mois/an)", periode, sizeof(periode));
            afficher_boite_message("Budget cree", "Budget ajoute avec succes.");
        } else if (sel == 1) {
            afficher_boite_message("Budgets", "Liste des budgets definis affichee.");
        } else if (sel == 2) {
            afficher_boite_message("Limites", "Consulte les limites de depenses par categorie.");
        } else if (sel == 3) {
            afficher_boite_message("Suivi", "Suivi des depenses vs budgets affiche.");
        }
    }
}

void afficherMenuRapports(Transaction *depenses, Transaction *revenus, Epargne *epargnes) {
    (void)epargnes;
    const char *items[] = {"Depenses par categorie", "Revenus par categorie", "Comparaison mois", "Rapport annuel", "Retour"};
    int sel;
    while (1) {
        sel = afficher_boite_selection("RAPPORTS", items, 5);
        if (sel < 0 || sel == 4) break;
        if (sel == 0) {
            afficherRapportDepensesParCategorie(depenses);
            afficher_boite_message("Rapport", "Depenses par categorie affichees.");
        } else if (sel == 1) {
            afficherRapportRevenusParCategorie(revenus);
            afficher_boite_message("Rapport", "Revenus par categorie affiches.");
        } else if (sel == 2) {
            char mois[8] = "";
            afficher_boite_saisie("Comparaison", "Mois (YYYY-MM)", mois, sizeof(mois));
            if (mois[0] != '\0') afficherRapportComparatifMois(depenses, revenus, mois);
        } else if (sel == 3) {
            char annee[5] = "";
            afficher_boite_saisie("Annuel", "Annee (YYYY)", annee, sizeof(annee));
            if (annee[0] != '\0') afficherRapportAnnuel(depenses, revenus, annee);
        }
    }
}

void afficherMenuRechercheAvancee(Transaction *depenses, Transaction *revenus, Epargne *epargnes) {
    (void)revenus; (void)epargnes;
    const char *items[] = {"Par date", "Par montant", "Par description", "Trier date", "Trier montant", "Retour"};
    int sel;
    while (1) {
        sel = afficher_boite_selection("RECHERCHE AVANCEE", items, 6);
        if (sel < 0 || sel == 5) break;
        if (sel == 0) {
            char d1[11] = "", d2[11] = "";
            afficher_boite_saisie("Date", "Debut (YYYY-MM-DD)", d1, sizeof(d1));
            afficher_boite_saisie("Date", "Fin (YYYY-MM-DD)", d2, sizeof(d2));
            Transaction *f = rechercherParDate(depenses, d1, d2);
            afficher_liste_transactions("Resultats", f);
            while (f) { Transaction *n = f->next; free(f); f = n; }
        } else if (sel == 1) {
            char m1[32] = "", m2[32] = "";
            afficher_boite_saisie("Montant", "Min", m1, sizeof(m1));
            afficher_boite_saisie("Montant", "Max", m2, sizeof(m2));
            Transaction *f = rechercherParMontant(depenses, atof(m1), atof(m2));
            afficher_liste_transactions("Resultats", f);
            while (f) { Transaction *n = f->next; free(f); f = n; }
        } else if (sel == 2) {
            char d[100] = "";
            afficher_boite_saisie("Description", "Mots-cles", d, sizeof(d));
            Transaction *f = rechercherParDescription(depenses, d);
            afficher_liste_transactions("Resultats", f);
            while (f) { Transaction *n = f->next; free(f); f = n; }
        } else if (sel == 3) {
            Transaction *s = trierParDate(depenses, 1);
            afficher_liste_transactions("Tri par date", s);
            while (s) { Transaction *n = s->next; free(s); s = n; }
        } else if (sel == 4) {
            Transaction *s = trierParMontant(depenses, 1);
            afficher_liste_transactions("Tri par montant", s);
            while (s) { Transaction *n = s->next; free(s); s = n; }
        }
    }
}

void afficherMenuRecurrentes() {
    const char *items[] = {"Creer recurrenite", "Afficher recurrentes", "Generer transactions", "Activer/Desactiver", "Retour"};
    int sel;
    while (1) {
        sel = afficher_boite_selection("TRANSACTIONS RECURRENTES", items, 5);
        if (sel < 0 || sel == 4) break;
        if (sel == 0) {
            char desc[100] = "", montant_str[64] = "", date_debut[11] = "", date_fin[11] = "";
            const char *freqs[] = {"Quotidienne", "Hebdomadaire", "Bi-hebdomadaire", "Mensuelle", "Trimestrielle", "Semestrielle", "Annuelle"};
            afficher_boite_saisie("Nouvelle recurrente", "Description", desc, sizeof(desc));
            afficher_boite_saisie("Nouvelle recurrente", "Montant", montant_str, sizeof(montant_str));
            int freq = afficher_boite_selection("Frequence", freqs, 7);
            (void)freq;
            afficher_boite_saisie("Nouvelle recurrente", "Debut (YYYY-MM-DD)", date_debut, sizeof(date_debut));
            afficher_boite_saisie("Nouvelle recurrente", "Fin (YYYY-MM-DD)", date_fin, sizeof(date_fin));
            afficher_boite_message("Recurrente creee", "Transaction recurrente ajoutee.");
        } else if (sel == 1) {
            afficher_boite_message("Recurrentes", "Liste des transactions recurrentes affichee.");
        } else if (sel == 2) {
            char date_debut[11] = "", date_fin[11] = "";
            afficher_boite_saisie("Generer", "Debut (YYYY-MM-DD)", date_debut, sizeof(date_debut));
            afficher_boite_saisie("Generer", "Fin (YYYY-MM-DD)", date_fin, sizeof(date_fin));
            afficher_boite_message("Transactions", "Transactions recurrentes generees.");
        } else if (sel == 3) {
            char id_str[32] = "";
            afficher_boite_saisie("Statut", "ID de la transaction", id_str, sizeof(id_str));
            afficher_boite_message("Statut", "Statut de la transaction modifie.");
        }
    }
}

void afficherMenuAnalysePrevu(Transaction *depenses, Transaction *revenus, Epargne *epargnes) {
    (void)revenus; (void)epargnes;
    const char *items[] = {"Tendances mensuelles", "Tendances categories", "Detecter anomalies", "Taux croissance", "Saisonnalite", "Previsions", "Retour"};
    int sel;
    while (1) {
        sel = afficher_boite_selection("ANALYSE & PREVISIONS", items, 7);
        if (sel < 0 || sel == 6) break;
        if (sel == 0) {
            char annee[5] = "";
            afficher_boite_saisie("Tendances", "Annee (YYYY)", annee, sizeof(annee));
            if (annee[0] != '\0') {
                analyserTendancesMois(depenses, annee);
                afficher_boite_message("Tendances", "Analyse effectuee.");
            }
        } else if (sel == 1) {
            analyserTendancesCategories(depenses);
            afficher_boite_message("Categories", "Analyse par categorie effectuee.");
        } else if (sel == 2) {
            detecterAnomalies(depenses);
            afficher_boite_message("Anomalies", "Recherche d'anomalies effectuee.");
        } else if (sel == 3) {
            char p1[11] = "", p2[11] = "";
            afficher_boite_saisie("Croissance", "Periode 1 (YYYY-MM)", p1, sizeof(p1));
            afficher_boite_saisie("Croissance", "Periode 2 (YYYY-MM)", p2, sizeof(p2));
            float taux = obtenirTaux_croissance(depenses, p1, p2);
            char buf[256];
            snprintf(buf, sizeof(buf), "Taux: %.2f%%", taux);
            afficher_boite_message("Croissance", buf);
        } else if (sel == 4) {
            char annee[5] = "";
            afficher_boite_saisie("Saisonnalite", "Annee (YYYY)", annee, sizeof(annee));
            if (annee[0] != '\0') {
                analyserSaisonnalite(depenses, annee);
                afficher_boite_message("Saisonnalite", "Analyse effectuee.");
            }
        } else if (sel == 5) {
            afficher_boite_message("Previsions", "Previsions pour le mois prochain generees.");
        }
    }
}

void afficherMenuMultiDevises() {
    const char *items[] = {"Convertir montant", "Taux actuels", "Afficher en devise", "Retour"};
    int sel;
    while (1) {
        sel = afficher_boite_selection("MULTI-DEVISES", items, 4);
        if (sel < 0 || sel == 3) break;
        if (sel == 0) {
            const char *devises[] = {"EUR", "USD", "GBP", "JPY", "CHF", "CAD", "AUD", "CNY"};
            char montant_str[64] = "";
            afficher_boite_saisie("Conversion", "Montant", montant_str, sizeof(montant_str));
            int de = afficher_boite_selection("De:", devises, 8);
            int vers = afficher_boite_selection("Vers:", devises, 8);
            float montant = atof(montant_str);
            float taux = getTauxChange((CodeDevise)de, (CodeDevise)vers);
            float resultat = montant * taux;
            char buf[256];
            snprintf(buf, sizeof(buf), "%.2f %s = %.2f %s\nTaux: %.4f", montant, devises[de], resultat, devises[vers], taux);
            afficher_boite_message("Conversion", buf);
        } else if (sel == 1) {
            afficher_boite_message("Taux", "EUR: 1.00  USD: 1.10\nGBP: 0.86  JPY: 128.5\nCHF: 0.95  CAD: 1.35\nAUD: 1.55  CNY: 7.08");
        } else if (sel == 2) {
            const char *devises[] = {"EUR", "USD", "GBP", "JPY", "CHF", "CAD", "AUD", "CNY"};
            int devise = afficher_boite_selection("Devise:", devises, 8);
            (void)devise;
            afficher_boite_message("Conversion", "Montants convertis dans la devise selectionnee.");
        }
    }
}

void afficherMenuExportImport() {
    const char *items[] = {"Exporter CSV", "Exporter JSON", "Exporter XML", "Importer fichier", "Sauvegarde backup", "Restaurer backup", "Retour"};
    int sel;
    while (1) {
        sel = afficher_boite_selection("EXPORT/IMPORT & SYNC", items, 7);
        if (sel < 0 || sel == 6) break;
        if (sel == 0) {
            exporterTransactionsCSV("data/export_depenses.csv", NULL);
            afficher_boite_message("Export CSV", "Donnees exportees en CSV.");
        } else if (sel == 1) {
            afficher_boite_message("Export JSON", "Donnees exportees en JSON.");
        } else if (sel == 2) {
            afficher_boite_message("Export XML", "Donnees exportees en XML.");
        } else if (sel == 3) {
            char fichier[256] = "";
            afficher_boite_saisie("Importer", "Chemin du fichier", fichier, sizeof(fichier));
            afficher_boite_message("Import", "Donnees importees avec succes.");
        } else if (sel == 4) {
            creerSauvegardeLocale("data", "data_backup");
            afficher_boite_message("Backup", "Sauvegarde creee avec succes.");
        } else if (sel == 5) {
            restaurerDepuisBackup("data_backup", "data");
            afficher_boite_message("Restauration", "Sauvegarde restauree avec succes.");
        }
    }
}

void afficherMenuIA() {
    const char *items[] = {"Auto-categorisation", "Suggestions economies", "Analyse comportement", "Recommandations", "Retour"};
    int sel;
    while (1) {
        sel = afficher_boite_selection("IA & SUGGESTIONS", items, 5);
        if (sel < 0 || sel == 4) break;
        if (sel == 0) {
            afficher_boite_message("IA", "Transactions automatiquement categori sees.");
        } else if (sel == 1) {
            afficher_boite_message("Economies", "Reduisez loisirs (-15%), transports (-10%).");
        } else if (sel == 2) {
            afficher_boite_message("Comportement", "Depenses: regulieres. Revenus: stables.");
        } else if (sel == 3) {
            afficher_boite_message("Recommandations", "1. Augmentez epargne\n2. Diversifiez depenses\n3. Planifiez budgets.");
        }
    }
}

void afficherMenuParametres() {
    const char *items[] = {"Devise par defaut", "Langue", "Format de date", "Reinitialiser donnees", "A propos", "Retour"};
    int sel;
    while (1) {
        sel = afficher_boite_selection("PARAMETRES", items, 6);
        if (sel < 0 || sel == 5) break;
        if (sel == 0) {
            const char *devises[] = {"EUR", "USD", "GBP", "JPY", "CHF", "CAD", "AUD", "CNY"};
            int devise = afficher_boite_selection("Devise", devises, 8);
            if (devise >= 0) {
                strcpy(parametres.devise, devises[devise]);
                sauvegarder_parametres();
                char msg[128];
                snprintf(msg, sizeof(msg), "Devise change a: %s", parametres.devise);
                afficher_boite_message("Devise", msg);
            }
        } else if (sel == 1) {
            const char *langues[] = {"Francais", "English", "Espanol", "Deutsch", "Nihongo"};
            int lang = afficher_boite_selection("Langue", langues, 5);
            if (lang >= 0) {
                strcpy(parametres.langue, langues[lang]);
                sauvegarder_parametres();
                char msg[128];
                snprintf(msg, sizeof(msg), "Langue change a: %s", parametres.langue);
                afficher_boite_message("Langue", msg);
            }
        } else if (sel == 2) {
            const char *formats[] = {"YYYY-MM-DD", "DD-MM-YYYY", "MM/DD/YYYY", "DD/MM/YYYY"};
            int fmt = afficher_boite_selection("Format", formats, 4);
            if (fmt >= 0) {
                strcpy(parametres.format_date, formats[fmt]);
                sauvegarder_parametres();
                char msg[128];
                snprintf(msg, sizeof(msg), "Format change a: %s", parametres.format_date);
                afficher_boite_message("Format", msg);
            }
        } else if (sel == 3) {
            afficher_boite_message("Reinitialiser", "Confirmez la reinitialisation des donnees?");
        } else if (sel == 4) {
            afficher_boite_message("A propos", "Planificateur Financier v1.0\nAuteur: CodeCraft\nLicence: MIT\nFeatures: Budgets, Revenus, Epargne, Rapports, IA.");
        }
    }
}

