from typing import List, Any, Dict

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import glob
import os

from matplotlib.lines import Line2D

mid_name_lut = {
    1: "On ramp",
    2: "Ready for pickup",
    3: "On moving wagon",
    4: "No movement",
}

mid_color_lut = {
    1: "red",
    2: "green",
    3: "blue",
    4: "black",
}


def load_all_movements(parent_folder: str) -> Dict[int, List[pd.DataFrame]]:
    movement_folders: List[Any] = glob.glob(f"{parent_folder}\\[1-4]")
    # movement_folders: List[Any] = glob.glob(f"{parent_folder}\\1")
    movement_dict = {}

    # load all csvs into the movement_dict
    for idx, folder in enumerate(movement_folders):
        movement_id = int(os.path.basename(os.path.normpath(folder)))

        # load the list of csvs
        sample_files: List[Any] = glob.glob(folder + "\\*.csv")
        samples = []

        # read every single csv
        for file in sample_files:
            # todo: read the metadata from the filename (freq, dev, packetid)
            sample_df = pd.read_csv(file, index_col=None, header=0)
            samples.append(sample_df)

        movement_dict[movement_id] = samples

    return movement_dict


def preprocess_samples(movements: Dict[int, List[pd.DataFrame]]) -> None:
    for movement_id in movements:
        for sample in movements[movement_id]:
            first_time = sample["time"][0]
            sample["time"] = sample["time"] - first_time


def plot_movements_xyz_raw(movements: Dict[int, List[pd.DataFrame]]) -> None:
    fig, axs = plt.subplots(len(movements), 3, tight_layout=True, sharex='all', sharey='all')
    for movement_id in movements:
        for sample in movements[movement_id]:
            idx = movement_id - 1
            axs[idx, 0].plot(sample["time"], sample["accx"])
            axs[idx, 1].plot(sample["time"], sample["accy"])
            axs[idx, 2].plot(sample["time"], sample["accz"])

            axs[idx, 0].set_ylabel(mid_name_lut[movement_id])
        axs[0, 0].set_title("acc-x")
        axs[0, 1].set_title("acc-y")
        axs[0, 2].set_title("acc-z")
    plt.show()


def generate_features(movements: Dict[int, List[pd.DataFrame]]) -> Dict[int, List[Dict]]:
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


def plot_feature_3d(features: Dict[int, List[Dict[str, float]]], feature_name: str = "mean") -> None:
    fig = plt.figure(tight_layout=True)
    ax = fig.add_subplot(projection="3d")
    legend_elements = []

    for movement_id in features:
        for samples in features[movement_id]:
            ax.scatter(samples[f"x_{feature_name}"],
                       samples[f"y_{feature_name}"],
                       samples[f"z_{feature_name}"],
                       color=mid_color_lut[movement_id])
        legend_elements.append(Line2D([0], [0], marker="o", color="w", label=mid_name_lut[movement_id],
                                      markerfacecolor=mid_color_lut[movement_id], markersize=7))

    ax.set_xlabel(f"x_{feature_name}")
    ax.set_ylabel(f"y_{feature_name}")
    ax.set_zlabel(f"z_{feature_name}")
    ax.legend(handles=legend_elements)

    plt.show()


if __name__ == "__main__":
    movements_path = r"N:\GoogleDrive\Programmieren\LernfabrikML_data\movements"
    all_movements = load_all_movements(movements_path)
    preprocess_samples(all_movements)
    plot_movements_xyz_raw(all_movements)
    # print(generate_features(all_movements))
    all_features = generate_features(all_movements)
    plot_feature_3d(all_features, "mean")
    plot_feature_3d(all_features, "std")
