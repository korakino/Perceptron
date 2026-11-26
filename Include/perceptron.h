#ifndef PERCEPTRON_H
#define PERCEPTRON_H

#include "../Include/dataset.h"

typedef struct perceptron
{
    int dim;  // dimension d'entrée (nombre de caractéristiques, sans le label)
    float *w; // vecteur de poids : tableau de taille dim contenant les poids w_i associés aux x_i
    float b;  // biais
    float lr; // taux d'apprentissage
} perceptron;

typedef struct matrice_confusion
{
    int TP;
    int FP;
    int TN;
    int FN;
} matrice;

// Initialise un perceptron, ses poids a zero et son taux d'apprentissage.
perceptron *creation_p(int dim, float lr);

// Libere la memoire du tableau de poids et du perceptron.
void liberation_p(perceptron *p);

// Transforme une somme ponderee en probabilite entre 0 et 1.
float sigmoide(float z);

// Calcule la sortie du perceptron pour une entree x.
float prediction(const perceptron *p, const float x[]);

// Corrige les poids et le biais a partir du label attendu.
void maj(perceptron *p, int label, const float x[]);

// Entraine le perceptron sur toutes les donnees pendant plusieurs epoques.
void entrainement(perceptron *p, data *data, int nb_epoque);

// calcul la justesse des interpretation du perceptron et les compare
float accuracy(perceptron *p, data d);

//cherche les 4 valeurs possible d'une reponse venant du perceptron et les met dans la matrice
void remplirMatrice(data data, matrice *matrice, const perceptron *perc, int classe_positive);

//calcul l'accuracy de la matrice
float accuracyMatrice(matrice matrice);

//calcul la precision de la matrice
float precisionMatrice(matrice matrice);

//calcul le recall de la matrice
float recallMatrice(matrice matrice);

//calcul le F1-score de la matrice
float scoreMatrice(matrice matrice);


#endif