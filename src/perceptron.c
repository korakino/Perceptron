#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../Include/perceptron.h"

struct perceptron creation(int dim){
    struct perceptron *p = malloc(sizeof(struct perceptron));
    p->w = malloc(dim * sizeof(float));
    for (int i=0;i<dim;i++){
        p->w[i]=0;
    }
    p->b = 0;
    p->lr = 0.1;
    p->dim = dim;
    return *p;
}

void libération(struct perceptron *p){
    free(p->w);
    free(p);
}

float sigmoide(int z){
    z = 1 / (1 + exp(-z));
    return z;
}

float prediction(struct perceptron *p, float x[], int z){
    for (int i=0; i<p->dim ; i++){
        z = x[i]*p->w[i];
    }
    z = z + p->b;
    z = sigmoide(z);
    return z;
}

void maj(struct perceptron *p, int label, float x[], int z){
    for (int i = 0; i<p->dim; i++){
        p->w[i]= p->w[i] + p->lr * x[i] * (label - prediction(p, x[p->dim], z));
    }
}

