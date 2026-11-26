# Implémentation d'un Perceptron en C (C99)

Implémentation complète en C d'un neurone formel (perceptron) à fonction d'activation sigmoïde, entraîné par descente de gradient stochastique (en ligne) avec mélange de Fisher-Yates, découpage train/test, normalisation Min-Max et évaluation par matrice de confusion.

---

## 1. Architecture et compilation

### Arborescence du projet
```text
.
├── Makefile
├── README.md
├── Include/
│   ├── dataset.h       # Structures data et norme, chargement CSV, shuffle, split, normalisation
│   └── perceptron.h    # Structures perceptron et matrice, prédiction, entraînement, métriques
├── src/
│   ├── dataset.c       # Implémentation de la gestion mémoire et du prétraitement des données
│   ├── perceptron.c    # Implémentation du modèle, de la SGD et du calcul des métriques
│   └── main.c          # Point d'entrée CLI, orchestration du pipeline et libération mémoire
└── data/
    ├── et.csv
    ├── ou.csv
    └── champignons.csv
```

### Compilation et exécution
Le projet compile sans aucun avertissement avec les drapeaux `-Wall -Wextra` :
```bash
make
```

**Syntaxe d'exécution :**
```bash
./output/main <fichier.csv> [epochs] [learning_rate] [ratio] [normalisation]
```
* `fichier.csv` : chemin vers le jeu de données (obligatoire).
* `epochs` : nombre de passages complets sur le jeu d'entraînement (défaut : `100`).
* `learning_rate` : taux d'apprentissage $\eta$ (défaut : `0.1`).
* `ratio` : proportion d'entraînement dans $]0, 1]$ (`1.0` = entraînement sur tout le dataset sans test set ; défaut : `0.8`).
* `normalisation` : `1` pour activer la normalisation Min-Max, `0` sinon (défaut : `1`).

**Validation mémoire (Valgrind) :**
Testé sur `data/champignons.csv` (`100` époques, `lr = 0.1`, `ratio = 0.8`, `normalisation = 1`) :
```text
HEAP SUMMARY:
    in use at exit: 0 bytes in 0 blocks
  total heap usage: 94 allocs, 94 frees, 17,040 bytes allocated
All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors from 0 contexts
```

---

## 2. Choix d'implémentation et difficultés rencontrées

