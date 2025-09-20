import numpy as np
import pandas as pd
import tensorflow as tf
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import MinMaxScaler
from sklearn.metrics import f1_score

# Load dataset
data = pd.read_csv("/home/cselab/Desktop/fraud_analytics/creditcard.csv")

# Drop the 'Time' column
data.drop('Time', axis=1, inplace=True)

# Convert amount feature into log scale
data['Log_Amount'] = np.log1p(data['Amount'])

# Apply Min-Max scaling to 'Log_Amount'
scaler = MinMaxScaler()
data['Log_Amount'] = scaler.fit_transform(data[['Log_Amount']])

print(data['Log_Amount'])

# Step 2: Model Architecture
input_dim = data.shape[1] - 1  # Excluding the target variable 'Class'

# Define the autoencoder architecture
input_layer = tf.keras.layers.Input(shape=(input_dim,))
encoded = tf.keras.layers.Dense(30, activation='relu')(input_layer)
encoded = tf.keras.layers.Dense(20, activation='relu')(encoded)  # Additional encoding layer
encoded = tf.keras.layers.Dense(15, activation='relu')(encoded)  # Additional encoding layer
encoded = tf.keras.layers.Dense(10, activation='relu')(encoded)  # Additional encoding layer

decoded = tf.keras.layers.Dense(15, activation='relu')(encoded)  # Additional decoding layer
decoded = tf.keras.layers.Dense(20, activation='relu')(decoded)  # Additional decoding layer
decoded = tf.keras.layers.Dense(30, activation='relu')(decoded)  # Additional decoding layer

output_layer = tf.keras.layers.Dense(input_dim, activation='sigmoid')(decoded)

autoencoder = tf.keras.models.Model(input_layer, output_layer)

# Compile the model
autoencoder.compile(optimizer='rmsprop', loss='mean_squared_error')

# Step 3: Training
# Separate fraudulent transactions for stratified sampling
X_train_nf, X_test_nf = train_test_split(data[data['Class'] == 0], test_size=0.01, random_state=42)

# Sample an equal number of fraudulent transactions for training and testing
fraudulent = data[data['Class'] == 1]
fraudulent_train, fraudulent_test = train_test_split(fraudulent, test_size=0.35, random_state=42)

# Concatenate the non-fraudulent and sampled fraudulent transactions for training
X_train = pd.concat([X_train_nf, fraudulent_train])

# Concatenate the remaining fraudulent transactions and the non-fraudulent test set for testingclear
X_test = pd.concat([X_test_nf, fraudulent_test])

# Shuffle the training and testing sets
X_train = X_train.sample(frac=1, random_state=42).reset_index(drop=True)
X_test = X_test.sample(frac=1, random_state=42).reset_index(drop=True)

# Remove the 'Class' column from training and testing sets
X_train = X_train.drop(['Class'], axis=1)
X_test = X_test.drop(['Class'], axis=1)

# Step 4: Training
autoencoder.fit(X_train, X_train, epochs=5, batch_size=16, shuffle=True, validation_data=(X_test, X_test))

# Step 5: Evaluation
# Reconstruction error for non-fraudulent transactions
non_fraudulent_recon = autoencoder.evaluate(X_test, X_test)

# Reconstruction error for fraudulent transactions
fraudulent_recon = autoencoder.evaluate(fraudulent_test.drop(['Class'], axis=1), fraudulent_test.drop(['Class'], axis=1))

# Step 6: Calculate F1 score
# Prediction score
threshold = 0.9
predicted_non_fraudulent = autoencoder.predict(X_test)
predicted_fraudulent = autoencoder.predict(fraudulent_test.drop(['Class'], axis=1))

# Assigning labels based on reconstruction error
predicted_labels_non_fraudulent = (np.linalg.norm(X_test - predicted_non_fraudulent, axis=1) > threshold).astype(int)
predicted_labels_fraudulent = (np.linalg.norm(fraudulent_test.drop(['Class'], axis=1) - predicted_fraudulent, axis=1) > threshold).astype(int)

# Combining predictions with actual labels
y_true = np.concatenate([np.zeros(len(X_test)), np.ones(len(fraudulent_test))])
y_pred = np.concatenate([predicted_labels_non_fraudulent, predicted_labels_fraudulent])

# Calculate F1 score
f1 = f1_score(y_true, y_pred)

print("Reconstruction error for non-fraudulent transactions:", non_fraudulent_recon)
print("Reconstruction error for fraudulent transactions:", fraudulent_recon)
print("F1 Score:", f1)