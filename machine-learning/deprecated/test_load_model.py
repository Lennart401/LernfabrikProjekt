import itertools
from typing import Dict, List, Tuple

import numpy as np
import pandas as pd
import tensorflow as tf
from numpy import ndarray

import test_plot_movements as loader
import matplotlib.pyplot as plt
import seaborn as sn


def convert_for_tf(features: Dict[int, List[Dict]]) -> Tuple[List, ndarray]:
    x_all = []
    y_all = []
    for movement_id in features:
        for sample in features[movement_id]:
            # x_all.append([sample["y_mean"], sample["z_mean"], sample["z_std"]])
            x_all.append(list(sample.values()))
            y_all.append(movement_id)

    return x_all, np.array(y_all) - 1


if __name__ == "__main__":
    movements_path = r"N:\GoogleDrive\Programmieren\LernfabrikML_data\movements"
    all_movements = loader.load_all_movements(movements_path)
    loader.preprocess_samples(all_movements)
    all_features = loader.generate_features(all_movements)

    x_all, y_all = convert_for_tf(all_features)
    model = tf.saved_model.load("./models/protocol_v2_1")

    predictions = [np.argmax(x) for x in model(x_all)]
    cm = tf.math.confusion_matrix(y_all, predictions)
    print(cm)

    classes = ["On ramp", "Ready for pickup", "On moving wagon", "No movement"]
    df_cm = pd.DataFrame(cm.numpy(), classes, classes)
    print(df_cm)
    # sn.heatmap(df_cm, annot=True, annot_kws={"size": 12})
    sn.heatmap(df_cm, annot=True)
    plt.show()
