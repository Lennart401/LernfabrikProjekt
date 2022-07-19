# ----------------------------------------------------------------------------------------------------------------------
# Model training
# In this script, we will look at training a model using the tensorflow library.
# 
# Again, the first step is to import the required modules:
import keras
import numpy as np
import tensorflow as tf
import tensorflow_addons as tfa
import keras_tuner as kt

from sklearn.metrics import confusion_matrix
from tensorflow import keras

from machine_learning import training
from preprocessing import preprocessing
from util import plotter, io, constants


# ----------------------------------------------------------------------------------------------------------------------
# 1. Load the dataset
#
# We load one of the preprocessed datasets from the previous script.
dataset = io.load_features('selected_k15.csv')
X = dataset.drop(columns=['movement_type']).to_numpy()
y = dataset['movement_type'].to_numpy()

# For the PCA dataset, drop some more columns:
X = dataset.drop(columns=['movement_type', 'pca_16', 'pca_17', 'pca_18', 'pca_19', 'pca_20', 'pca_21', 'pca_22',
                          'pca_23', 'pca_24', 'pca_25', 'pca_26', 'pca_27']).to_numpy()

# Next, the split the data into a training set and into a test set for model evaluation. We use a train size of 70%
# and a random state for reproducable results. Feel free to change these values to whatever you need.
X_train, X_test, y_train, y_test = preprocessing.split_train_test(X, y, train_size=0.7, random_state=10, stratify=y)

# Next, we split the training data into the actual training set and a validation set, used for training the model and
# checking the score.
X_train, X_val, y_train, y_val = preprocessing.split_train_test(X_train, y_train, train_size=0.7, random_state=10,
                                                                stratify=y_train)

# ----------------------------------------------------------------------------------------------------------------------
# 1.1. Class imbalance (optional)
#
# Have a look at the number of samples per class. If there is a significant imbalance, we can oversample the dataset by
# using the SMOTE algorithm.
classes, counts = np.unique(y_train, return_counts=True)
for i in classes:
    print(f'Class "{constants.LUT_INDEX_TO_NAME[i]}" ({i}): {counts[i]}')

# Use SMOTE to oversample the dataset.
X_train, y_train = preprocessing.oversample_dataset(X_train, y_train)

# Now let's look at the counts again (but a bit less verbose):
print(np.unique(y_train, return_counts=True)[1])

# ----------------------------------------------------------------------------------------------------------------------
# 1.2. One-hot encoding
#
# Each element in the y-arrays is the movement type as an integer. However, we don't want our labels in this form.
# For the model, we need our labels to be one-hot encoded. In this case, this means that every 'label' is an array of
# the length of the possible classes (number of movement types). All elements of this array are zero, except for the
# corresponding class.
# 
# An example: we have a sample with movement-type 4 (no-movement). This translates to index 3 from the movement type to
# index lookup-table. There are 8 classes of samples in total. The resulting one-hot encoded label would look like this:
# `[ 0.  0.  0.  1.  0.  0.  0.  0. ]`
#
# The categories for our encoder are the keys from the indices look-up table in the shape of (n_features,) (but as list
# and not a tuple). So with e.g. 8 classes, the categories are: [[0, 1, 2, 3, 4, 5, 6, 7]]
# 
# So let's one-hot-encode all the labels
categories = [list(constants.LUT_INDEX_TO_NAME.keys())]
y_train_enc = preprocessing.one_hot_encode_labels(y_train, categories=categories)
y_val_enc = preprocessing.one_hot_encode_labels(y_val, categories=categories)
y_test_enc = preprocessing.one_hot_encode_labels(y_test, categories=categories)


# ----------------------------------------------------------------------------------------------------------------------
# 2. Machine Learning
#
# Now it's time to do some actual machine learning using tensorflow and keras.
# 
# We start by creating a basic keras model: the first layer is just our input layer with the shape of the training data
# (we can just use the first one). The output layer is another dense layer with as many outputs as we have classes. In
# between, there are a couple of dense layers.

# b_model = baseline_model
b_model = tf.keras.Sequential([
    tf.keras.layers.Input(shape=[len(X_train[0])]),
    tf.keras.layers.Dense(28, activation=tf.keras.activations.relu),
    tf.keras.layers.Dropout(0.1),
    # tf.keras.layers.Dense(15, activation=tf.keras.activations.relu),
    tf.keras.layers.Dense(len(y_train_enc[0]), activation=tf.keras.activations.softmax)
])

# Let's define some constants for training the model:
OPTIMIZER = tf.keras.optimizers.Adam()
LOSS = tf.keras.losses.CategoricalCrossentropy()
METRICS = [tfa.metrics.F1Score(num_classes=len(y_train_enc[0]), average='macro')]
STOP_EARLY = tf.keras.callbacks.EarlyStopping(monitor='val_loss', patience=5)
EPOCHS = 35

# ----------------------------------------------------------------------------------------------------------------------
# 2.1. Learning Rate Optimization
#
# Optionally, we can run the learning rate optimizer first to find the best learning rate. We need more epochs to find
# the best learning rate. This may need adjusting, training should stop when the learning rate gets to around 10 or so
# in order to create a clean graph.
lr_history = training.optimize_learning_rate(b_model,
                                             optimizer=OPTIMIZER,
                                             loss=LOSS,
                                             metrics=METRICS,
                                             x_train=X_train,
                                             y_train_enc=y_train_enc,
                                             epochs=90,
                                             minimum_lr=1e-8)

