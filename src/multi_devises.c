#include "multi_devises.h"
#include <stdio.h>
#include <stdlib.h>

// Taux de change (base EUR = 1.0)
TauxChange tauxChanges[8] = {
    {EUR, USD, 1.10},
    {EUR, GBP, 0.86},
    {EUR, JPY, 155.00},
    {EUR, CHF, 0.96},
    {EUR, CAD, 1.47},
    {EUR, AUD, 1.67},
    {EUR, CNY, 7.96}
};

int nbTauxChanges = 7;

// Retourne le code string d'une devise
const char *getCodeDevise(CodeDevise devise) {
    switch (devise) {
        case EUR: return "EUR";
        case USD: return "USD";
        case GBP: return "GBP";
        case JPY: return "JPY";
        case CHF: return "CHF";
        case CAD: return "CAD";
        case AUD: return "AUD";
        case CNY: return "CNY";
        default: return "UNKNOWN";
    }
}

// Retourne la devise à partir du code string
CodeDevise getDeviseFromCode(const char *code) {
    if (strcmp(code, "EUR") == 0) return EUR;
    if (strcmp(code, "USD") == 0) return USD;
    if (strcmp(code, "GBP") == 0) return GBP;
    if (strcmp(code, "JPY") == 0) return JPY;
    if (strcmp(code, "CHF") == 0) return CHF;
    if (strcmp(code, "CAD") == 0) return CAD;
    if (strcmp(code, "AUD") == 0) return AUD;
    if (strcmp(code, "CNY") == 0) return CNY;
    return EUR;
}

// Obtient le taux de change entre deux devises
float getTauxChange(CodeDevise from, CodeDevise to) {
    if (from == to) return 1.0;
    
    // Si la première devise est EUR
    if (from == EUR) {
        for (int i = 0; i < nbTauxChanges; i++) {
            if (tauxChanges[i].from == EUR && tauxChanges[i].to == to) {
                return tauxChanges[i].taux;
            }
        }
    }
    
    // Si la deuxième devise est EUR
    if (to == EUR) {
        for (int i = 0; i < nbTauxChanges; i++) {
            if (tauxChanges[i].from == EUR && tauxChanges[i].to == from) {
                return 1.0 / tauxChanges[i].taux;
            }
        }
    }
    
    // Conversion via EUR comme devise intermédiaire
    float tauxFromEur = getTauxChange(from, EUR);
    float tauxToEur = getTauxChange(EUR, to);
    return tauxFromEur * tauxToEur;
}

// Convertit un montant d'une devise à une autre
float convertirDevise(float montant, CodeDevise from, CodeDevise to) {
    float taux = getTauxChange(from, to);
    return montant * taux;
}

// Définit un nouveau taux de change
void definirTauxChange(CodeDevise from, CodeDevise to, float taux) {
    // Chercher si le taux existe déjà
    for (int i = 0; i < nbTauxChanges; i++) {
        if (tauxChanges[i].from == from && tauxChanges[i].to == to) {
            tauxChanges[i].taux = taux;
            return;
        }
    }
    
    // Si on ne le trouve pas, on l'ajoute
    if (nbTauxChanges < 8) {
        tauxChanges[nbTauxChanges].from = from;
        tauxChanges[nbTauxChanges].to = to;
        tauxChanges[nbTauxChanges].taux = taux;
        nbTauxChanges++;
    }
}

// Affiche toutes les devises disponibles
void afficherDevises() {
    printf("\n=== DEVISES DISPONIBLES ===\n");
    printf("0 - EUR (Euro)\n");
    printf("1 - USD (Dollar américain)\n");
    printf("2 - GBP (Livre sterling)\n");
    printf("3 - JPY (Yen japonais)\n");
    printf("4 - CHF (Franc suisse)\n");
    printf("5 - CAD (Dollar canadien)\n");
    printf("6 - AUD (Dollar australien)\n");
    printf("7 - CNY (Yuan chinois)\n");
}
