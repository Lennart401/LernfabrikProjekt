# ----------------------------------------------------------------------------------------------------------------------
# Model training
# In this script, we will look at training a model using the tensorflow library.
# 
# Again, the first step is to import the required modules:
from util import plotter, io, paths, constants
from preprocessing import preprocessing, featureengineering
from machine_learning import training

import tensorflow as tf
import numpy as np

# ----------------------------------------------------------------------------------------------------------------------
# 1. Preprocessing
# We start by importing the samples and doing the known preprocessing on them and also immediate generating the features
all_movements = io.load_all_movements(paths.MOVEMENTS_PATH)
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
x, y = preprocessing.convert_dict_to_dataset(all_features)
print(x)
print(y)

# Next, the split the data into a training set and into a test set for model evaluation. We use a train size of 70%
# and a random state for reproducable results. Feel free to change these values to whatever you need.
x_train, x_test, y_train, y_test = preprocessing.split_train_test(x, y, train_size=0.7, random_state=10, stratify=y)
print(x_test)

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
y_train_enc = preprocessing.one_hot_encode_labels(y_train, categories=[list(constants.LUT_INDEX_TO_NAME.keys())])
y_test_enc = preprocessing.one_hot_encode_labels(y_test, categories=[list(constants.LUT_INDEX_TO_NAME.keys())])
print(y_test_enc)

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
    tf.keras.layers.Input(shape=[len(x_train[0])]),
    tf.keras.layers.Dense(7, activation=tf.keras.activations.relu),
    tf.keras.layers.Dense(len(y_train_enc[0]), activation=tf.keras.activations.softmax)
])

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
# TODO: set learning rate to something more reasonable.
# 
# Next, we fit the model to our training data. To get a stable model, we train multiple models and take the best one.
# This ensures that we do not mistake a good model for a worse one, just because the score is bad by random.
# 
# The function returns the fitted model and the training history for that model.
model, history = training.train_stable_model(model=model,
                                             optimizer=tf.keras.optimizers.Adam(0.07),
                                             loss=tf.keras.losses.CategoricalCrossentropy(),
                                             metrics=['accuracy'],
                                             x_train=x_train,
                                             y_train_enc=y_train_enc,
                                             epochs=30,
                                             optimize='train',
                                             n=3)

# The models are trained over 30 epochs and function will return the best out of n=3 models based on the training score.

# ----------------------------------------------------------------------------------------------------------------------
# 3. Model Evaluation
# Let's look at the model evaluation on the training data:
model.evaluate(x_train, y_train_enc, verbose=2)

# And let's also evaluate the model on the test data:
model.evaluate(x_test, y_test_enc, verbose=2)

# Finally, let's create some plots for the model. Will be creating plot model history and a confusion matrix. Let's
# start by creating the confusion matrix. We will need to un-one-hot-encode the labels in order to create a confusion
# matrix:
pred_train = [np.argmax(x) for x in model.predict(x_train)]
pred_test = [np.argmax(x) for x in model.predict(x_test)]

cm_train = tf.math.confusion_matrix(y_train, pred_train)
cm_test = tf.math.confusion_matrix(y_test, pred_test)

plotter.plot_confusion_matrix(cm_train, classes=constants.LUT_MOVEMENT_ID_TO_NAME.values())
plotter.plot_confusion_matrix(cm_test, classes=constants.LUT_MOVEMENT_ID_TO_NAME.values())

# Last but not least, let's create a plot of the model's history, to see if the need more/less epochs.
plotter.plot_model_history(history, num_epochs=30, use_validation_values=False)
