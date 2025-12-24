#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

static int max_x, max_y;

// Initialise l'interface NCurses
void init_interface() {
    // Activer la locale pour permettre les emojis/UTF-8
    setlocale(LC_ALL, "");

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0); // Masquer le curseur
    
    // Obtenir les dimensions
    getmaxyx(stdscr, max_y, max_x);
    
    // Initialiser les couleurs
    if (has_colors()) {
        start_color();
        init_couleurs();
    }
    
    clear();
    refresh();
}

// Ferme l'interface
void close_interface() {
    endwin();
}

// Initialise les couleurs (désactivé)
void init_couleurs() {
    // Pas de couleurs
}

// Obtient la largeur maximale
int get_max_x() {
    return max_x;
}

// Obtient la hauteur maximale
int get_max_y() {
    return max_y;
}

// Affiche une bordure
void afficher_bordure(int x, int y, int largeur, int hauteur) {
    // Coins
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + largeur - 1, ACS_URCORNER);
    mvaddch(y + hauteur - 1, x, ACS_LLCORNER);
    mvaddch(y + hauteur - 1, x + largeur - 1, ACS_LRCORNER);
    
    // Lignes horizontales
    for (int i = x + 1; i < x + largeur - 1; i++) {
        mvaddch(y, i, ACS_HLINE);
        mvaddch(y + hauteur - 1, i, ACS_HLINE);
    }
    
    // Lignes verticales
    for (int i = y + 1; i < y + hauteur - 1; i++) {
        mvaddch(i, x, ACS_VLINE);
        mvaddch(i, x + largeur - 1, ACS_VLINE);
    }
    
    refresh();
}

// Affiche un titre centré
void afficher_titre(const char *titre) {
    attron(A_BOLD);
    afficher_centree(1, titre);
    attroff(A_BOLD);
}

// Affiche un pied de page
void afficher_pied_page(const char *message) {
    mvprintw(max_y - 1, 1, "%-*s", max_x - 2, message);
    refresh();
}

// Efface une zone
void effacer_zone(int x, int y, int largeur, int hauteur) {
    for (int i = y; i < y + hauteur; i++) {
        for (int j = x; j < x + largeur; j++) {
            mvaddch(i, j, ' ');
        }
    }
    refresh();
}

// Affiche du texte centré
void afficher_centree(int y, const char *texte) {
    int x = (max_x - strlen(texte)) / 2;
    if (x < 0) x = 0;
    mvprintw(y, x, "%s", texte);
    refresh();
}

WINDOW *creer_fenetre(int hauteur, int largeur, int starty, int startx, const char *titre) {
    WINDOW *win = newwin(hauteur, largeur, starty, startx);
    
    // Dessiner bordure simple avec ASCII
    for (int i = 0; i < largeur; i++) {
        mvwaddch(win, 0, i, '-');
        mvwaddch(win, hauteur-1, i, '-');
    }
    for (int i = 0; i < hauteur; i++) {
        mvwaddch(win, i, 0, '|');
        mvwaddch(win, i, largeur-1, '|');
    }
    mvwaddch(win, 0, 0, '+');
    mvwaddch(win, 0, largeur-1, '+');
    mvwaddch(win, hauteur-1, 0, '+');
    mvwaddch(win, hauteur-1, largeur-1, '+');

    if (titre != NULL) {
        wattron(win, A_BOLD);
        mvwprintw(win, 0, 2, " %s ", titre);
        wattroff(win, A_BOLD);
    }

    wrefresh(win);
    return win;
}

// Détruit une fenêtre
void detruire_fenetre(WINDOW *win) {
    delwin(win);
    refresh();
}

