#ifndef INTERFACE_H
#define INTERFACE_H

#include <ncurses.h>
#include "database.h"

// Constantes
#define MAX_MENU_ITEMS 15
#define BORDER_CHAR '|'
#define COLOR_NORMAL 1
#define COLOR_HIGHLIGHT 2
#define COLOR_ERROR 3
#define COLOR_SUCCESS 4
// Couleurs additionnelles pour interface vive
#define COLOR_TITLE 5
#define COLOR_MENU 6
#define COLOR_EMOJI 7

// Énumérations
typedef enum {
    MENU_PRINCIPAL,
    MENU_TRANSACTIONS,
    MENU_REVENUS,
    MENU_DEPENSES,
    MENU_EPARGNES,
    MENU_BUDGET,
    MENU_RAPPORTS,
    MENU_RECHERCHE,
    MENU_PARAMETRES,
    MENU_EXIT
} MenuState;

// Structure pour les items de menu
typedef struct {
    char label[50];
    int key;
} MenuItem;

// Prototypes des fonctions
void init_interface();
void close_interface();
int afficher_menu_principal();
void afficher_boite_message(const char *titre, const char *message);
void afficher_boite_saisie(const char *titre, const char *label, char *buffer, int max_len);
int afficher_boite_selection(const char *titre, const char *items[], int nb_items);
void afficher_bordure(int x, int y, int largeur, int hauteur);
void afficher_titre(const char *titre);
void afficher_pied_page(const char *message);

// Affichage de listes et formulaires NCurses
void afficher_liste_transactions(const char *titre, Transaction *tete);
void afficher_liste_epargnes(const char *titre, Epargne *tete);
int afficher_menu_local(const char *titre, const char *items[], int nb_items);
// Utilitaires d'affichage
void effacer_zone(int x, int y, int largeur, int hauteur);
void afficher_centree(int y, const char *texte);
int get_max_x();
int get_max_y();

// Formulaires
typedef struct {
    char champ[20];
    char valeur[256];
} FormField;

int afficher_formulaire(const char *titre, FormField *champs, int nb_champs);
int afficher_menu_selection(const char *titre, MenuItem *items, int nb_items);

// Panels et fenêtres
WINDOW *creer_fenetre(int hauteur, int largeur, int starty, int startx, const char *titre);
void detruire_fenetre(WINDOW *win);

// Gestion des couleurs
void init_couleurs();

#endif
