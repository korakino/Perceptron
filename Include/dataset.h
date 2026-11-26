#ifndef DATASET_H
#define DATASET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef struct normalisation
{
    int caracteristiques;
    float *min;
    float *max;
} norme;

typedef struct data
{
    /* data */
    int n;           // nb d'exemples
    int dim;         // nombre de caractéristiques
    float **exemple; // caratéristiques du chapignons
} data;

data creation_dataset(data exempleAddr);
// alloue dynamiquement la mémoire

void freedataset(data exempleAddr);
// libere la mémoire dynamiquement

int nbLignes();
// fonction qui va chercher les données dans champignons.csv et compte le nombre de lignes que le fichier a

int nbDim();
// compte le nombre de colonnes (dim) de champignons.csv

int main();
// fonction principale du dataset

int remplirDataset(data *data, char *chemin, int interval_a, int interval_b);
// charge les données des csv pour les mettes dans data

int loadDataset(data *data, char *chemin, int interval_a, int interval_b);
// appelle les fonctions nbLignes, nbDim et remplir dataset pour créer un dataset complet. interval_b = -1 pour prendre tout le dataset

void swap(data *data, int indice_a, int indice_b);
// échange l'adresse entre deux exemples.

void melange(data *data);
// melange les exemples du dataset

int splitDataset(data *source, float ratio, data *train, data *test);
// creer les dataset train et test à partir du dataset d'origine

void calcul(norme *norme, data data);
//alloue les tableaux max et min et calcul les valeurs max et min de chaque colonne

void applicationNorme(data *data, norme norme);
//modifie les valeurs de data ajustee avec la norme en vigueur

void liberation_n(norme *norme);
//libere la memoire occupee par la structure norme

#endif