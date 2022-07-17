from typing import Tuple, List, Union, Any

from numpy import ndarray
import tensorflow as tf


def train_stable_model(model: tf.keras.Model,
                       optimizer: Union[str, tf.keras.optimizers.Optimizer],
                       loss: Union[str, tf.keras.losses.Loss],
                       metrics: Any,
                       x_train: ndarray,
                       y_train_enc: ndarray,
                       epochs: int,
                       x_test: ndarray = None,
                       y_test_enc: ndarray = None,
                       x_val: ndarray = None,
                       y_val_enc: ndarray = None,
                       optimize: str = "train",
                       n: int = 3,
                       callbacks: list = None) \
        -> Tuple[tf.keras.Model, tf.keras.callbacks.History]:
    """
    Train a model n times and return the best model.

    :param model: The model to be fitted to the data. The model should not be compiled yet
    :param optimizer: the optimizer to compile the model with. will be used for model.compile(...)
    :param loss: the loss function to compile the model with. will be used for model.compile(...)
    :param metrics: a list of metric to compile the model with. will be used for model.compile(...)
    :param x_train: Training data (x)
    :param y_train_enc: Training labels, one-hot-encoded (y)
    :param epochs: number of epochs to train the model for
    :param x_test: (optional) Test data (x)
    :param y_test_enc: (optional) Test labels, one-hot-encoded (y)
    :param x_val: (optional) Validation data (x). Will be used if provided
    :param y_val_enc: (optional) Validation data, one-hot-encoded (y). Will be used if provided
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

    # verify that x_val and y_val_enc are provided if optimize='val'
    if optimize == 'val' and (x_val is None or y_val_enc is None):
        raise ValueError('When using optimize=\'val\', both x_val and y_val_enc must be provided (not None)')

    stable_model = None
    stable_history = None
    best_score = 0.0

    # train n models
    for i in range(n):
        print(f'Training model {i+1} of {n} models...')

        # create a copy of the original model and train it
        temp_model = tf.keras.models.clone_model(model)
        temp_model.compile(optimizer=optimizer, loss=loss, metrics=metrics)

        if x_val is None:
            temp_history = temp_model.fit(x_train, y_train_enc, epochs=epochs, callbacks=callbacks)
        else:
            temp_history = temp_model.fit(x_train, y_train_enc, epochs=epochs, validation_data=(x_val, y_val_enc),
                                          callbacks=callbacks)

        # get the scores for the set optimize parameter and overwrite the best model if the accuracy is better than the
        # one from the previous best model
        if optimize == 'train':
            scores = temp_model.evaluate(x_train, y_train_enc)
        elif optimize == 'val':
            scores = temp_model.evaluate(x_val, y_val_enc)
        else:  # optimize == 'test'
            scores = temp_model.evaluate(x_test, y_test_enc)

        print(scores)
        if scores[1] > best_score:
            stable_model = temp_model
            stable_history = temp_history
            best_score = scores[1]

    return stable_model, stable_history


def optimize_learning_rate(model: tf.keras.Model,
                           optimizer: Union[str, tf.keras.optimizers.Optimizer],
                           loss: Union[str, tf.keras.losses.Loss],
                           metrics: Any,
                           x_train: ndarray,
                           y_train_enc: ndarray,
                           epochs: int,
                           minimum_lr: float = 1e-8) -> tf.keras.callbacks.History:
    """
    Optimizes the learning rate using a LearningRateScheduler, which gradually increases the learning rate.

    This function creates a model history containing the learning rate vs loss. Use a plotting function like
    plotter.plot_learning_rate to find the optimal minimum and what learning rate to use.

    :param model: the model to train. should not be compiled yet. a copy will be made with is then compiled.
    :param optimizer: the optimizer to use. if specifying the learning rate, use minimum_lr as default
    :param loss: the loss function to use.
    :param metrics: the metric to use.
    :param x_train: Training data (x)
    :param y_train_enc: Training labels, one-hot-encoded (y)
    :param epochs: number of epochs to train the model for
    :param minimum_lr: the learning rate where to start. will be increased by a factor of 10 every 10 epochs
    :return: the history of the trained model.
    """
    temp_model = tf.keras.models.clone_model(model)
    temp_model.compile(optimizer=optimizer, loss=loss, metrics=metrics)

    # TODO implement early stopping once the loss becomes bigger than 15?
    lr_scheduler = tf.keras.callbacks.LearningRateScheduler(schedule=lambda epoch: minimum_lr * 10 ** (epoch / 10))
    temp_history = temp_model.fit(x_train, y_train_enc, epochs=epochs, callbacks=[lr_scheduler])
    return temp_history
