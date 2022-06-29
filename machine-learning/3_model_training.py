# ----------------------------------------------------------------------------------------------------------------------
# Model training
# In this script, we will look at training a model using the tensorflow library.
# 
# Again, the first step is to import the required modules:
import numpy as np
import tensorflow as tf

from machine_learning import training
from preprocessing import preprocessing, featureengineering
from util import plotter, io, paths, constants

# ----------------------------------------------------------------------------------------------------------------------
# Settings
RESAMPLE = True
EPOCHS = 25

PRINT_INTERMEDIATE_RESULTS = False

# ----------------------------------------------------------------------------------------------------------------------
# 1. Preprocessing
# We start by importing the samples and doing the known preprocessing on them and also immediate generating the features
all_movements = io.load_all_movements(paths.MOVEMENTS_PATH)

if RESAMPLE:
    all_movements = preprocessing.resample_dataset(all_movements)

all_movements = preprocessing.normalize_time(all_movements)
all_features = featureengineering.generate_basic_features(all_movements)

# Next, we convert the movement-type ordered dictionary to two numpy array: x and y.
# 
# x is a 2d array in the shape of `( len(observations), len(features) )` and y is the label array with a shape of
# `( len(observations), )`.
# 
# Note that call convert_dict_to_dataset will translate the movement type id into the 'array index form' using a lookup
# table. This has the benefit that no matter what movement types we use, the model will see n classes from 0 to n-1.
# After a model prediction, you would apply the sample LUT in reverse to extract the actual movement type predicted.
X, y = preprocessing.convert_dict_to_dataset(all_features)
if PRINT_INTERMEDIATE_RESULTS:
    print(X)
    print(y)

# Let's do some intermediate cleanup:
del all_movements, all_features

# Next, the split the data into a training set and into a test set for model evaluation. We use a train size of 70%
# and a random state for reproducable results. Feel free to change these values to whatever you need.
X_train, X_test, y_train, y_test = preprocessing.split_train_test(X, y, train_size=0.7, random_state=10, stratify=y)
if PRINT_INTERMEDIATE_RESULTS:
    print(X_test)

# Next, we split the training data into the actual training set and a validation set, used for training the model and
# checking the score.
X_train, X_val, y_train, y_val = preprocessing.split_train_test(X_train, y_train, train_size=0.7, random_state=10,
                                                                stratify=y_train)

# Before we one-hot-encode our labels, let's have a look at another problem in our data: there is a strong class
# imbalance. This is not so much a problem for our test data but very much so for our training data. These are the
# number of samples for each class:
if PRINT_INTERMEDIATE_RESULTS:
    classes, counts = np.unique(y_train, return_counts=True)
    for i in classes:
        print(f'Class "{constants.LUT_INDEX_TO_NAME[i]}" ({i}): {counts[i]}')

# What we can do now it to and upsample all classes to the one with the most samples:
X_train, y_train = preprocessing.oversample_dataset(X_train, y_train)

# Now let's look at the counts again (but a bit less verbose):
if PRINT_INTERMEDIATE_RESULTS:
    print(np.unique(y_train, return_counts=True)[1])

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
if PRINT_INTERMEDIATE_RESULTS:
    print(y_test_enc)

# Again, some cleanup:
del X
del y, y_train, y_val, y_test

# ----------------------------------------------------------------------------------------------------------------------
# 2. Machine Learning
# Now it's time to do some actual machine learning using tensorflow and keras.
# 
# We start by creating a basic keras model: the first layer is just our input layer with the shape of the training data
# (we can just use the first one), then we add one dense layer with seven nodes and finally, the output layer is another
# dense layer with as many outputs as we have classes in our labels.
# 
# The second layer has rectified linear unit activation function (i.e. f(x) = max(0, x)) and the last layer has a
# softmax activation function. This means that the probabilities of all classes add up to one.
model = tf.keras.Sequential([
    tf.keras.layers.Input(shape=[len(X_train[0])]),
    tf.keras.layers.Dense(10, activation=tf.keras.activations.relu),
    tf.keras.layers.Dense(15, activation=tf.keras.activations.relu),
    tf.keras.layers.Dense(len(y_train_enc[0]), activation=tf.keras.activations.softmax)
])

