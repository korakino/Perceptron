#include "../Include/dataset.h"

data creation_dataset(data exempleAddr)
{
    int ligne = exempleAddr.n;
    int colonnes = exempleAddr.dim;

    exempleAddr.exemple = malloc(ligne * sizeof(float *));

    if (exempleAddr.exemple == NULL)
    {
        perror("Erreur d'allocation du tableau principal");
        exempleAddr.n = 0;
        return exempleAddr;
    }

    for (int i = 0; i < ligne; i++)
    {
        exempleAddr.exemple[i] = malloc(colonnes * sizeof(*exempleAddr.exemple[i]));
        if (exempleAddr.exemple[i] == NULL)
        {
            perror("Erreur d'allocation d'une ligne");
            for (int j = 0; j < i; j++)
            {
                free(exempleAddr.exemple[j]);
            }
            free(exempleAddr.exemple);
            exempleAddr.exemple = NULL;
            exempleAddr.n = 0;
            return exempleAddr;
        }
    }
    return exempleAddr;
}

void freedataset(data exempleAddr)
{
    if (exempleAddr.exemple == NULL)
        return;

    for (int i = 0; i < exempleAddr.n; i++)
    {
        free(exempleAddr.exemple[i]);
    }
    free(exempleAddr.exemple);
}

int nbLignes(char *chemin)
{
    int nbLigne = 0;
    FILE *fo = fopen(chemin, "r");

    if (fo == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier dans nbLignes\n");
        perror(chemin);
        return -1;
    }

    char ligne[4096];

    while (fgets(ligne, sizeof(ligne), fo) != NULL)
    {
        if (!(ligne[0] == '\n' || ligne[0] == '\r'))
            nbLigne++;
    }

    fclose(fo);
    return nbLigne;
}

int nbDim(char *chemin)
{
    int dim = 0;
    FILE *fo = fopen(chemin, "r");
    char ligne[4096];

    if (fo == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier dans la fonction nbDim\n");
        perror(chemin);
        return -1;
    }
    else
    {
        fgets(ligne, sizeof(ligne), fo);
        const char *delim = ",\r\n";

        char *token = strtok(ligne, delim);

        while (token != NULL)
        {
            dim++;
            token = strtok(NULL, delim);
        }
    }

    fclose(fo);
    return dim;
}

int remplirDataset(data *data, char *chemin, int interval_a, int interval_b)
{

    int ligne = 0;
    int colonne = 0;
    FILE *fo = fopen(chemin, "r");
    char caracteres[4096];
    int interval = 0;

    if (fo == NULL)
    {
        printf("Erreur lors de l'ouverture du fichier");
        return -1;
    }

    else
    {
        while (fgets(caracteres, sizeof(caracteres), fo) != NULL && interval != interval_b)
        { // debut du remplissage.
          // On ignore les lignes vides avant le saut (interval < interval_a) ET avant la lecture
            if (!(caracteres[0] == '\n' || caracteres[0] == '\r'))
            {
                if (interval < interval_a)
                {
                    interval++;
                }
                else
                {
                    if (!(caracteres[0] == '\n' || caracteres[0] == '\r'))
                    {
                        const char *delim = ",\r\n";

                        char *token = strtok(caracteres, delim);
                        colonne = 0;
                        while (token != NULL)
                        {
                            if (colonne >= data->dim)
                            {
                                printf("trop de valeurs dans la ligne\n");
                                fclose(fo);
                                return -1;
                            }
                            data->exemple[ligne][colonne] = (float)atof(token);
                            token = strtok(NULL, delim);
                            colonne++;
                        }

                        if (colonne < data->dim)
                        {
                            printf("pas assez de valeurs dans la ligne\n");
                            fclose(fo);
                            return -1;
                        }
                        ligne++;
                        interval++;
                    }
                }
            }
        }
    }
    fclose(fo);
    return 0;
}

void afficheData(data data)
{
    int ligne = data.n;
    int colonne = data.dim - 1;
    for (int i = 0; i < ligne; i++)
    {
        printf("[");
        for (int j = 0; j < colonne; j++)
        {
            printf("%.2f, ", data.exemple[i][j]);
        }
        printf("%.2f]\n", data.exemple[i][colonne]);
    }
}