### Choix d'architecture et structures de données
* **Stockage unifié des lignes (`data`) :** chaque ligne `exemple[i]` alloue `dim` flottants contigus où les `dim - 1` premières colonnes contiennent les caractéristiques $x_1, \dots, x_{d}$ et la dernière colonne (`dim - 1`) stocke le label (`0` ou `1`). Le perceptron est initialisé avec `perc->dim = dataset.dim - 1` et lit naturellement les `perc->dim` premières cases sans recopie de tableau.
* **Mélange en place en $\mathcal{O}(1)$ par échange (`melange`) :** l'algorithme de Fisher-Yates échange uniquement les pointeurs de lignes `float *` dans `exemple[]`, évitant toute copie de flottants à chaque époque.
* **Prévention des fuites d'information (*data leakage*) :** lors d'un découpage train/test (`splitDataset` dans l'ordre du fichier, sans mélange préalable), les paramètres $\min_j$ et $\max_j$ sont calculés **uniquement** sur `train`, puis appliqués à `train` et `test`.
* **Règle de mise à jour (descente de gradient) :** en combinant la fonction sigmoïde $\sigma(z) = \frac{1}{1 + e^{-z}}$ et la perte d'entropie croisée binaire $\mathcal{L} = -\left[y\ln(\hat{y}) + (1-y)\ln(1-\hat{y})\right]$, le facteur $\sigma'(z) = \hat{y}(1-\hat{y})$ se simplifie avec le dénominateur de $\frac{\partial \mathcal{L}}{\partial \hat{y}} = \frac{\hat{y}-y}{\hat{y}(1-\hat{y})}$, aboutissant à :
  $$w_i \leftarrow w_i + \eta \cdot (y - \hat{y}) \cdot x_i \qquad \text{et} \qquad b \leftarrow b + \eta \cdot (y - \hat{y})$$

### Difficultés rencontrées et solutions apportées
1. **Division entière cachée en C :** l'expression `(float)(nb_correcte / d.n)` effectuait une division entière avant la conversion en flottant, tronquant toute accuracy inférieure à `100 %` à `0.00 %`. Corrigé en convertissant explicitement le numérateur ou le dénominateur en `float` avant la division (`(float)nb_correcte / d.n` et `float den`).
2. **Gestion des indices et double pointeur :** correction d'un accès hors limites sur le label (`data.dim` au lieu de `data.dim - 1`) lors du remplissage de la matrice de confusion, ainsi que du passage de pointeur (`perc` au lieu de `&perc`), détectés grâce à `-Wall -Wextra` et Valgrind.
3. **Sécurisation mémoire CSV & mode sans split :** remise à `NULL` systématique des pointeurs après libération partielle en cas d'erreur de lecture CSV afin d'écarter tout risque de *double free*, et adaptation de `splitDataset` pour accepter `ratio = 1.0` (`test.n = 0`) sans allouer de jeu de test vide.

---

## 3. Résultats expérimentaux sur les jeux de données

| Dataset | Split (`ratio`) | Normalisation | Époques (`lr = 0.1`) | Accuracy Train | Accuracy Test | Remarques |
| :--- | :---: | :---: | :---: | :---: | :---: | :--- |
| `data/et.csv` | Non (`1.0`) | Non (`0`) | 100 | **100,00 %** | — | Linéairement séparable (`w=[1.78, 1.79], b=-2.94`) |
| `data/ou.csv` | Non (`1.0`) | Non (`0`) | 100 | **100,00 %** | — | Linéairement séparable (`w=[2.52, 2.51], b=-0.59`) |
| `data/champignons.csv` | Oui (`0.8`) | Oui (`1`) | 100 | **100,00 %** | **100,00 %** | Convergence stable (`w=[2.74, 1.85, 1.98, -10.12], b=1.11`) |
| `data/points2d.csv` *(à compléter)* | Oui (`0.8`) | Oui (`1`) | 100 | *XX,XX %* | *XX,XX %* | *Commande : `./output/main data/points2d.csv 100 0.1 0.8 1`* |
| `data/xor.csv` *(à compléter)* | Non (`1.0`) | Non (`0`) | 100 | *XX,XX %* | — | *Non linéairement séparable (~50–75 % attendu)* |

---

## 4. Étude de cas : `data/champignons.csv` (Le secret du vieux sage)

### 4.1 Comparaison de convergence avec et sans normalisation (`lr = 0.1`, `ratio = 0.8`)

Les quatre critères morphologiques des champignons (diamètre du chapeau, hauteur du pied, intensité de couleur, diamètre du pied) s'étendent de `0` à `255`.

| Époques | Mode | Accuracy Train | Accuracy Test | F1-score Test | Évolution des poids ($w_3$) et du biais ($b$) |
| :---: | :--- | :---: | :---: | :---: | :--- |
| **5** | Sans normalisation | 81,25 % | **50,00 %** | **60,00 %** | $w_3 = -76.1500 \mid b = 0.5500$ |
| **5** | **Avec normalisation** | **90,62 %** | **87,50 %** | **80,00 %** | $w_3 = -1.6688 \mid b = 0.0582$ |
| **10** | Sans normalisation | 100,00 % | 100,00 % | 100,00 % | $w_3 = -100.5500 \mid b = 0.5500$ *(poids figés)* |
| **10** | **Avec normalisation** | 96,88 % | 100,00 % | 100,00 % | $w_3 = -2.8328 \mid b = 0.2442$ |
| **20** | Sans normalisation | 100,00 % | 100,00 % | 100,00 % | $w_3 = -100.5500 \mid b = 0.5500$ *(poids figés)* |
| **20** | **Avec normalisation** | 100,00 % | 100,00 % | 100,00 % | $w_3 = -4.5081 \mid b = 0.4744$ |
| **50** | Sans normalisation | 100,00 % | 100,00 % | 100,00 % | $w_3 = -100.5500 \mid b = 0.5500$ *(poids figés)* |
| **50** | **Avec normalisation** | 100,00 % | 100,00 % | 100,00 % | $w_3 = -7.4191 \mid b = 0.8847$ |
| **100** | Sans normalisation | 100,00 % | 100,00 % | 100,00 % | $w_3 = -100.5500 \mid b = 0.5500$ *(poids figés)* |
| **100** | **Avec normalisation** | 100,00 % | 100,00 % | 100,00 % | $w_3 = -10.1221 \mid b = 1.1099$ |

**Analyse :**
* **Instabilité initiale sans normalisation :** la mise à jour $\Delta w_i = \eta (y - \hat{y}) x_i$ est proportionnelle à $x_i \in [0, 255]$. À `lr = 0.1`, les pas atteignent jusqu'à `25,5` par exemple, propulsant $w_3$ à `-76.15` dès 5 époques. Ce saut dégrade fortement la généralisation (`50,00 %` d'accuracy sur le test, `4` faux positifs) tandis que le biais ($b = 0.55$) reste négligeable face aux poids.
* **Saturation numérique vs marge régulière :** dès 10 époques sans normalisation, $|z|$ devient si grand que $\sigma(z)$ sature exactement à `0.0` ou `1.0` en précision `float` 32 bits : l'erreur $(y - \hat{y})$ s'annule et les poids restent figés jusqu'à l'époque 100 (`w = [36.60, 27.85, 29.70, -100.55]`). À l'inverse, la normalisation Min-Max ramène les entrées dans $[0, 1]$ : dès 5 époques, l'accuracy de test atteint `87,50 %` sans aucun faux positif, et la sigmoïde reste dans sa zone active pour élargir progressivement la marge de séparation jusqu'à l'époque 100.

