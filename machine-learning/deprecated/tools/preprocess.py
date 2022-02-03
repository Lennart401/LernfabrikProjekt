import copy
import math
import numpy as np
import pandas as pd
from scipy import special


def read_and_reindex(file: str, reindex_time="10ms", use_reindex=True) -> pd.DataFrame:
    df = pd.read_csv(file, index_col="time", parse_dates=True, header=0)[1:]
    df.index = pd.to_datetime(df.index, unit="ms")
    start = df.index[0]
    end = df.index[-1]
    new_index = pd.date_range(start=start, end=end, freq=reindex_time)
    if use_reindex:
        df = df.index(new_index)
    return df


def process_outlier_detection(data: pd.DataFrame, cols: list) -> pd.DataFrame:
    """
    Finds and replaces outliers in the given data by distribution-based outlier detection.
    :param data: Data to do the outlier detection on.
    :param cols: Specific cols to use.
    :return: Data with replaced outliers by interpolation.
    """
    data_table = copy.deepcopy(data)
    for col in cols:
        # Compute the mean & standard deviation and set criterion
        mean = data_table[col].mean()
        std = data_table[col].std()
        criterion = 0.005
        # Consider the deviation for the data points
        deviation = abs(data_table[col] - mean) / std
        # Express the upper and lower bounds
        low = -deviation / math.sqrt(2)
        high = deviation / math.sqrt(2)
        prob = []
        mask = []
        # Pass all rows in the dataset.
        running_index = 0
        num_outliers = 0
        for i in data_table.index:
            # Determine the probability of observing the point
            prob.append(1.0 - 0.5 * (special.erf(high[i]) - special.erf(low[i])))
            # And mark as an outlier when the probability is below our criterion
            is_outlier = prob[running_index] < criterion
            if is_outlier:
                num_outliers += 1
            mask.append(is_outlier)
            running_index += 1

        print(f"for {col}: {num_outliers} outliers")
        data_table[col + '_outlier'] = mask
        data_table.loc[data_table[f'{col}_outlier'], col] = np.nan
        del data_table[col + '_outlier']
        data_table = impute_interpolate(data_table, col)
    return data_table


def impute_interpolate(dataset: pd.DataFrame, col: str) -> pd.DataFrame:
    """
    Fills missing values after outlier detection with interpolated values.
    :param dataset: Data with outliers as NANs.
    :param col: Col in which to impute values.
    :return: Returns data without NANs.
    """
    # Fill NANs with interpolated values
    dataset[col] = dataset[col].interpolate()
    dataset[col] = dataset[col].fillna(method='bfill')
    return dataset

