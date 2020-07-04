import numpy as np
from sklearn.datasets import load_iris, load_digits
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import umap.umap_ as umap

sns.set(style='white', context='notebook', rc={'figure.figsize':(14,10)})
#reducer = umap.UMAP()

'''
iris = load_iris()
#print(iris.DESCR)

iris_df            = pd.DataFrame(iris.data, columns=iris.feature_names)
iris_df['species'] = pd.Series(iris.target).map(dict(zip(range(3),iris.target_names)))

#sns.pairplot(iris_df, hue='species');
#plt.show(sns)

embedding = reducer.fit_transform(iris.data)
#print(embedding.shape)

plt.scatter(embedding[:, 0], embedding[:, 1], c=[sns.color_palette()[x] for x in iris.target])
plt.gca().set_aspect('equal', 'datalim')
plt.title('UMAP projection of the Iris dataset', fontsize=24);

plt.show()
'''


digits = load_digits()
#print(digits.DESCR)

'''
fig, ax_array = plt.subplots(20, 20)
axes = ax_array.flatten()
for i, ax in enumerate(axes):
    ax.imshow(digits.images[i], cmap='gray_r')
plt.setp(axes, xticks=[], yticks=[], frame_on=False)
plt.tight_layout(h_pad=0.5, w_pad=0.01)

plt.show()

'''

reducer = umap.UMAP(random_state=42)
reducer.fit(digits.data)

print((digits))
embedding = reducer.transform(digits.data)
print("embedding")
print(embedding)

plt.scatter(embedding[:, 0], embedding[:, 1], c=digits.target, cmap='Spectral', s=5)
plt.gca().set_aspect('equal', 'datalim')
plt.colorbar(boundaries=np.arange(11)-0.5).set_ticks(np.arange(10))
plt.title('UMAP projection of the Digits dataset', fontsize=24);
plt.show()
