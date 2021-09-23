from typing import Dict, List, Tuple

import numpy as np
import pandas as pd
import copy

from numpy import ndarray
from sklearn.preprocessing import OneHotEncoder

from util import constants


def normalize_time(movements: Dict[int, List[pd.DataFrame]]) -> Dict[int, List[pd.DataFrame]]:
    """
    Normalize the time for each DataFrame.

    For each DataFrame, the time of the first row will be subtracted from all rows, resulting in the first row being
    time "0" and all other times being relative to the first one. The original dictionary will not be changed by the
    function, but a new dictionary with the normalized time will be returned.
    :param movements: Dictionary of lists of DataFrames. Will not be changed by the function.
    :return: A new dictionary with normalized time of each DataFrame of the movement-type.
    """
    movements_new = copy.deepcopy(movements)
    for movement_id in movements_new:
        for sample in movements_new[movement_id]:
            first_time = sample["time"][0]
            sample["time"] = sample["time"] - first_time
    return movements_new


def convert_dict_to_dataset(movements: Dict[int, List[Dict]]) -> Tuple[ndarray, ndarray]:
    """
    Builds a usuable dataset from a movement-id ordered dictionary.

    This function returns the x-Data first and the y-Data second. The x-Data is a ndarray in the shape of
    (len(observations), len(features)). The y-Data is a ndarray in the shape of (len(observations),). The y-Data will be
    translated from movement-type to index using :any:`~util.constants.LUT_MOVEMENT_ID_TO_INDEX`.

    :param movements: movement-id ordered dictionary of Lists of Dicts containing the features
    :return: usuable dataset as x- and y-Data
    """
    x_all = []
    y_all = []
    for movement_id in movements:
        for sample in movements[movement_id]:
            # x_all.append([sample["y_mean"], sample["z_mean"], sample["z_std"]])
            x_all.append(list(sample.values()))
            y_all.append(constants.LUT_MOVEMENT_ID_TO_INDEX[movement_id])

    return np.array(x_all), np.array(y_all)


def one_hot_encode_labels(labels: ndarray) -> ndarray:
    """
    Transforms labels into a one-hot-encoded state for training.

    The input is a ndarray containing the classes of each observations, like this: ``array([0, 1, 2])``. The
    output is an array of arrays, each row containing the labels in a one-hot-encoded state:
    ``[[1. 0. 0.] [0. 1. 0.] [0. 0. 1.]]``
    :param labels: the classes-array to be one-hot-encoded
    :return: an ndarray where every entry is a one-hot-encoded labels (also an ndarray)
    """
    reshaped_labels = labels.reshape(-1, 1)

    encoder = OneHotEncoder()
    encoder.fit(reshaped_labels)
    return encoder.transform(reshaped_labels).toarray()

