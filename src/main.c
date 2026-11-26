#include "../Include/perceptron.h"
#include "../Include/dataset.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage : %s <fichier.csv> [epochs] [learning_rate]\n", argv[0]);
        return 1;
    }

    char *chemin = argv[1];
    int epochs = 100; // valeur par défaut si non précisée
    float lr = 0.1f;  // valeur par défaut si non précisée

    if (argc >= 3)
        epochs = atoi(argv[2]);
    if (argc >= 4)
        lr = (float)atof(argv[3]);


    if (epochs <= 0 || lr <= 0.0f)
    {
        fprintf(stderr, "Erreur : epochs et learning_rate doivent etre strictement positifs.\n");
        return 1;
    }

    srand((unsigned int)time(NULL));

    data *dataset = loadDataset(dataset, chemin, 0, -1);

    perceptron *perc = creation_p(dataset->dim -1, lr);







    return 0;
}