### 4.2 Matrice de confusion et métriques sur le jeu de test

À convergence (`100` époques, `lr = 0.1`, `ratio = 0.8`, `normalisation = 1`, `classe_positive = 1`) sur les `8` exemples de test :

| | Prédit Positif (`1` : toxique) | Prédit Négatif (`0` : comestible) |
| :--- | :---: | :---: |
| **Réellement Positif (`1` : toxique)** | **TP = 3** | **FN = 0** |
| **Réellement Négatif (`0` : comestible)** | **FP = 0** | **TN = 5** |

* **Accuracy :** $\frac{TP + TN}{TP + FP + TN + FN} = \frac{3 + 5}{8} = \mathbf{100,00\ \%}$
* **Précision :** $\frac{TP}{TP + FP} = \frac{3}{3 + 0} = \mathbf{100,00\ \%}$
* **Rappel (Recall) :** $\frac{TP}{TP + FN} = \frac{3}{3 + 0} = \mathbf{100,00\ \%}$
* **F1-score :** $2 \times \frac{1.0 \times 1.0}{1.0 + 1.0} = \mathbf{100,00\ \%}$

### 4.3 Choix de la classe positive et métrique prioritaire
* **Classe positive (`1 = non comestible`) :** `label = 1` correspond aux champignons toxiques et `label = 0` aux champignons comestibles. Définir la classe `1` comme classe positive place l'événement critique (le danger) au cœur des métriques de détection.
* **Métrique à privilégier (Rappel / Recall) :** l'asymétrie du coût des erreurs est vitale :
  * un **Faux Négatif (`FN`)** classe un champignon toxique (`1`) comme comestible (`0`), exposant les habitants à une intoxication mortelle ;
  * un **Faux Positif (`FP`)** rejette un champignon comestible (`0`), entraînant uniquement un gaspillage alimentaire sans danger.
  Il faut donc maximiser en priorité le **Rappel** ($\frac{TP}{TP + FN}$) pour garantir `FN = 0` (aucun champignon toxique ne doit passer), tout en surveillant le **F1-score** pour éviter un classifieur trivial qui rejetterait 100 % de la récolte.

### 4.4 Conclusion : peut-on faire confiance au perceptron pour remplacer le vieux sage ?
**Non, pas en autonomie complète.** Bien que le modèle atteigne `100,00 %` sur le jeu de test synthétique, il ne peut servir que d'outil d'aide à la décision pour trois raisons :
1. **Échantillon trop restreint :** `40` champignons au total (`32` en entraînement, `8` en test) sont statistiquement insuffisants pour garantir la généralisation à de nouvelles variétés ou variations morphologiques.
2. **Frontière purement linéaire :** un perceptron monocouche ne sépare l'espace que par un hyperplan ; il échouerait face à des critères de toxicité non linéairement séparables.
3. **Risque vital :** dans un problème où une seule erreur (`FN`) peut être mortelle, une validation empirique sur 8 exemples ne remplace pas une expertise humaine.

---

## 5. Effet du mélange aléatoire (`shuffle` de Fisher-Yates)

Test réalisé sur `data/champignons.csv` (`lr = 0.1`, `ratio = 0.8`, `normalisation = 1`) sur un faible nombre d'époques (`5` et `10`) en désactivant temporairement `melange()` en début d'époque :

| Configuration | Époques | Accuracy Train | Accuracy Test | F1-score Test | Comportement observé |
| :--- | :---: | :---: | :---: | :---: | :--- |
| **Sans shuffle** | 5 | 81,25 % | 87,50 % | 80,00 % | Trajectoire déterministe |
| **Sans shuffle** | 10 | 90,62 % | 100,00 % | 100,00 % | Progression régulière |
| **Avec shuffle** *(tirage A)* | 5 / 10 | 81,25 % / 90,62 % | 87,50 % / 87,50 % | 80,00 % / 80,00 % | Variance selon l'ordre tiré |
| **Avec shuffle** *(tirage B)* | 5 / 10 | 96,88 % / 96,88 % | 100,00 % / 100,00 % | 100,00 % / 100,00 % | Convergence plus rapide |

**Explication :**
* Sur `champignons.csv`, l'effet du *shuffle* reste **modéré** car les lignes du fichier CSV ne sont pas regroupées par classe : les exemples `0` et `1` y sont déjà entrelacés, ce qui évite que les poids ne soient poussés dans une seule direction pendant toute une demi-époque.
* Sur un faible nombre d'époques et un petit jeu d'entraînement (`32` exemples), le mélange introduit une **variance stochastique** liée au caractère non commutatif des mises à jour en ligne (SGD) : selon l'ordre des derniers exemples vus en fin d'époque, la frontière converge légèrement plus vite ou plus lentement. Dès `20` époques, les deux approches rejoignent `100,00 %`, mais le *shuffle* demeure indispensable pour prévenir le biais de récence sur des fichiers triés par étiquette.