int loadDataset(data *data, char *chemin, int interval_a, int interval_b)
{
    int tmp = nbLignes(chemin);
    data->dim = nbDim(chemin);

    if (interval_b == -1)
    {
        interval_b = tmp;
    }

    data->n = interval_b - interval_a;

    if (data->n <= 0 || data->dim <= 0)
        return -1;
    if (interval_a < 0 || interval_b > tmp)
        return -1;

    *data = creation_dataset(*data);
    if (data->exemple == NULL)
    {
        return -1;
    }

    if (remplirDataset(data, chemin, interval_a, interval_b) == -1)
    {
        freedataset(*data);
        data->exemple = NULL;
        data->n = 0;
        data->dim = 0;
        return -1;
    }
    return 0;
}

void swap(data *data, int indice_a, int indice_b)
{
    float *tmp;
    tmp = data->exemple[indice_a];
    data->exemple[indice_a] = data->exemple[indice_b];
    data->exemple[indice_b] = tmp;
}

void melange(data *data)
{
    if (data->exemple == NULL)
    {
        printf("data est vide !\n");
        return;
    }

    int num = 0;
    for (int i = data->n - 1; i > 0; i--)
    {
        num = rand() % (i + 1);
        swap(data, i, num);
    }
}

int splitDataset(data *source, float ratio, data *train, data *test)
{
    if (ratio <= 0.0f || ratio > 1.0f)
        return -1;

    int n_train = (int)(source->n * ratio);
    int n_test = source->n - n_train;

    if (n_train <= 0 || n_test < 0)
        return -1;

    train->n = n_train;
    train->dim = source->dim;
    *train = creation_dataset(*train);
    if (train->exemple == NULL)
        return -1;

    test->n = n_test;
    test->dim = source->dim;
    test->exemple = NULL;
    if (n_test > 0)
    {
        *test = creation_dataset(*test);
        if (test->exemple == NULL)
        {
            freedataset(*train);
            return -1;
        }
    }

    for (int i = 0; i < n_train; i++)
    {
        for (int k = 0; k < train->dim; k++)
        {
            train->exemple[i][k] = source->exemple[i][k];
        }
    }
    for (int j = 0; j < n_test; j++)
    {
        for (int k = 0; k < test->dim; k++)
        {
            test->exemple[j][k] = source->exemple[j + n_train][k];
        }
    }

    return 0;
}

void calcul(norme *norme, data data)
{
    if (norme == NULL || data.exemple == NULL || data.n <= 0 || data.dim <= 1)
        return;
    norme->caracteristiques = data.dim - 1;

    norme->max = malloc(sizeof(float) * norme->caracteristiques);
    norme->min = malloc(sizeof(float) * norme->caracteristiques);

    if (norme->max == NULL || norme->min == NULL)
    {
        perror("erreur d'allocation des valeurs caractéristiques");
        free(norme->max);
        free(norme->min);
        norme->max = NULL;
        norme->min = NULL;
        norme->caracteristiques = 0;
        return;
    }

    int colonne = 0;

    while (colonne < norme->caracteristiques)
    {

        norme->max[colonne] = data.exemple[0][colonne];
        norme->min[colonne] = data.exemple[0][colonne];
        for (int i = 0; i < data.n; i++)
        {
            if (norme->max[colonne] < data.exemple[i][colonne])
            {
                norme->max[colonne] = data.exemple[i][colonne];
            }
            if (norme->min[colonne] > data.exemple[i][colonne])
            {
                norme->min[colonne] = data.exemple[i][colonne];
            }
        }

        colonne++;
    }
}

void applicationNorme(data *data, norme norme)
{
    if (data->exemple == NULL || data->n <= 0 || data->dim <= 1 || norme.max == NULL || norme.min == NULL || norme.caracteristiques != data->dim - 1 || data == NULL)
        return;

    float etendu = 0;
    float valeur = 0;

    for (int i = 0; i < data->n; i++)
    {
        for (int j = 0; j < data->dim - 1; j++)
        {
            etendu = norme.max[j] - norme.min[j];
            valeur = data->exemple[i][j];
            if (etendu == 0)
            {
                valeur = 0;
            }
            else
            {
                valeur = (valeur - norme.min[j]) / etendu;
            }
            data->exemple[i][j] = valeur;
        }
    }
}

void liberation_n(norme *norme)
{
    if (norme == NULL)
        return;
    norme->caracteristiques = 0;
    free(norme->min);
    free(norme->max);
    norme->min = NULL;
    norme->max = NULL;
}

// int main(void)
// {
//     data data;
//     char *chemin = "data/champignons.csv";
//     loadDataset(&data, chemin, 0, -1);
//     afficheData(data);

//     freedataset(data);

//     return 0;
// }