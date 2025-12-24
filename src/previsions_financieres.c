#include "previsions_financieres.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Calcule la moyenne simple sur les N dernières périodes
float calculerMoyenneSimple(Transaction *tete, int nb_periodes) {
    float total = 0;
    int count = 0;

    Transaction *current = tete;
    while (current != NULL && count < nb_periodes) {
        total += current->montant;
        count++;
        current = current->next;
    }

    return (count > 0) ? (total / count) : 0;
}

// Calcule la moyenne exponentielle
float calculerMoyenneExponentielle(Transaction *tete, float alpha) {
    if (alpha < 0 || alpha > 1) alpha = 0.3; // Valeur par défaut

    float ema = 0;
    int first = 1;

    Transaction *current = tete;
    while (current != NULL) {
        if (first) {
            ema = current->montant;
            first = 0;
        } else {
            ema = alpha * current->montant + (1 - alpha) * ema;
        }
        current = current->next;
    }

    return ema;
}

// Calcule la tendance linéaire
float calculerTendanceLineaire(Transaction *tete, int *pente) {
    if (tete == NULL) return 0;

    int n = 0;
    float somme_x = 0, somme_y = 0, somme_xy = 0, somme_x2 = 0;

    Transaction *current = tete;
    while (current != NULL) {
        somme_y += current->montant;
        somme_x += n;
        somme_xy += n * current->montant;
        somme_x2 += n * n;
        n++;
        current = current->next;
    }

    if (n == 0) return 0;

    float a = (n * somme_xy - somme_x * somme_y) / (n * somme_x2 - somme_x * somme_x);
    float b = (somme_y - a * somme_x) / n;

    if (pente) *pente = (int)a;
    return b;
}

// Prévision par saisonnalité
float previsionParSaisonnalite(Transaction *tete) {
    // Calcule la moyenne saisonnière
    float t1 = 0, t2 = 0, t3 = 0, t4 = 0;
    int c1 = 0, c2 = 0, c3 = 0, c4 = 0;

    Transaction *current = tete;
    while (current != NULL) {
        int mois = atoi(current->date + 5) - 1;

        if (mois >= 0 && mois < 3) { t1 += current->montant; c1++; }
        else if (mois >= 3 && mois < 6) { t2 += current->montant; c2++; }
        else if (mois >= 6 && mois < 9) { t3 += current->montant; c3++; }
        else if (mois >= 9 && mois < 12) { t4 += current->montant; c4++; }

        current = current->next;
    }

    float moy_globale = (t1 + t2 + t3 + t4) / (c1 + c2 + c3 + c4 + 1);
    return moy_globale;
}

// Prévision pour le mois prochain
PrevisionMontant previsionMois(Transaction *tete, ModeleProvision modele) {
    PrevisionMontant prevision = {0, 0, 0, 0.8, 75};

    switch (modele) {
        case MODELE_SIMPLE:
            prevision.montant_prevu = calculerMoyenneSimple(tete, 3);
            break;
        case MODELE_EXPONENTIEL:
            prevision.montant_prevu = calculerMoyenneExponentielle(tete, 0.3);
            break;
        case MODELE_TENDANCE: {
            int pente = 0;
            prevision.montant_prevu = calculerTendanceLineaire(tete, &pente);
            if (pente != 0) prevision.montant_prevu += pente;
            break;
        }
        case MODELE_SAISONNIER:
            prevision.montant_prevu = previsionParSaisonnalite(tete);
            break;
        default:
            prevision.montant_prevu = calculerMoyenneSimple(tete, 3);
    }

    // Calculer l'intervalle de confiance (approximatif)
    float ecart = prevision.montant_prevu * 0.15; // 15% d'écart
    prevision.intervalle_confiance_bas = prevision.montant_prevu - ecart;
    prevision.intervalle_confiance_haut = prevision.montant_prevu + ecart;

    return prevision;
}

// Prévision pour le trimestre prochain
PrevisionMontant previsionTrimestre(Transaction *tete, ModeleProvision modele) {
    PrevisionMontant prevision = previsionMois(tete, modele);
    prevision.montant_prevu *= 3; // Multiplication par 3 pour le trimestre
    prevision.intervalle_confiance_bas *= 3;
    prevision.intervalle_confiance_haut *= 3;
    return prevision;
}

// Prévision pour l'année prochaine
PrevisionMontant previsionAnnee(Transaction *tete, ModeleProvision modele) {
    PrevisionMontant prevision = previsionMois(tete, modele);
    prevision.montant_prevu *= 12; // Multiplication par 12 pour l'année
    prevision.intervalle_confiance_bas *= 12;
    prevision.intervalle_confiance_haut *= 12;
    prevision.fiabilite = 60; // Moins fiable pour une année complète
    return prevision;
}

