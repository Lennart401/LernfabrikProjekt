# Preprocess the data and save it to a csv file.
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
from sklearn.feature_selection import SelectKBest, f_classif

from preprocessing import preprocessing, featureengineering
from util import io, paths


# ----------------------------------------------------------------------------------------------------------------------
# Loading the data from their raw file structure
raw_movements = io.load_all_movements(paths.MOVEMENTS_PATH)


# ----------------------------------------------------------------------------------------------------------------------
# Optionally, resample the data
# - After resampling, each sample is 100 data points long
# - A new sample will be created every 10 data points, until all data is used
SAMPLE_LENGTH = 128
all_movements = preprocessing.resample_dataset(raw_movements,
                                               sample_length=SAMPLE_LENGTH,
                                               resample_offset=16)

# ----------------------------------------------------------------------------------------------------------------------
# Normalize the time
all_movements = preprocessing.normalize_time(all_movements)

# ----------------------------------------------------------------------------------------------------------------------
# Synthesize transition samples
synthesize_samples = preprocessing.resample_dataset(raw_movements, sample_length=SAMPLE_LENGTH, resample_offset=124)
synthesize_samples = preprocessing.normalize_time(synthesize_samples)

transitions = np.array([
    [1, 5], [5, 1],
    [2, 5], [5, 2],
    [4, 5], [5, 4],
    [6, 5], [5, 6],
    # [3, 4], [4, 3],
])
transition_movements = preprocessing.synthesize_transition_samples(synthesize_samples,
                                                                   transitions=transitions,
                                                                   splits=[0.9, 0.8, 0.7, 0.6],
                                                                   drop=0.9)

# merge transiton_movements into all_movements
for movement_type in all_movements:
    all_movements[movement_type].extend(transition_movements[movement_type])

# ----------------------------------------------------------------------------------------------------------------------
# Convert the samples into observations by generating features that describe the data. We will generate all possible
# features and thus not specify which ones we want to use.
HZ = 100
all_features = featureengineering.generate_selected_features(all_movements,
                                                             signal_length=SAMPLE_LENGTH,
                                                             samples_per_seconds=HZ)

# ----------------------------------------------------------------------------------------------------------------------
# Let's create a xy dataset from our features, each column of the features becomes a column in the X-matrix
X, y, names = preprocessing.convert_dict_to_dataset(all_features)

# ----------------------------------------------------------------------------------------------------------------------
# Scaling
# This is necessary to use a PCA, as the PCA is sensitive to the scale of the data.
scaler = StandardScaler()
scaler.fit(X)  # can be reused later
X_scaled = scaler.transform(X)

# optionally, save the data of the scaler for later use
print(scaler.scale_)
print(scaler.mean_)

# ----------------------------------------------------------------------------------------------------------------------
# PCA
pca = PCA()
pca.fit(X_scaled)
X_pca = pca.transform(X_scaled)
pca_names = np.array([f'pca_{str(i)}' for i in range(len(pca.components_))])

# Plot the explained variance of the pca components
plt.bar(np.arange(0, len(pca.explained_variance_ratio_)), pca.explained_variance_ratio_)

# Plot the cumulative explained variance of the pca components
plt.bar(np.arange(0, len(pca.explained_variance_ratio_)), pca.explained_variance_ratio_.cumsum())

# How many components do we need for 98% of the variance?
np.argmax(pca.explained_variance_ratio_.cumsum() >= 0.98)

# ----------------------------------------------------------------------------------------------------------------------
# Concat X_scaled and X_pca
X_full = np.concatenate((X_scaled, X_pca), axis=1)
all_names = np.concatenate((names, pca_names), axis=0)

# ----------------------------------------------------------------------------------------------------------------------
# Feature selection, X_full, k=10
selector_k10 = SelectKBest(f_classif, k=10)
selector_k10.fit(X_full, y)
X_selected_k10 = selector_k10.transform(X_full)

# What features were selected?
selector_k10.get_feature_names_out(all_names)

# k=15
selector_k15 = SelectKBest(f_classif, k=15)
selector_k15.fit(X_full, y)
X_selected_k15 = selector_k15.transform(X_full)

selector_k15.get_feature_names_out(all_names)

# ----------------------------------------------------------------------------------------------------------------------
# Feature selection, X_scaled, k=10
selector_scaled_k10 = SelectKBest(f_classif, k=10)
selector_scaled_k10.fit(X_scaled, y)
X_scaled_selected_k10 = selector_scaled_k10.transform(X_scaled)

# What features were selected?
selector_scaled_k10.get_feature_names_out(names)

# k=15
selector_scaled_k15 = SelectKBest(f_classif, k=15)
selector_scaled_k15.fit(X_scaled, y)
X_scaled_selected_k15 = selector_scaled_k15.transform(X_scaled)

selector_scaled_k15.get_feature_names_out(names)

# ----------------------------------------------------------------------------------------------------------------------
# Save some data frames
selected_k10_df = pd.DataFrame(X_selected_k10, columns=selector_k10.get_feature_names_out(all_names))
selected_k10_df['movement_type'] = y
io.save_features(selected_k10_df, 'selected_k10.csv')

selected_k15_df = pd.DataFrame(X_selected_k15, columns=selector_k15.get_feature_names_out(all_names))
selected_k15_df['movement_type'] = y
io.save_features(selected_k15_df, 'selected_k15.csv')

scaled_selected_k10_df = pd.DataFrame(X_scaled_selected_k10, columns=selector_scaled_k10.get_feature_names_out(names))
scaled_selected_k10_df['movement_type'] = y
io.save_features(scaled_selected_k10_df, 'scaled_selected_k10.csv')

scaled_selected_k15_df = pd.DataFrame(X_scaled_selected_k15, columns=selector_scaled_k15.get_feature_names_out(names))
scaled_selected_k15_df['movement_type'] = y
io.save_features(scaled_selected_k15_df, 'scaled_selected_k15.csv')

pca_df = pd.DataFrame(X_pca, columns=pca_names)
pca_df['movement_type'] = y
io.save_features(pca_df, 'pca.csv')

full_df = pd.DataFrame(X_full, columns=all_names)
full_df['movement_type'] = y
io.save_features(full_df, 'full.csv')

# ----------------------------------------------------------------------------------------------------------------------
# Save metadata
io.save_metadata(scaler.scale_, scaler.mean_, pca.components_)
