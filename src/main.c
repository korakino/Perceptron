#include "../Include/perceptron.h"
#include "../Include/dataset.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage : %s <fichier.csv> [epochs] [learning_rate] [ratio] [normalisation]\n", argv[0]);
        return 1;
    }

    char *chemin = argv[1];
    int epochs = 100;      // valeur par défaut si non précisée
    float lr = 0.1f;       // valeur par défaut si non précisée
    float ratio = 0.8f;    // valeur par défaut si non précisée
    int normalisation = 1; // valeur par défaut si non précisée

    if (argc >= 3)
        epochs = atoi(argv[2]);
    if (argc >= 4)
        lr = (float)atof(argv[3]);
    if (argc >= 5)
        ratio = (float)atof(argv[4]);
    if (argc >= 6)
        normalisation = atoi(argv[5]);

    if (epochs <= 0 || lr <= 0.0f)
    {
        fprintf(stderr, "Erreur : epochs et learning_rate doivent etre strictement positifs.\n");
        return 1;
    }

    srand((unsigned int)time(NULL));

    data dataset = {0, 0, NULL};
    data train = {0, 0, NULL};
    data test = {0, 0, NULL};
    norme norme = {0, NULL, NULL};

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

    if (normalisation == 1)
    {
        calcul(&norme, train);
        if (norme.max == NULL || norme.min == NULL)
        {
            liberation_p(perc);
            liberation_n(&norme);
            freedataset(dataset);
            freedataset(train);
            freedataset(test);
            return 1;
        }
        applicationNorme(&train, norme);
        applicationNorme(&test, norme);
    }

    printf("Dataset : %d exemples (%d train, %d test), %d caracteristiques\n",
           dataset.n, train.n, test.n, dataset.dim - 1);
    printf("Accuracy avant entrainement (train) : %.2f%%\n", accuracy(perc, train) * 100.0f);
    if (test.n > 0)
        printf("Accuracy avant entrainement (test)  : %.2f%%\n", accuracy(perc, test) * 100.0f);

    entrainement(perc, &train, epochs);

    printf("Accuracy apres entrainement (%d epoques, lr=%.4f) :\n", epochs, lr);
    printf("  -> Train : %.2f%%\n", accuracy(perc, train) * 100.0f);
    if (test.n > 0)
    {
        printf("  -> Test  : %.2f%%\n", accuracy(perc, test) * 100.0f);

        matrice mat = {0, 0, 0, 0};
        remplirMatrice(test, &mat, perc, 1);

        printf("\nMatrice de confusion sur le test set (classe positive = 1) :\n");
        printf("  TP = %d | FP = %d\n", mat.TP, mat.FP);
        printf("  FN = %d | TN = %d\n", mat.FN, mat.TN);

        printf("Metriques sur le test set :\n");
        printf("  -> Accuracy  : %.2f%%\n", accuracyMatrice(mat) * 100.0f);
        printf("  -> Precision : %.2f%%\n", precisionMatrice(mat) * 100.0f);
        printf("  -> Rappel    : %.2f%%\n", recallMatrice(mat) * 100.0f);
        printf("  -> F1-score  : %.2f%%\n", scoreMatrice(mat) * 100.0f);
    }

    printf("\nParametres appris : ");
    for (int i = 0; i < perc->dim; i++)
    {
        printf("w[%d]=%.4f ", i, perc->w[i]);
    }
    printf("| b=%.4f\n", perc->b);

    liberation_p(perc);
    liberation_n(&norme);
    freedataset(dataset);
    freedataset(train);
    freedataset(test);

    return 0;
}