from numpy import ndarray
import numpy as np
import tensorflow as tf
import copy


def train_stable_model(model: tf.keras.Model, x_train: ndarray, y_train_enc: ndarray, epochs: int,
                       x_test: ndarray = None, y_test_enc: ndarray = None, optimize: str = "train", n: int = 3) \
        -> tf.keras.Model:
    """
    Train a model n times and return the best model.

    :param model: The model to be fitted to the data
    :param x_train: Training data (x)
    :param y_train_enc: Training labels, one-hot-encoded (y)
    :param epochs: number of epochs to train the model for
    :param x_test: (optional) Test data (x)
    :param y_test_enc: (optional) Test labels, one-hot-encoded (y)
    :param optimize: 'train' to use the training score or 'test' to use the test score. x_test and y_test_enc have to
     be provided when using 'test'. the first metric of the model will be used (e.g. accuracy)
    :param n: how many models to train
    :return: The model with the best metric
    """

    # verify that optimize is either 'train' or 'test'
    if not (optimize == "train" or optimize == "test"):
        raise ValueError("Parameter optimize must be either 'train' or 'test'")

    # verify that x_test and y_test_enc are provided if optimize='test'
    if optimize == "test" and (x_test is None or y_test_enc is None):
        raise ValueError("When using optimize='test', both x_test and y_test_enc must be provided (not None)")

    stable_model = None
    best_score = 0.0

    # train n models
    for i in range(n):
        # create a copy of the original model and train it
        temp_model = copy.deepcopy(model)
        temp_model.fit(x_train, y_train_enc, epochs=epochs)

        # get the scores for the set optimize parameter and overwrite the best model if the accuracy is better than the
        # one from the previous best model
        scores = model.evaluate(x_train, y_train_enc) if optimize == "train" else model.evaluate(x_test, y_test_enc)
        if scores[1] > best_score:
            stable_model = temp_model
            best_score = scores[1]

    return stable_model