# Let's define some constants for training the model:
OPTIMIZER = tf.keras.optimizers.Adam()
LOSS = tf.keras.losses.CategoricalCrossentropy()
METRICS = ['accuracy']

# Optionally, we can run the learning rate optimizer first to find the best learning rate. We need more epochs to find
# the best learning rate. This may need adjusting, training should stop when the learning rate gets to around 10 or so
# in order to create a clean graph.
lr_history = training.optimize_learning_rate(model,
                                             optimizer=OPTIMIZER,
                                             loss=LOSS,
                                             metrics=METRICS,
                                             x_train=X_train,
                                             y_train_enc=y_train_enc,
                                             epochs=90,
                                             minimum_lr=1e-8)

# Plot the learning rate history and look for the first minimum:
plotter.plot_learning_rate(lr_history)

# Based on that graph, we can choose the best learning rate:
LEARNING_RATE = 0.01
OPTIMIZER = tf.keras.optimizers.Adam(learning_rate=LEARNING_RATE)

# Cleanup the learning rate optimization variables:
del lr_history

# To create a usuable model, we have to compile it and provide tensorflow with an optimizer, a loss function and
# specify, which training metric to use. We will use an Adam optimizer and a Categeorical-Crossentropy loss function.
# For simplicity, we will use the metric 'accuracy'.
# 
# This would be done using the following code for a single model, but we want something more sophisticated.
# 
#     model.compile(optimizer=tf.keras.optimizers.Adam(0.07),
#                   loss=tf.keras.losses.CategoricalCrossentropy(),
#                   metrics=['accuracy'])
# 
# Next, we fit the model to our training data. To get a stable model, we train multiple models and take the best one.
# This ensures that we do not mistake a good model for a worse one, just because the score is bad by random.
# 
# The function returns the fitted model and the training history for that model.
model, history = training.train_stable_model(model=model,
                                             optimizer=OPTIMIZER,
                                             loss=LOSS,
                                             metrics=METRICS,
                                             x_train=X_train,
                                             y_train_enc=y_train_enc,
                                             epochs=EPOCHS,
                                             x_test=X_test,
                                             y_test_enc=y_test_enc,
                                             x_val=X_val,
                                             y_val_enc=y_val_enc,
                                             optimize='test',
                                             n=10)

# The models are trained over 50 epochs and function will return the best out of n=10 models based on the training
# score.

# ----------------------------------------------------------------------------------------------------------------------
# 3. Model Evaluation
# Let's look at the model evaluation on the training data:
print('Train evaluation:', model.evaluate(X_train, y_train_enc, verbose=2))

# And let's also evaluate the model on the test data:
print('Test evaluation:', model.evaluate(X_test, y_test_enc, verbose=2))

# Finally, let's create some plots for the model. Will be creating plot model history and a confusion matrix. Let's
# start by creating the confusion matrix. We will need to un-one-hot-encode the labels in order to create a confusion
# matrix:
pred_train = [np.argmax(x) for x in model.predict(X_train)]
pred_test = [np.argmax(x) for x in model.predict(X_test)]

cm_train = tf.math.confusion_matrix(y_train, pred_train)
cm_test = tf.math.confusion_matrix(y_test, pred_test)

plotter.plot_confusion_matrix(cm_train, classes=constants.LUT_MOVEMENT_ID_TO_NAME.values())
plotter.plot_confusion_matrix(cm_test, classes=constants.LUT_MOVEMENT_ID_TO_NAME.values())

# Last but not least, let's create a plot of the model's history, to see if the need more/less epochs.
plotter.plot_model_history(history, num_epochs=EPOCHS, use_validation_values=True)

if input("save model? ") == "y":
    io.save_model(model, "./tmp/6_classes")
