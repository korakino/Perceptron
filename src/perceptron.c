#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../Include/perceptron.h"

perceptron *creation_p(int dim, float lr)
{
    if (dim <= 0)
        return NULL;

    perceptron *p = malloc(sizeof(perceptron));
    if (p == NULL)
    {
        perror("Erreur d'allocation du perceptron");
        return NULL;
    }

    p->w = malloc((size_t)dim * sizeof(float));
    if (p->w == NULL)
    {
        perror("Erreur d'allocation des poids");
        free(p);
        return NULL;
    }

    for (int i = 0; i < dim; i++)
    {
        p->w[i] = 0.0f;
    }

    p->b = 0.0f;
    p->lr = lr;
    p->dim = dim;
    return p;
}

void liberation_p(perceptron *p)
{
    if (p == NULL)
        return;

    free(p->w);
    free(p);
}

float sigmoide(float z)
{
    return 1.0f / (1.0f + expf(-z));
}

float prediction(const perceptron *p, const float x[])
{
    float z = 0.0f;
    for (int i = 0; i < p->dim; i++)
    {
        z += x[i] * p->w[i];
    }
    z += p->b;
    return sigmoide(z);
}

void maj(perceptron *p, int label, const float x[])
{
    float y = prediction(p, x);
    float erreur = (float)label - y;

    for (int i = 0; i < p->dim; i++)
    {
        p->w[i] += p->lr * x[i] * erreur;
    }
    p->b += p->lr * erreur;
}

void entrainement(perceptron *p, data *data, int nb_epoque)
{
    if (p == NULL || data == NULL || nb_epoque < 1 || data->exemple == NULL || p->dim != data->dim - 1)
    {
        printf("valeur d'entrée incorrecte\n");
        return;
    }
    int epoque = 0;
    while (epoque < nb_epoque)
    {
        melange(data);
        for (int i = 0; i < data->n; i++)
        {
            int label = (int)(data->exemple[i][data->dim - 1] + 0.5f);
            maj(p, label, data->exemple[i]);
        }
        epoque++;
    }
}

float accuracy(perceptron *p, data d)
{
    int nb_correcte = 0;
    if (p == NULL || d.exemple == NULL || p->dim != d.dim - 1 || d.n <= 0)
    {
        printf("valeur d'entrée incorrecte\n");
        return -1;
    }
    for (int i = 0; i < d.n; i++)
    {
        float predit = prediction(p, d.exemple[i]);
        int label = (int)(d.exemple[i][d.dim - 1] + 0.5f);
        if ( (predit >= 0.5 && 1 == label) || (predit < 0.5 && 0 == label ) )
        {
            nb_correcte++;
        }

    }
    return (float)(nb_correcte/d.n);
}