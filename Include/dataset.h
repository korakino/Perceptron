#ifndef DATASET_H
#define DATASET_H



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


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
//libere la mémoire dynamiquement

int nbLignes();
// fonction qui va chercher les données dans champignons.csv et compte le nombre de lignes que le fichier a

int nbDim();
//compte le nombre de colonnes (dim) de champignons.csv

int main();
//fonction principale du dataset

int remplirDataset(data *data, char *chemin, int interval_a, int interval_b);
//charge les données des csv pour les mettes dans data

int loadDataset(data *data, char *chemin, int interval_a, int interval_b);
//appelle les fonctions nbLignes, nbDim et remplir dataset pour créer un dataset complet. interval_b = -1 pour prendre tout le dataset

void swap(data *data, int indice_a, int indice_b);
//échange l'adresse entre deux exemples.

void melange(data *data);
//melange les exemples du dataset


#endif