// Affiche une boîte de message
void afficher_boite_message(const char *titre, const char *message) {
    clear();
    refresh();
    int largeur = 60;
    int hauteur = 10;
    int startx = (max_x - largeur) / 2;
    int starty = (max_y - hauteur) / 2;
    
    WINDOW *win = creer_fenetre(hauteur, largeur, starty, startx, titre);
    
    // Afficher le message
    int msg_y = 2;
    const char *ptr = message;
    int line = 0;
    
    while (*ptr != '\0' && line < hauteur - 4) {
        char line_text[58];
        int i = 0;
        
        while (*ptr != '\0' && *ptr != '\n' && i < 57) {
            line_text[i++] = *ptr++;
        }
        
        line_text[i] = '\0';
        mvwprintw(win, msg_y + line, 2, "%s", line_text);
        
        if (*ptr == '\n') ptr++;
        line++;
    }
    
    // Bouton OK
    mvwprintw(win, hauteur - 2, (largeur - 4) / 2, "[ OK ]");
    wrefresh(win);
    
    // Attendre une touche
    wgetch(win);
    delwin(win);
    clear();
    refresh();
}

// Affiche une boîte de saisie
void afficher_boite_saisie(const char *titre, const char *label, char *buffer, int max_len __attribute__((unused))) {
    clear();
    refresh();
    int largeur = 60;
    int hauteur = 10;
    int startx = (max_x - largeur) / 2;
    int starty = (max_y - hauteur) / 2;
    
    WINDOW *win = creer_fenetre(hauteur, largeur, starty, startx, titre);
    
    // Afficher le label
    mvwprintw(win, 2, 2, "%s: ", label);
    
    // Zone de saisie
    mvwprintw(win, 3, 2, "[");
    mvwprintw(win, 3, largeur - 3, "]");
    
    // Activer l'écho et le curseur pour la saisie
    echo();
    curs_set(1);
    wmove(win, 3, 3);
    wgetstr(win, buffer);
    curs_set(0);
    noecho();
    
    delwin(win);
    clear();
    refresh();
}

// Affiche une boîte de sélection
int afficher_boite_selection(const char *titre, const char *items[], int nb_items) {
    int largeur = 50;
    int hauteur = nb_items + 6;
    if (hauteur > max_y - 4) hauteur = max_y - 4;
    
    int startx = (max_x - largeur) / 2;
    int starty = (max_y - hauteur) / 2;
    
    WINDOW *win = creer_fenetre(hauteur, largeur, starty, startx, titre);
    
    keypad(win, TRUE);
    int selection = 0;
    int running = 1;
    
    while (running) {
        // Afficher les items
        for (int i = 0; i < nb_items; i++) {
            char display[256];
            snprintf(display, sizeof(display), "  %s", items[i]);
            if (i == selection) {
                wattron(win, A_BOLD);
                mvwprintw(win, 3 + i, 3, "> %s", display+2);
                wattroff(win, A_BOLD);
            } else {
                mvwprintw(win, 3 + i, 3, "  %s", display+2);
            }
        }
        
        // Footer hint
        mvwprintw(win, hauteur - 2, 2, "UP/DOWN: Navigate | ENTER: Select | q: Quit");
        wrefresh(win);
        
        int ch = wgetch(win);
        switch (ch) {
            case KEY_UP:
                selection = (selection - 1 + nb_items) % nb_items;
                break;
            case KEY_DOWN:
                selection = (selection + 1) % nb_items;
                break;
            case 'q':
            case 'Q':
                selection = -1;
                running = 0;
                break;
            case '\n':
                running = 0;
                break;
        }
    }
    
    delwin(win);
    clear();
    refresh();
    return (selection >= 0) ? selection : -1;
}

