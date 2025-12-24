#include "cryptage_donnees.h"

// Crée une clé de chiffrement à partir d'un mot de passe
CleChiffrement creerCle(const char *motdepasse) {
    CleChiffrement cle;
    memset(cle.cle, 0, sizeof(cle.cle));

    cle.longueur_cle = strlen(motdepasse);
    if (cle.longueur_cle > 32) cle.longueur_cle = 32;

    strncpy((char *)cle.cle, motdepasse, cle.longueur_cle);

    return cle;
}

// Chiffre une chaîne avec XOR (algorithme simple)
void chiffrerChaine(char *texte, CleChiffrement cle) {
    if (texte == NULL || cle.longueur_cle == 0) return;

    for (int i = 0; texte[i] != '\0'; i++) {
        texte[i] ^= cle.cle[i % cle.longueur_cle];
    }
}

// Déchiffre une chaîne (XOR est symétrique)
void dechiffrerChaine(char *texte_chiffre, CleChiffrement cle) {
    chiffrerChaine(texte_chiffre, cle); // XOR est son propre inverse
}

// Sauvegarde du contenu chiffré
void sauvegarderChiffre(const char *filename, const char *contenu, CleChiffrement cle) {
    if (contenu == NULL) return;

    // Créer une copie du contenu
    char *copie = (char *)malloc(strlen(contenu) + 1);
    if (copie == NULL) return;

    strcpy(copie, contenu);

    // Chiffrer la copie
    chiffrerChaine(copie, cle);

    // Sauvegarder
    FILE *file = fopen(filename, "wb");
    if (file != NULL) {
        fwrite(copie, 1, strlen(copie) + 1, file);
        fclose(file);
    }

    // Effacer la copie de la mémoire
    effacerMemoire(copie, strlen(copie) + 1);
    free(copie);
}

// Charge et déchiffre le contenu
char *chargerDechiffre(const char *filename, CleChiffrement cle) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) return NULL;

    // Déterminer la taille
    fseek(file, 0, SEEK_END);
    long taille = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Lire le contenu
    char *contenu = (char *)malloc(taille + 1);
    if (contenu == NULL) {
        fclose(file);
        return NULL;
    }

    size_t lus = fread(contenu, 1, taille, file);
    contenu[lus] = '\0';
    fclose(file);

    // Déchiffrer
    dechiffrerChaine(contenu, cle);

    return contenu;
}

// Sauvegarde la clé (dangéreux, à utiliser avec prudence)
void sauvegarderCle(const char *filename, CleChiffrement cle) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    // Sauvegarder uniquement la longueur et la clé
    fwrite(&cle.longueur_cle, sizeof(int), 1, file);
    fwrite(cle.cle, sizeof(unsigned char), cle.longueur_cle, file);
    fclose(file);
}

// Charge la clé
CleChiffrement chargerCle(const char *filename) {
    CleChiffrement cle;
    memset(cle.cle, 0, sizeof(cle.cle));
    cle.longueur_cle = 0;

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return cle;
    }

    fread(&cle.longueur_cle, sizeof(int), 1, file);
    if (cle.longueur_cle > 0 && cle.longueur_cle <= 32) {
        fread(cle.cle, sizeof(unsigned char), cle.longueur_cle, file);
    }
    fclose(file);

    return cle;
}

// Vérifie le mot de passe
int verifierMotdepasse(const char *motdepasse) {
    // Vérifier la longueur minimum
    if (strlen(motdepasse) < 4) {
        printf("Erreur: Le mot de passe doit contenir au moins 4 caractères.\n");
        return 0;
    }

    // Vérifier qu'il y a au moins un chiffre
    int has_digit = 0;
    for (int i = 0; motdepasse[i] != '\0'; i++) {
        if (motdepasse[i] >= '0' && motdepasse[i] <= '9') {
            has_digit = 1;
            break;
        }
    }

    if (!has_digit) {
        printf("Avertissement: Il est recommandé d'inclure au moins un chiffre.\n");
    }

    return 1;
}

// Hachage simple (DJB2)
unsigned int hacher_djb2(const char *str) {
    unsigned int hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

// Génère un hash
char *genererHash(const char *str) {
    unsigned int hash = hacher_djb2(str);

    char *hash_str = (char *)malloc(16);
    if (hash_str != NULL) {
        sprintf(hash_str, "%u", hash);
    }

    return hash_str;
}

// Efface la mémoire (importante pour la sécurité)
void effacerMemoire(void *ptr, size_t size) {
    volatile unsigned char *p = (volatile unsigned char *)ptr;
    while (size--) {
        *p++ = 0;
    }
}

// Valide l'intégrité d'un fichier
int validerIntegrite(const char *fichier, const char *hash_attendu) {
    FILE *file = fopen(fichier, "rb");
    if (file == NULL) return 0;

    // Lire le contenu
    fseek(file, 0, SEEK_END);
    long taille = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *contenu = (char *)malloc(taille + 1);
    if (contenu == NULL) {
        fclose(file);
        return 0;
    }

    size_t lus = fread(contenu, 1, taille, file);
    contenu[lus] = '\0';
    fclose(file);

    // Calculer le hash
    char *hash_calcule = genererHash(contenu);
    int resultat = strcmp(hash_calcule, hash_attendu) == 0;

    // Nettoyer
    free(hash_calcule);
    effacerMemoire(contenu, taille + 1);
    free(contenu);

    return resultat;
}
