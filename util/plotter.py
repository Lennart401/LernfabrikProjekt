from typing import List, Any, Dict

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import glob
import os

from matplotlib.lines import Line2D
from util import constants


def plot_movements_raw(movements: Dict[int, List[pd.DataFrame]], sensors: List[str] = None, index_col="time",
                       sharex="all", sharey="all") -> None:
    """
    Plots the raw movement data in a huge figure.

    Very important: normalize the time of the sample first for readability. There will be one row per movement type and
    one column for each sensor respectively.
    :param movements: dict of samples for each movement. each sample will be plotted.
    :param sensors: the sensors to plot, each of them will be one column. by default "accx", "accy" and "accz"
    :param index_col: what column of every sample to use as x-axis. "time" by default
    :param sharex: the x-axes to share. see pyplot documentation.
    :param sharey: the y-axes to share. see pyplot documentation.
    :return: None
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