// Affiche une liste de transactions avec pagination
void afficher_liste_transactions(const char *titre, Transaction *tete) {
    clear();
    refresh();
    int largeur = max_x - 10;
    int hauteur = max_y - 8;
    int startx = (max_x - largeur) / 2;
    int starty = (max_y - hauteur) / 2;

    WINDOW *win = creer_fenetre(hauteur, largeur, starty, startx, titre);

    // Collect lines
    char **lines = malloc(sizeof(char*));
    int count = 0;
    Transaction *cur = tete;
    while (cur) {
        char buf[512];
        snprintf(buf, sizeof(buf), "%d | %s | %-20s | %.2f", cur->id, cur->date, cur->description, cur->montant);
        char *dup = malloc(strlen(buf) + 1);
        strcpy(dup, buf);
        lines = realloc(lines, sizeof(char*) * (count + 1));
        lines[count] = dup;
        count++;
        cur = cur->next;
    }

    int per_page = hauteur - 6;
    int page = 0;
    int total_pages = (count + per_page -1) / per_page;
    int ch;

    keypad(win, TRUE);
    while (1) {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 1, 2, "%s", titre);
        int start = page * per_page;
        for (int i = 0; i < per_page && start + i < count; ++i) {
            mvwprintw(win, 3 + i, 2, "%s", lines[start + i]);
        }
        mvwprintw(win, hauteur-2, 2, "Page %d/%d - q: Quit | Left/Right: Pages", page+1, total_pages>0?total_pages:1);
        wrefresh(win);
        ch = wgetch(win);
        if (ch == 'q' || ch == 'Q' || ch == 27) break;
        if (ch == KEY_RIGHT) page = (page +1) % (total_pages>0?total_pages:1);
        if (ch == KEY_LEFT) page = (page -1 + (total_pages>0?total_pages:1)) % (total_pages>0?total_pages:1);
    }

    // free
    for (int i=0;i<count;i++) free(lines[i]);
    free(lines);
    delwin(win);
    clear();
    refresh();
}

// Affiche une liste d'épargnes
void afficher_liste_epargnes(const char *titre, Epargne *tete) {
    clear();
    refresh();
    int largeur = max_x - 10;
    int hauteur = max_y - 8;
    int startx = (max_x - largeur) / 2;
    int starty = (max_y - hauteur) / 2;

    WINDOW *win = creer_fenetre(hauteur, largeur, starty, startx, titre);

    // Collect lines
    char **lines = malloc(sizeof(char*));
    int count = 0;
    Epargne *cur = tete;
    while (cur) {
        char buf[512];
        snprintf(buf, sizeof(buf), "%d | %s | %-20s | %.2f | taux %.2f%%", cur->id, cur->date, cur->description, cur->montant, cur->taux_interet);
        char *dup = malloc(strlen(buf) + 1);
        strcpy(dup, buf);
        lines = realloc(lines, sizeof(char*) * (count + 1));
        lines[count] = dup;
        count++;
        cur = cur->next;
    }

    int per_page = hauteur - 6;
    int page = 0;
    int total_pages = (count + per_page -1) / per_page;
    int ch;

    keypad(win, TRUE);
    while (1) {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 1, 2, "%s", titre);
        int start = page * per_page;
        for (int i = 0; i < per_page && start + i < count; ++i) {
            mvwprintw(win, 3 + i, 2, "%s", lines[start + i]);
        }
        mvwprintw(win, hauteur-2, 2, "Page %d/%d - q: Quit | Left/Right: Pages", page+1, total_pages>0?total_pages:1);
        wrefresh(win);
        ch = wgetch(win);
        if (ch == 'q' || ch == 'Q' || ch == 27) break;
        if (ch == KEY_RIGHT) page = (page +1) % (total_pages>0?total_pages:1);
        if (ch == KEY_LEFT) page = (page -1 + (total_pages>0?total_pages:1)) % (total_pages>0?total_pages:1);
    }

    // free
    for (int i=0;i<count;i++) free(lines[i]);
    free(lines);
    delwin(win);
    clear();
    refresh();
}
// Affiche un menu avec sélection
int afficher_menu_selection(const char *titre, MenuItem *items, int nb_items) {
    clear();
    refresh();
    int largeur = 50;
    int hauteur = nb_items + 6;
    if (hauteur > max_y - 4) hauteur = max_y - 4;
    
    int startx = (max_x - largeur) / 2;
    int starty = (max_y - hauteur) / 2;
    
    WINDOW *win = creer_fenetre(hauteur, largeur, starty, startx, titre);
    
    keypad(win, TRUE);
    int selection = 0;
    int running = 1;
    
    while (running) {
        // Afficher les items
        for (int i = 0; i < nb_items; i++) {
            // show key at right
            int keypos = largeur - 6;
            if (i == selection) {
                wattron(win, A_BOLD);
                mvwprintw(win, 3 + i, 3, "> %s", items[i].label);
                mvwprintw(win, 3 + i, keypos, "[%d]", items[i].key);
                wattroff(win, A_BOLD);
            } else {
                mvwprintw(win, 3 + i, 3, "  %s", items[i].label);
                mvwprintw(win, 3 + i, keypos, " %d ", items[i].key);
            }
        }
        
        mvwprintw(win, hauteur - 2, 2, "UP/DOWN: Navigate | ENTER: Select | q: Quit");
        wrefresh(win);
        
        int ch = wgetch(win);
        switch (ch) {
            case KEY_UP:
                selection = (selection - 1 + nb_items) % nb_items;
                break;
            case KEY_DOWN:
                selection = (selection + 1) % nb_items;
                break;
            case 'q':
            case 'Q':
                // return a special key (0) for quit
                delwin(win);
                clear();
                refresh();
                return 0;
            case '\n':
                running = 0;
                break;
        }
    }
    
    delwin(win);
    clear();
    refresh();
    return items[selection].key;
}

