#ifndef BILAN_MENSUEL_H
#define BILAN_MENSUEL_H

#include "depenses.h"
#include "revenus.h"
#include "epargnes.h"

// Prototypes des fonctions
void genererBilanMensuel(Transaction *revenus, Transaction *depenses, Epargne *epargnes, const char *mois);
float calculerSoldeMensuel(Transaction *revenus, Transaction *depenses, const char *mois);
float calculerEpargneMensuelle(Epargne *epargnes, const char *mois);
void afficherStatistiquesParCategorie(Transaction *revenus, Transaction *depenses, const char *mois);

#endif