# Plot the learning rate history and look for the first minimum:
plotter.plot_learning_rate(lr_history)

# ----------------------------------------------------------------------------------------------------------------------
# 2.2. Fitting the model
#
# Based on the graph from the learning rate scheduler, we can choose the best learning rate:
LEARNING_RATE = 0.01
OPTIMIZER = tf.keras.optimizers.Adam(learning_rate=LEARNING_RATE)

# To create model, we have to compile it and provide tensorflow with an optimizer, a loss function and specify, which
# training metric to use. We will use an Adam optimizer and a Categeorical-Crossentropy loss function. We will use the
# F1 score as a metric.
# 
# As the model's weights are initialized randomly, there is always some inconsitency in the results. By training
# multiple model with the same parameters, we can get a more stable and accurate result.
b_model, history = training.train_stable_model(model=b_model,
                                               optimizer=OPTIMIZER,
                                               loss=LOSS,
                                               metrics=METRICS,
                                               epochs=EPOCHS,
                                               x_train=X_train,
                                               y_train_enc=y_train_enc,
                                               x_test=X_test,
                                               y_test_enc=y_test_enc,
                                               x_val=X_val,
                                               y_val_enc=y_val_enc,
                                               optimize='test',
                                               callbacks=[STOP_EARLY],
                                               n=3)

# ----------------------------------------------------------------------------------------------------------------------
# 3. Model Evaluation
#
# Let's evaluate the model in raw numbers:
b_model.evaluate(X_train, y_train_enc, verbose=2)
b_model.evaluate(X_test, y_test_enc, verbose=2)

# Build a confusion matrix to have a visual look at the results:
b_pred_train = [np.argmax(x) for x in b_model.predict(X_train)]
b_pred_test = [np.argmax(x) for x in b_model.predict(X_test)]

confusion_matrix(y_train, b_pred_train)
confusion_matrix(y_test, b_pred_test)

# Optional: matplotlib confusion matrix
cm_train = tf.math.confusion_matrix(y_train, b_pred_train)
cm_test = tf.math.confusion_matrix(y_test, b_pred_test)

# TODO rewrite, plot_confusion_matrix should only take y_true and y_pred
plotter.plot_confusion_matrix(cm_train, classes=constants.LUT_MOVEMENT_ID_TO_NAME.values())
plotter.plot_confusion_matrix(cm_test, classes=constants.LUT_MOVEMENT_ID_TO_NAME.values())

# Last but not least, create a plot of the model's history, to see if the need more/less epochs.
plotter.plot_model_history(history, num_epochs=EPOCHS, use_validation_values=True)

io.save_model(b_model, "./tmp/model_k15")

# ----------------------------------------------------------------------------------------------------------------------
# 4. Improving the model


# Use keras tuner to find the best parameters for the model
def build_model(hp):
    model = keras.Sequential()
    # Input layer
    model.add(tf.keras.layers.Input(shape=[len(X_train[0])]))

    # Hidden layers
    for j in range(0, hp.Int('num_layers', 1, 2)):
        model.add(tf.keras.layers.Dense(
            units=hp.Int(f'units_{j}', min_value=10, max_value=30, step=3),
            activation=tf.keras.activations.relu)
        )
        model.add(tf.keras.layers.Dropout(hp.Float(f'dropout_{j}', 0.0, 0.3, step=0.1)))

    # Output layer
    model.add(tf.keras.layers.Dense(len(y_train_enc[0]), activation=tf.keras.activations.softmax))

    # Learning rate
    hp_learning_rate = hp.Choice('learning_rate', [1e-2, 1e-3])

    model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=hp_learning_rate),
                  loss=LOSS,
                  metrics=METRICS)
    return model


tuner = kt.Hyperband(build_model,
                     objective=kt.Objective('val_f1_score', direction='max'),
                     max_epochs=EPOCHS,
                     factor=3,
                     hyperband_iterations=10,
                     directory='./tmp/tuner',
                     project_name='test3')

tuner.search_space_summary()

STOP_EARLY = tf.keras.callbacks.EarlyStopping(monitor='val_loss', patience=5)
tuner.search(X_train, y_train_enc, epochs=EPOCHS, validation_data=(X_val, y_val_enc), callbacks=[STOP_EARLY])

best_hps = tuner.get_best_hyperparameters()[0]
h_model = tuner.hypermodel.build(best_hps)
STOP_EARLY = tf.keras.callbacks.EarlyStopping(monitor='val_loss', patience=5)
h_model.fit(X_train, y_train_enc, epochs=EPOCHS, validation_data=(X_val, y_val_enc), callbacks=[STOP_EARLY])

h_model.evaluate(X_train, y_train_enc, verbose=2)
h_model.evaluate(X_test, y_test_enc, verbose=2)

h_pred_train = [np.argmax(x) for x in h_model.predict(X_train)]
h_pred_test = [np.argmax(x) for x in h_model.predict(X_test)]

confusion_matrix(y_train, h_pred_train)
confusion_matrix(y_test, h_pred_test)

# using the first 17 pca components
io.save_model(h_model, "./tmp/model_tuned_k15")

