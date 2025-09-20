import pandas as pd
import networkx as nx
import random
from gensim.models import Word2Vec
from sklearn.cluster import KMeans
from sklearn.decomposition import PCA
import matplotlib.pyplot as plt

# Load the dataset
df = pd.read_csv('/home/cselab/Desktop/fraud_analytics/Payments.csv')
# Print the number of rows and columns
print(f"Number of rows: {df.shape[0]}")
print(f"Number of columns: {df.shape[1]}")

# Print the column names
print("Column names:")
for column in df.columns:
    print(column)

# Create a directed graph from the dataset
G = nx.from_pandas_edgelist(df, source='Sender', target='Receiver', edge_attr='Amount', create_using=nx.DiGraph)

# Node2Vec parameters
dimensions = 64
walk_length = 30
num_walks = 200
workers = 4
window_size = 10
min_count = 1
batch_words = 4
num_clusters = 3

# Function to generate random walks
def generate_random_walks(graph, num_walks, walk_length):
    """
    Generates random walks on the graph.

    Parameters:
    - graph: NetworkX graph object
    - num_walks: Number of walks to generate per node
    - walk_length: Length of each random walk
    """
    walks = []
    nodes = list(graph.nodes())
    for _ in range(num_walks):
        random.shuffle(nodes)
        for node in nodes:
            walk = [node]
            while len(walk) < walk_length:
                current_node = walk[-1]
                neighbors = list(graph.neighbors(current_node))
                if len(neighbors) > 0:
                    next_node = random.choice(neighbors)
                    walk.append(next_node)
                else:
                    break
            walks.append([str(node) for node in walk])
    return walks

# Function to train Word2Vec model
def train_word2vec_model(walks, vector_size, window_size, min_count, workers, sg, batch_words, epochs):
    """
    Trains a Word2Vec model on the provided walks.

    Parameters:
    - walks: List of lists containing random walks
    - vector_size: Dimensionality of the word vectors
    - window_size: Maximum distance between the current and predicted word within a sentence
    - min_count: Ignores all words with total frequency lower than this
    - workers: Number of worker threads to train the model
    - sg: Training algorithm. 1 for skip-gram; otherwise CBOW
    - batch_words: Number of words to be sampled for each batch
    - epochs: Number of iterations over the corpus

    Returns:
    - Word2Vec model
    """
    model = Word2Vec(walks, vector_size=vector_size, window=window_size, min_count=min_count, workers=workers, sg=sg, batch_words=batch_words, epochs=epochs)
    return model

# Function to apply PCA for dimensionality reduction
def apply_pca(node_embeddings, n_components):
    """
    Applies PCA to reduce the dimensionality of the embeddings.

    Parameters:
    - node_embeddings: Dictionary containing node embeddings
    - n_components: Number of components to keep

    Returns:
    - PCA model
    - Reduced embeddings
    """
    pca = PCA(n_components=n_components)
    embeddings = list(node_embeddings.values())
    reduced_embeddings = pca.fit_transform(embeddings)
    return pca, reduced_embeddings

# Function to cluster nodes using KMeans
def cluster_nodes(embeddings, num_clusters):
    """
    Clusters the nodes using KMeans algorithm.

    Parameters:
    - embeddings: Node embeddings
    - num_clusters: Number of clusters

    Returns:
    - Cluster labels
    """
    kmeans = KMeans(n_clusters=num_clusters, random_state=0).fit(embeddings)
    return kmeans.labels_

# Generate random walks
random_walks = generate_random_walks(G, num_walks, walk_length)

# Train Word2Vec model
word2vec_model = train_word2vec_model(random_walks, dimensions, window_size, min_count, workers, 1, batch_words, 50)

# Get node embeddings
node_embeddings = {str(node): word2vec_model.wv[str(node)] for node in G.nodes()}

# Apply PCA to reduce dimensionality to 2D
pca, reduced_embeddings = apply_pca(node_embeddings, 2)

# Cluster the nodes using KMeans on 2D embeddings
kmeans = KMeans(n_clusters=num_clusters, random_state=0).fit(reduced_embeddings)
cluster_labels = kmeans.labels_

# Assign cluster labels to nodes
for node, label in zip(G.nodes(), cluster_labels):
    G.nodes[node]['cluster'] = label

# Print the clusters
for node in G.nodes():
    print(f"Node {node} belongs to cluster {G.nodes[node]['cluster']}")

# Plot the clusters
colors = ['r', 'g', 'b']  # Red, Green, Blue for three clusters
plt.scatter(reduced_embeddings[:, 0], reduced_embeddings[:, 1], c=[colors[label] for label in cluster_labels])
plt.show()