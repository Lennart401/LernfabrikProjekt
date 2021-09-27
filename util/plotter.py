from typing import List, Any, Dict

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sn
import tensorflow as tf

from matplotlib.lines import Line2D
from util import constants


def plot_movements_raw(movements: Dict[int, List[pd.DataFrame]], sensors: List[str] = None, index_col="time",
                       sharex="all", sharey="all") -> None:
    """
    Plot the raw movement data in a huge figure.

    Very important: normalize the time of the sample first for readability. There will be one row per movement type and
    one column for each sensor respectively.

    :param movements: dict of samples for each movement. each sample will be plotted.
    :param sensors: the sensors to plot, each of them will be one column. by default "accx", "accy" and "accz"
    :param index_col: what column of every sample to use as x-axis. "time" by default
    :param sharex: the x-axes to share. see pyplot documentation.
    :param sharey: the y-axes to share. see pyplot documentation.
    """
    # set default parameters for axes
    if sensors is None:
        sensors = ["accx", "accy", "accz"]
    # abort if no sensors are passed
    if len(sensors) == 0:
        raise ValueError(f"{plot_movements_raw.__name__}: no sensors have been passed, so nothing can be plotted. "
                         f"Please pass at least on sensors as a list to this function.")
    if len(movements) == 0:
        raise ValueError(f"{plot_movements_raw.__name__}: no movements have been passed, so nothing can be plotted. "
                         f"Please pass at least on movement with minimum one sample.")

    fig, axs = plt.subplots(nrows=len(movements), ncols=len(sensors), tight_layout=True, sharex=sharex, sharey=sharey)
    for row, movement_id in enumerate(movements):
        for sample in movements[movement_id]:
            for column, sensor in enumerate(sensors):
                axs[row, column].plot(sample[index_col], sample[sensor])
                # set the xlabel/title on the first row only
                if row == 0:
                    axs[row, column].set_title(sensor)
            # the set ylabel on the first column only
            axs[row, 0].set_ylabel(constants.LUT_MOVEMENT_ID_TO_NAME[movement_id])
    plt.show()


def plot_3_axis_feature(features: Dict[int, List[Dict[str, float]]], feature_name: str = "mean",
                        color_lookup_table: Dict[int, str] = None) -> None:
    """
    Plot 3-axis features as a colored 3D scatter plot.

    The features shall contain a list of dicts per movement type. The dictionarys in each list correspond to one sample/
    observations each. The feature-keys of the 3-axis features shall be named ``x_<feature name>``, ``y_<feature name>``
    and ``z_<feature name>``. The feature name maybe for example be "mean" or "std".

    This function does not check for empty lists/dicts.

    :param features: dictionary of features by movement type.
    :param feature_name: the name of the feature to plot
    :param color_lookup_table: lookup-table which maps movement type to color.
    """
    if color_lookup_table is None:
        color_lookup_table = constants.LUT_MOVEMENT_ID_TO_COLOR

    # create a 3d-figure
    fig = plt.figure(tight_layout=True)
    ax = fig.add_subplot(projection="3d")

    # custom legend: just one line for each movement (if there is at least one observation/sample)
    legend_elements = []

    for movement_id in features:
        # add each sample to the plot
        for samples in features[movement_id]:
            ax.scatter(samples[f"x_{feature_name}"],
                       samples[f"y_{feature_name}"],
                       samples[f"z_{feature_name}"],
                       color=color_lookup_table[movement_id])

        # only add the legend for the movement type if there is at least one sample/observation
        if len(features[movement_id]) > 0:
            # this is a white line and a marker of type "o" with the color form the LUT in the middle, which means only
            # the marker will be visible on the white background.
            legend_elements.append(Line2D([0], [0], marker="o", color="w",
                                          label=constants.LUT_MOVEMENT_ID_TO_NAME[movement_id],
                                          markerfacecolor=color_lookup_table[movement_id], markersize=7))

    # set the axis labels
    ax.set_xlabel(f"x_{feature_name}")
    ax.set_ylabel(f"y_{feature_name}")
    ax.set_zlabel(f"z_{feature_name}")
    # set the custom legend
    ax.legend(handles=legend_elements)

    plt.show()


def plot_model_history(history, num_epochs: int, use_validation_values=True) -> None:
    """
    Plot the history of the model (accuracy + loss) as two line graphs.

    :param history: the history object returned from model.fit()
    :param num_epochs: how many epochs to show on the graph
    :param use_validation_values: true if validation values should be plotted too else false
    """
    # turn off usage of validation values if there are no validations values in the history object
    if history.history["val_loss"] is None:
        use_validation_values = False

    # get the losses and accuracies for training
    losses = [history.history["loss"]]
    accuracies = [history.history["accuracy"]]
    if use_validation_values:
        # get the losses and accuracies for validation
        losses.append(history.history["val_loss"])
        accuracies.append(history.history["val_accuracy"])

    epochs = np.arange(0, num_epochs, 1)
    fig, axs = plt.subplots(nrows=2, sharex="all")

    # upper plot is the accuracy
    axs[0].plot(epochs, accuracies[0], label="Training")
    if use_validation_values: axs[0].plot(epochs, accuracies[1], label="Validation")
    axs[0].set(title="Accuracy", xlim=(0, epochs))
    axs[0].legend()

    # lower plot is the loss
    axs[1].plot(epochs, losses[0], label="Training")
    if use_validation_values: axs[1].plot(epochs, losses[1], label="Validation")
    axs[1].set(title="Accuracy", xlim=(0, epochs))
    axs[1].legend()

    plt.show()


def plot_confusion_matrix(cm: tf.Tensor, classes: List[str] = None) -> None:
    """
    Plot a confusion matrix as a seaborn.heatmap

    :param cm: the confusion matrix as a [n, n]-shaped Tensor as returned from tf.math.confusion_matrix
    :param classes: the classes for the confusion matrix (ordered list). Should be the same length as each dimension (n)
     of cm.
    """
    if classes is None:
        # set the classes to the values of all movement types if not yet set
        classes = constants.LUT_MOVEMENT_ID_TO_NAME.values()

    # create a dataframe from the confusion matrix with the classes as both labels
    df_cm = pd.DataFrame(cm.numpy(), classes, classes)
    sn.heatmap(df_cm, annot=True)
    plt.show()
