import copy
from typing import Dict, List, Tuple, Any, Union

import numpy as np
import pandas as pd
from imblearn.over_sampling import SMOTE
from numpy import ndarray
from sklearn.model_selection import train_test_split
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


def resample_dataset(movements: Dict[int, List[pd.DataFrame]], sample_length=100, resample_offset=10) \
        -> Dict[int, List[pd.DataFrame]]:
    """
    Resample the dataset to create more samples.

    This method will start at the beginning of the dataset (e.g. 150 samples) and create a new sample from the
    existing data at 0 samples offset, then at 1*resample_offset, then 2*resample_offset and so on. Each of those
    samples is sample_length many samples long. The samples are created until there is no more data from the original
    sample left.

    ``n_new_samples = int((len(original_sample) - sample_length) / resample_offset) + 1``

    :param movements: the movements loaded from files
    :param sample_length: how long each new sample should be (in number of rows)
    :param resample_offset: the offset between every new sample (in number of rows)
    :return: the resamples dataset in the same structure as movements
    """
    movements_new = {}
    for movement_id in movements:
        new_samples = []
        for original_sample in movements[movement_id]:
            n_new_samples = int((len(original_sample) - sample_length) / resample_offset) + 1
            for i in range(0, n_new_samples):
                start = i * resample_offset
                end = start + sample_length
                new_samples.append(original_sample.iloc[start:end].reset_index(drop=True))
        movements_new[movement_id] = new_samples
    return movements_new


def synthesize_transition_samples(movements: Dict[int, List[pd.DataFrame]],
                                  transitions: ndarray,
                                  splits: list,
                                  drop: float = 0.9):
    synthesized = {x: [] for x in movements.keys()}

    for primary_type, secondary_type in transitions:
        # list of DataFrames
        primary_list = movements[primary_type]
        secondary_list = movements[secondary_type]

        for primary_df in primary_list:
            for secondary_df in secondary_list:
                for split in splits:
                    if np.random.random() > drop:
                        # split the primary and secondary DataFrames into their respective sizes
                        primary_split = primary_df.iloc[:round(split * len(primary_df))]
                        secondary_split = secondary_df.iloc[round(split * len(secondary_df)):]

                        # concatenate the two DataFrames
                        synthesized[primary_type].append(pd.concat([primary_split, secondary_split]))

    return synthesized


def convert_dict_to_dataset(movements: Dict[int, List[Dict]]) -> Tuple[ndarray, ndarray, list]:
    """
    Build an X, y dataset from a movement-id ordered dictionary.

    :param movements: movement-id ordered dictionary of Lists of Dicts containing the features
    :return: dataset as X- and y-Data and the column names as a list
    """
    x_all = []
    y_all = []
    names = None
    for movement_id in movements:
        for sample in movements[movement_id]:
            if names is None:
                names = list(sample.keys())
            # x_all.append([sample["y_mean"], sample["z_mean"], sample["z_std"]])
            x_all.append(list(sample.values()))
            y_all.append(constants.LUT_MOVEMENT_ID_TO_INDEX[movement_id])

    return np.array(x_all), np.array(y_all), names


def one_hot_encode_labels(labels: ndarray, categories: Union[str, List] = 'auto') -> ndarray:
    """
    Transform labels into a one-hot-encoded state for training.

    The input is a ndarray containing the classes of each observations, like this: ``array([0, 1, 2])``. The
    output is an array of arrays, each row containing the labels in a one-hot-encoded state:
    ``[[1. 0. 0.] [0. 1. 0.] [0. 0. 1.]]``

    :param labels: the classes-array to be one-hot-encoded
    :param categories: the categories to use, or 'auto', to determine the categories automatically
    :return: an ndarray where every entry is a one-hot-encoded labels (also an ndarray)
    """
    reshaped_labels = labels.reshape(-1, 1)

    encoder = OneHotEncoder(categories=categories)
    encoder.fit(reshaped_labels)
    return encoder.transform(reshaped_labels).toarray()


def split_train_test(x: ndarray, y: ndarray, train_size: float = 0.7, random_state=None, stratify: ndarray = None) \
        -> Any:
    """
    Wrapper for sklearn.model_selection.train_test_split. Just calls that method.

    :param x: the samples array
    :param y: the label array, can be, but does not necessarily need to be one-hot-encoded
    :param train_size: percentage of observations to use for training
    :param random_state: an integer or RandomState instance to produce reproducable results
    :param stratify: what labels to use for stratified train test split
    :return: the result from sklearn.model_selection.train_test_split
    """
    return train_test_split(x, y, train_size=train_size, random_state=random_state, stratify=stratify)


def oversample_dataset(x: ndarray, y: ndarray) -> Tuple[ndarray, ndarray]:
    """
    Wrapper for SMOTE oversample.

    This method oversamples the given data so that all classes have as many samples as the class with the most samples.

    :param x: the samples array
    :param y: the label array. only tested to work when NOT one-hot-encoded
    :return: the x and y array with oversampled classes
    """
    classes, counts = np.unique(y, return_counts=True)
    oversample = SMOTE(k_neighbors=counts.min() - 1)
    x_oversampled, y_oversampled = oversample.fit_resample(x, y)
    return x_oversampled, y_oversampled
