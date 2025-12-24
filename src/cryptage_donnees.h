#ifndef CRYPTAGE_DONNEES_H
#define CRYPTAGE_DONNEES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Algorithme simple de chiffrement (XOR avec clé)
typedef struct {
    unsigned char cle[32];
    int longueur_cle;
} CleChiffrement;

// Prototypes des fonctions
CleChiffrement creerCle(const char *motdepasse);
void chiffrerChaine(char *texte, CleChiffrement cle);
void dechiffrerChaine(char *texte_chiffre, CleChiffrement cle);

// Fichiers chiffrés
void sauvegarderChiffre(const char *filename, const char *contenu, CleChiffrement cle);
char *chargerDechiffre(const char *filename, CleChiffrement cle);

// Gestion des clés
void sauvegarderCle(const char *filename, CleChiffrement cle);
CleChiffrement chargerCle(const char *filename);
int verifierMotdepasse(const char *motdepasse);

// Hachage (simple)
unsigned int hacher_djb2(const char *str);
char *genererHash(const char *str);

// Sécurité
void effacerMemoire(void *ptr, size_t size);
int validerIntegrite(const char *fichier, const char *hash_attendu);

#endif
