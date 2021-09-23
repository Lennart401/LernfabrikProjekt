from typing import Dict, List
import pandas as pd


def generate_basic_features(movements: Dict[int, List[pd.DataFrame]]) -> Dict[int, List[Dict]]:
    """
    Generate basic features from the given dict of lists of DataFrames.

    This functions builds a new dictionary similary to the parameter, however each DataFrame is replaced by a dictionary
    with the key as the feature name and the value as the feature value.

    The generated features are:

    - mean (on x, y, and z) (x_mean, y_mean, z_mean)
    - standard deviation (on x, y, and z) (x_std, y_std_, z_std)

    :param movements: the data on which to generate the features
    :return: the features generated for each sample
    """
    movement_features = dict()
    for movement_id in movements:
        samples_list = []
        for sample in movements[movement_id]:
            sample_features = {
                "x_mean": sample["accx"].mean(),
                "y_mean": sample["accy"].mean(),
                "z_mean": sample["accz"].mean(),
                "x_std": sample["accx"].std(),
                "y_std": sample["accy"].std(),
                "z_std": sample["accz"].std(),
            }
            samples_list.append(sample_features)
        movement_features[movement_id] = samples_list
    return movement_features
