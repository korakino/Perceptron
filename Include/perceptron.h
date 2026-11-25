#include <stdio.h>

typedef struct perceptron 
{
int dim; //dimension d'entrée
float *w; //vecteur de poids : tableau de taille dim contenant les poids de chaque entrée (les w_i associés aux x_i)
float b; //biais 
float lr; //taux d'apprentissage
} perceptron;