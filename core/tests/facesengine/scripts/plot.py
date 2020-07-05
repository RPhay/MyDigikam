import numpy as np
from sklearn.model_selection import train_test_split, cross_val_score
from sklearn.neighbors import KNeighborsClassifier
from sklearn.svm import SVC

import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import umap.umap_ as umap
import json

sns.set(style='white', context='notebook', rc={'figure.figsize':(14,10)})

with open('extendedB.json') as f:
    jsonData = json.load(f)

faceEmbeddings = []
labels = []
for json in jsonData:
    faceEmbeddings.append(json['faceembedding'])
    labels.append(json['id'])
'''
for i in range(1, 50):
    reducer = umap.UMAP(n_neighbors=16, metric='euclidean', min_dist=0.1, n_components=i*2, transform_seed=42)

    reducer.fit(faceEmbeddings, y=labels)

    reducedEmbedding = reducer.transform(faceEmbeddings)

    X_train, X_test, y_train, y_test = train_test_split(reducedEmbedding,
                                                        labels,
                                                        stratify=labels,
                                                        random_state=42)

    svc = SVC().fit(X_train, y_train)
    knn = KNeighborsClassifier().fit(X_train, y_train)

    print(i*2)
    print(svc.score(X_test, y_test), knn.score(X_test, y_test))
'''

reducer = umap.UMAP(n_neighbors=15, metric='euclidean', min_dist=0.1, n_components=2, transform_seed=42)
reducer.fit(faceEmbeddings, y=labels)
reducedEmbedding = reducer.transform(faceEmbeddings)

X_train, X_test, y_train, y_test = train_test_split(reducedEmbedding,
                                                    labels,
                                                    stratify=labels,
                                                    random_state=42)
svc = SVC().fit(X_train, y_train)
knn = KNeighborsClassifier().fit(X_train, y_train)

print(svc.score(X_test, y_test), knn.score(X_test, y_test))


plt.scatter(reducedEmbedding[:, 0], reducedEmbedding[:, 1], c=labels, cmap='Spectral', s=5)
plt.gca().set_aspect('equal', 'datalim')
plt.colorbar(boundaries=np.arange(11)-0.5).set_ticks(np.arange(10))
plt.title('UMAP projection of the faceembedding', fontsize=24);
plt.show()

