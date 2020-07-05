import numpy as np
from sklearn.datasets import load_iris, load_digits
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import umap.umap_ as umap
import json

sns.set(style='white', context='notebook', rc={'figure.figsize':(14,10)})

with open('yalefaces.json') as f:
    jsonData = json.load(f)

faceEmbeddings = []
labels = []
for json in jsonData:
    faceEmbeddings.append(json['faceembedding'])
    labels.append(json['id'])

reducer = umap.UMAP(a=None, angular_rp_forest=False, b=None, force_approximation_algorithm=False, init='spectral', learning_rate=1.0,
                    local_connectivity=1.0, low_memory=False, metric='euclidean', metric_kwds=None, min_dist=0.01, n_components=2, n_epochs=None,
                    n_neighbors=50, negative_sample_rate=5, output_metric='euclidean', output_metric_kwds=None, random_state=42, repulsion_strength=1.0,
                    set_op_mix_ratio=1.0, spread=1.0, target_metric='categorical', target_metric_kwds=None, target_n_neighbors=-1, target_weight=0.5,
                    transform_queue_size=4.0, transform_seed=42, unique=False, verbose=False)

reducer.fit(faceEmbeddings)

reducedEmbedding = reducer.transform(faceEmbeddings)
print(reducedEmbedding)

plt.scatter(reducedEmbedding[:, 0], reducedEmbedding[:, 1], c=labels, cmap='Spectral', s=5)
plt.gca().set_aspect('equal', 'datalim')
plt.colorbar(boundaries=np.arange(11)-0.5).set_ticks(np.arange(10))
plt.title('UMAP projection of the Digits dataset', fontsize=24);
plt.show()

