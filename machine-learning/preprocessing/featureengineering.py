from typing import Dict, List

from scipy.fft import rfft, rfftfreq
from scipy.stats import entropy
from scipy.signal import welch
import numpy as np
import pandas as pd


def generate_basic_features(movements: Dict[int, List[pd.DataFrame]]) -> Dict[int, List[Dict[str, float]]]:
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


def generate_selected_features(movements: Dict[int, List[pd.DataFrame]],
                               selected_featrues: List[str] = None,
                               signal_length: int = 100,
                               samples_per_seconds: int = 100) -> Dict[int, List[Dict[str, float]]]:
    if selected_featrues is None:
        selected_featrues = ['acc_x_mean',
                             'acc_y_mean',
                             'acc_z_mean',
                             'acc_abs_mean',
                             'acc_x_std',
                             'acc_y_std',
                             'acc_z_std',
                             'acc_abs_std',
                             'acc_x_min',
                             'acc_y_min',
                             'acc_z_min',
                             'acc_abs_min',
                             'acc_x_max',
                             'acc_y_max',
                             'acc_z_max',
                             'acc_abs_max',
                             'acc_x_entropy',
                             'acc_y_entropy',
                             'acc_z_entropy',
                             'acc_abs_entropy',
                             'acc_x_pse',
                             'acc_y_pse',
                             'acc_z_pse',
                             'acc_abs_pse',
                             'acc_x_peak_freq',
                             'acc_y_peak_freq',
                             'acc_z_peak_freq',
                             'acc_abs_peak_freq']

    movement_features = dict()
    for movement_id in movements:
        samples_list = []
        for sample in movements[movement_id]:
            # calculate the absolute acceleration
            acc_abs = np.sqrt(sample['accx'] ** 2 + sample['accy'] ** 2 + sample['accz'] ** 2)
            acc_abs_no_static = acc_abs - acc_abs.mean()

            acc_x_no_static = sample['accx'] - sample['accx'].mean()
            acc_y_no_static = sample['accy'] - sample['accy'].mean()
            acc_z_no_static = sample['accz'] - sample['accz'].mean()

            # gyro_x_no_static = sample['gyrox'] - sample['gyrox'].mean()
            # gyro_y_no_static = sample['gyroy'] - sample['gyroy'].mean()
            # gyro_z_no_static = sample['gyroz'] - sample['gyroz'].mean()

            # calculate ffts of different signals
            acc_x_fft = rfft(acc_x_no_static.to_numpy())
            acc_y_fft = rfft(acc_y_no_static.to_numpy())
            acc_z_fft = rfft(acc_z_no_static.to_numpy())
            acc_abs_fft = rfft(acc_abs_no_static.to_numpy())
            # gyro_x_fft = rfft(gyro_x_no_static.to_numpy())
            # gyro_y_fft = rfft(gyro_y_no_static.to_numpy())
            # gyro_z_fft = rfft(gyro_z_no_static.to_numpy())

            acc_x_entropy = entropy(np.abs(acc_x_fft))
            acc_y_entropy = entropy(np.abs(acc_y_fft))
            acc_z_entropy = entropy(np.abs(acc_z_fft))
            acc_abs_entropy = entropy(np.abs(acc_abs_fft))
            # gyro_x_entropy = entropy(np.abs(gyro_x_fft))
            # gyro_y_entropy = entropy(np.abs(gyro_y_fft))
            # gyro_z_entropy = entropy(np.abs(gyro_z_fft))

            # calculate power spectral density of different signals
            acc_x_psd = (np.abs(acc_x_fft) ** 2) / len(np.abs(acc_x_fft))
            acc_y_psd = (np.abs(acc_y_fft) ** 2) / len(np.abs(acc_y_fft))
            acc_z_psd = (np.abs(acc_z_fft) ** 2) / len(np.abs(acc_z_fft))
            acc_abs_psd = (np.abs(acc_abs_fft) ** 2) / len(np.abs(acc_abs_fft))

            # calculate power spectrum entropy of different signals
            acc_x_pse = entropy(acc_x_psd)
            acc_y_pse = entropy(acc_y_psd)
            acc_z_pse = entropy(acc_z_psd)
            acc_abs_pse = entropy(acc_abs_psd)
            # gyro_x_pse = entropy(welch(gyro_x_no_static.to_numpy(), fs=sampling_freq)[1])
            # gyro_y_pse = entropy(welch(gyro_y_no_static.to_numpy(), fs=sampling_freq)[1])
            # gyro_z_pse = entropy(welch(gyro_z_no_static.to_numpy(), fs=sampling_freq)[1])

            # peak frequency
            sampling_freq = 1 / samples_per_seconds
            fft_freqs = rfftfreq(signal_length, sampling_freq)

            acc_x_peak_freq = fft_freqs[np.argmax(np.abs(acc_x_fft))]
            acc_y_peak_freq = fft_freqs[np.argmax(np.abs(acc_y_fft))]
            acc_z_peak_freq = fft_freqs[np.argmax(np.abs(acc_z_fft))]
            acc_abs_peak_freq = fft_freqs[np.argmax(np.abs(acc_abs_fft))]
            # gyro_x_peak_freq = fft_freqs[np.argmax(np.abs(gyro_x_fft))]
            # gyro_y_peak_freq = fft_freqs[np.argmax(np.abs(gyro_y_fft))]
            # gyro_z_peak_freq = fft_freqs[np.argmax(np.abs(gyro_z_fft))]

            sample_features = {}
            if 'acc_x_mean' in selected_featrues:           sample_features['acc_x_mean'] = sample['accx'].mean()
            if 'acc_y_mean' in selected_featrues:           sample_features['acc_y_mean'] = sample['accy'].mean()
            if 'acc_z_mean' in selected_featrues:           sample_features['acc_z_mean'] = sample['accz'].mean()
            if 'acc_abs_mean' in selected_featrues:         sample_features['acc_abs_mean'] = acc_abs.mean()
            if 'acc_x_min' in selected_featrues:            sample_features['acc_x_min'] = sample['accx'].min()
            if 'acc_y_min' in selected_featrues:            sample_features['acc_y_min'] = sample['accy'].min()
            if 'acc_z_min' in selected_featrues:            sample_features['acc_z_min'] = sample['accz'].min()
            if 'acc_abs_min' in selected_featrues:          sample_features['acc_abs_min'] = acc_abs.min()
            if 'acc_x_max' in selected_featrues:            sample_features['acc_x_max'] = sample['accx'].max()
            if 'acc_y_max' in selected_featrues:            sample_features['acc_y_max'] = sample['accy'].max()
            if 'acc_z_max' in selected_featrues:            sample_features['acc_z_max'] = sample['accz'].max()
            if 'acc_abs_max' in selected_featrues:          sample_features['acc_abs_max'] = acc_abs.max()
            if 'acc_x_std' in selected_featrues:            sample_features['acc_x_std'] = sample['accx'].std()
            if 'acc_y_std' in selected_featrues:            sample_features['acc_y_std'] = sample['accy'].std()
            if 'acc_z_std' in selected_featrues:            sample_features['acc_z_std'] = sample['accz'].std()
            if 'acc_abs_std' in selected_featrues:          sample_features['acc_abs_std'] = acc_abs.std()
            if 'acc_x_entropy' in selected_featrues:        sample_features['acc_x_entropy'] = acc_x_entropy
            if 'acc_y_entropy' in selected_featrues:        sample_features['acc_y_entropy'] = acc_y_entropy
            if 'acc_z_entropy' in selected_featrues:        sample_features['acc_z_entropy'] = acc_z_entropy
            if 'acc_abs_entropy' in selected_featrues:      sample_features['acc_abs_entropy'] = acc_abs_entropy
            if 'acc_x_peak_freq' in selected_featrues:      sample_features['acc_x_peak_freq'] = acc_x_peak_freq
            if 'acc_y_peak_freq' in selected_featrues:      sample_features['acc_y_peak_freq'] = acc_y_peak_freq
            if 'acc_z_peak_freq' in selected_featrues:      sample_features['acc_z_peak_freq'] = acc_z_peak_freq
            if 'acc_abs_peak_freq' in selected_featrues:    sample_features['acc_abs_peak_freq'] = acc_abs_peak_freq
            if 'acc_x_pse' in selected_featrues:            sample_features['acc_x_pse'] = acc_x_pse
            if 'acc_y_pse' in selected_featrues:            sample_features['acc_y_pse'] = acc_y_pse
            if 'acc_z_pse' in selected_featrues:            sample_features['acc_z_pse'] = acc_z_pse
            if 'acc_abs_pse' in selected_featrues:          sample_features['acc_abs_pse'] = acc_abs_pse

            samples_list.append(sample_features)
        movement_features[movement_id] = samples_list
    return movement_features