// Prévision du solde
float previsionSolde(Transaction *revenus, Transaction *depenses, int mois) {
    PrevisionMontant prev_revenus = previsionMois(revenus, MODELE_SIMPLE);
    PrevisionMontant prev_depenses = previsionMois(depenses, MODELE_SIMPLE);
    return (prev_revenus.montant_prevu - prev_depenses.montant_prevu) * mois;
}

// Affiche les prévisions
void afficherPrevisions(PrevisionMontant prevision, const char *periode) {
    printf("\n=== PRÉVISIONS: %s ===\n", periode);
    printf("Montant prévu:              %.2f €\n", prevision.montant_prevu);
    printf("Intervalle de confiance:    [%.2f € - %.2f €]\n",
           prevision.intervalle_confiance_bas,
           prevision.intervalle_confiance_haut);
    printf("Probabilité de confiance:   %.0f%%\n", prevision.probabilite_confiance * 100);
    printf("Fiabilité de la prévision:  %d%%\n", prevision.fiabilite);
}

// Affiche l'intervalle de confiance
void afficherIntervalleConfiance(PrevisionMontant prevision) {
    printf("Intervalle de confiance: [%.2f € - %.2f €]\n",
           prevision.intervalle_confiance_bas,
           prevision.intervalle_confiance_haut);
}

// Affiche les prévisions de cashflow
void afficherPrevisionsCashflow(Transaction *revenus, Transaction *depenses, int mois_avance) {
    printf("\n=== PRÉVISIONS DE CASHFLOW (%d MOIS) ===\n", mois_avance);
    printf("Mois | Revenus Prévus | Dépenses Prévues | Solde Prévu\n");
    printf("-----+----------------+-----------------+----------\n");

    for (int i = 1; i <= mois_avance; i++) {
        float solde = previsionSolde(revenus, depenses, i);
        PrevisionMontant prev_rev = previsionMois(revenus, MODELE_SIMPLE);
        PrevisionMontant prev_dep = previsionMois(depenses, MODELE_SIMPLE);

        printf("%4d | %14.2f | %15.2f | %10.2f €\n",
               i,
               prev_rev.montant_prevu,
               prev_dep.montant_prevu,
               solde);
    }
}

// Scénario pessimiste
void analyserScenario_pessimiste(Transaction *revenus, Transaction *depenses) {
    printf("\n=== SCÉNARIO PESSIMISTE ===\n");

    PrevisionMontant prev_rev = previsionMois(revenus, MODELE_SIMPLE);
    PrevisionMontant prev_dep = previsionMois(depenses, MODELE_SIMPLE);

    float revenus_pessimistes = prev_rev.intervalle_confiance_bas;
    float depenses_pessimistes = prev_dep.intervalle_confiance_haut;

    printf("Revenus (bas):     %.2f €\n", revenus_pessimistes);
    printf("Dépenses (hautes): %.2f €\n", depenses_pessimistes);
    printf("Solde:             %.2f €\n", revenus_pessimistes - depenses_pessimistes);
    printf("\n⚠️  Dans ce scénario, il faudrait réduire les dépenses ou augmenter les revenus.\n");
}

// Scénario optimiste
void analyserScenario_optimiste(Transaction *revenus, Transaction *depenses) {
    printf("\n=== SCÉNARIO OPTIMISTE ===\n");

    PrevisionMontant prev_rev = previsionMois(revenus, MODELE_SIMPLE);
    PrevisionMontant prev_dep = previsionMois(depenses, MODELE_SIMPLE);

    float revenus_optimistes = prev_rev.intervalle_confiance_haut;
    float depenses_optimistes = prev_dep.intervalle_confiance_bas;

    printf("Revenus (hauts):   %.2f €\n", revenus_optimistes);
    printf("Dépenses (basses): %.2f €\n", depenses_optimistes);
    printf("Solde:             %.2f €\n", revenus_optimistes - depenses_optimistes);
    printf("\n✓ Dans ce scénario, vous pouvez augmenter votre épargne.\n");
}

// Scénario réaliste
void analyserScenario_realiste(Transaction *revenus, Transaction *depenses) {
    printf("\n=== SCÉNARIO RÉALISTE ===\n");

    PrevisionMontant prev_rev = previsionMois(revenus, MODELE_SIMPLE);
    PrevisionMontant prev_dep = previsionMois(depenses, MODELE_SIMPLE);

    printf("Revenus prévus:    %.2f €\n", prev_rev.montant_prevu);
    printf("Dépenses prévues:  %.2f €\n", prev_dep.montant_prevu);
    printf("Solde prévu:       %.2f €\n", prev_rev.montant_prevu - prev_dep.montant_prevu);
    printf("\n→ C'est le scénario le plus probable basé sur l'historique.\n");
}
