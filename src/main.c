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
    int epochs = 100;  // valeur par défaut si non précisée
    float lr = 0.1f;   // valeur par défaut si non précisée
    float ratio = 0.8; // valeur par défaut si non précisée

    if (argc >= 3)
        epochs = atoi(argv[2]);
    if (argc >= 4)
        lr = (float)atof(argv[3]);
    if (argc >= 5)
        ratio = (float)atof(argv[4]);

    if (epochs <= 0 || lr <= 0.0f)
    {
        fprintf(stderr, "Erreur : epochs et learning_rate doivent etre strictement positifs.\n");
        return 1;
    }

    srand((unsigned int)time(NULL));

    data dataset = {0, 0, NULL};
    data train = {0, 0, NULL};
    data test = {0, 0, NULL};

    loadDataset(&dataset, chemin, 0, -1);
    if (dataset.exemple == NULL || dataset.n <= 0 || dataset.dim <= 1)
    {
        fprintf(stderr, "Erreur lors du chargement du fichier : %s\n", chemin);
        freedataset(dataset);
        freedataset(train);
        freedataset(test);
        return 1;
    }
    if (splitDataset(&dataset, ratio, &train, &test) == -1)
    {
        freedataset(dataset);
        freedataset(train);
        freedataset(test);
        return 1;
    }

    perceptron *perc = creation_p(dataset.dim - 1, lr);

    if (perc == NULL)
    {
        freedataset(dataset);
        freedataset(train);
        freedataset(test);
        return 1;
    }

    printf("Dataset : %d exemples (%d train, %d test), %d caracteristiques\n",
           dataset.n, train.n, test.n, dataset.dim - 1);
    printf("Accuracy avant entrainement (train) : %.2f%%\n", accuracy(perc, train) * 100.0f);
    printf("Accuracy avant entrainement (test)  : %.2f%%\n", accuracy(perc, test) * 100.0f);

    entrainement(perc, &train, epochs);

    printf("Accuracy apres entrainement (%d epoques, lr=%.4f) :\n", epochs, lr);
    printf("  -> Train : %.2f%%\n", accuracy(perc, train) * 100.0f);
    printf("  -> Test  : %.2f%%\n", accuracy(perc, test) * 100.0f);

    printf("Parametres appris : ");
    for (int i = 0; i < perc->dim; i++)
    {
        printf("w[%d]=%.4f ", i, perc->w[i]);
    }
    printf("| b=%.4f\n", perc->b);

    liberation_p(perc);
    freedataset(dataset);
    freedataset(train);
    freedataset(test);

    return 0;
}