// Affiche un formulaire
int afficher_formulaire(const char *titre, FormField *champs, int nb_champs) {
    clear();
    refresh();
    int largeur = 60;
    int hauteur = nb_champs + 8;
    if (hauteur > max_y - 4) hauteur = max_y - 4;
    
    int startx = (max_x - largeur) / 2;
    int starty = (max_y - hauteur) / 2;
    
    WINDOW *win = creer_fenetre(hauteur, largeur, starty, startx, titre);
    
    // Saisie des champs
    for (int i = 0; i < nb_champs; i++) {
        echo();
        curs_set(1);
        
        mvwprintw(win, 2 + i * 2, 2, "%s: ", champs[i].champ);
        mvwprintw(win, 3 + i * 2, 2, "[");
        mvwprintw(win, 3 + i * 2, largeur - 3, "]");
        
        wmove(win, 3 + i * 2, 3);
        wgetstr(win, champs[i].valeur);
    }
    
    curs_set(0);
    noecho();
    
    mvwprintw(win, hauteur - 2, (largeur - 20) / 2, "[Valider] [Annuler]");
    wrefresh(win);
    
    wgetch(win);
    delwin(win);
    refresh();
    
    return 1;
}

// Affiche le menu principal
int afficher_menu_principal() {
    clear();
    
    // En-tête simple et lisible
    attron(A_BOLD);
    afficher_centree(1, "");
    afficher_centree(2, "========================================");
    afficher_centree(3, "   PLANIFICATEUR FINANCIER PERSONNEL   ");
    afficher_centree(4, "========================================");
    attroff(A_BOLD);
    
    afficher_centree(5, "");

    MenuItem menu[] = {
        {"[1] Gerer les depenses", 1},
        {"[2] Gerer les revenus", 2},
        {"[3] Gerer l'epargne", 3},
        {"[4] Gerer les budgets", 4},
        {"[5] Afficher rapports", 5},
        {"[6] Recherche avancee", 6},
        {"[7] Transactions recurrentes", 7},
        {"[8] Analyse & Previsions", 8},
        {"[9] Multi-devises", 9},
        {"[10] Export/Import & Sync", 10},
        {"[11] IA & Suggestions", 11},
        {"[12] Parametres", 12},
        {"[0] QUITTER", 0}
    };

    return afficher_menu_selection("MENU PRINCIPAL", menu, 13);
}
