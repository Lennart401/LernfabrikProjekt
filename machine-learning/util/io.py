from typing import Dict, List, Any
from util import paths

import glob
import os
import pandas as pd
import tensorflow as tf


def load_all_movements(parent_folder: str) -> Dict[int, List[pd.DataFrame]]:
    """
    Loads all movement files from the specified parent folder and return them as Lists of DataFrames by movement id.

    Each movement file corresponds to one DataFrame. The DataFrame are aggregated in Lists, each Lists represents one
    movement-type folder (e.g. "1"). These lists are then put in a dictionary with the key being the movement type.

    Example of returned structure:

    .. code-block:: text

        {
            1: [ pd.DataFrame, pd.DataFrame, ... ],
            2: [ pd.DataFrame, ... ],
            ...
        }

    :param parent_folder: The folder in which all movement-type folders (e.g. "1", "2", etc.) are.
    :return: A dictionary of lists of DataFrames, each dictionary represents one movement type
    """
    movement_folders: List[Any] = glob.glob(f"{parent_folder}/*")
    movement_folders.sort()
    movement_dict = {}

    # load all csvs into the movement_dict
    for idx, folder in enumerate(movement_folders):
        movement_id = int(os.path.basename(os.path.normpath(folder)))

        # ignore movement id 0, this is not a movement but rather a placeholder for not-a-movement
        if movement_id == 0:
            continue

        # load the list of csvs
        sample_files: List[Any] = glob.glob(folder + "/*.csv")
        samples = []

        # read every single csv
        for file in sample_files:
            # todo: read the metadata from the filename (freq, dev, packetid)
            sample_df = pd.read_csv(file, index_col=None, header=0)
            samples.append(sample_df)

        movement_dict[movement_id] = samples

    return movement_dict


def save_features(features: pd.DataFrame, filename: str):
    """
    Save the features to a csv file.

    :param features: the features to be saved
    :param filename: the filename of the csv file
    """
    features.to_csv(f'{paths.PREPROCESSED_DATA_PATH}/{filename}', index=False)


def load_features(filename: str) -> pd.DataFrame:
    """
    Loads the features from a csv file.

    :param filename: the filename of the csv file
    :return: the features as a DataFrame
    """
    return pd.read_csv(f'{paths.PREPROCESSED_DATA_PATH}/{filename}', index_col=None, header=0)


def save_metadata(scale, offset, pca_components):
    """
    Save the metadata to a file.

    :param scale: the scale of the features
    :param offset: the offset of the features
    :param pca_components: the pca components of the features
    """
    with open(f'{paths.PREPROCESSED_DATA_PATH}/metadata.txt', 'w') as f:
        f.write('SCALING\n')
        f.write(f'scale:\n{scale}\n')
        f.write(f'offset:\n{offset}\n')

        f.write('PCA\n')
        f.write(f'components:\n{pca_components}')


def load_model(directory: str) -> tf.keras.Model:
    """
    Wrapper for loading a keras pb-Model.

    :param directory: the model directory
    :return: the tf.keras.Model loaded from the directory
    """
    return tf.saved_model.load(directory)


def save_model(model: tf.keras.Model, directory: str) -> None:
    """
    Wrapper for saving a keras model as a pb-file/directory.

    :param model: the tf.keras.Model to be saved
    :param directory: where to save the model
    """
    tf.saved_model.save(model, directory)


def convert_model_to_tflite(model: tf.keras.Model) -> Any:
    """
    Convert a keras pb-Model to a tflite model.

    :param model: the model that should be converted
    :return: the tflite model
    """
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    return converter.convert()


def save_model_as_tflite(model: tf.keras.Model, tflite_file: str) -> None:
    """
    Convert a tf.keras.Model to a tflite model and save it to a file.

    The tflite model will be saved as raw binary data to the file, the file should have an ending of `.tflite`. To
    convert the tflite file to a c array, open a bash shell (normal terminal on Unix-based systems or Git Bash on
    Windows) and execute the following command: `xxd -i model_file.tflite > c_file.h`.
    Alternatively, the script in 4_deploy_model.py can also be used to convert a saved model to tflite c-array.

    :param model: the model that should be converted
    :param tflite_file: the filename or file which the tflite-model should be saved to
    """
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    tflite_model = converter.convert()

    with open(tflite_file, "wb") as f:
        f.write(tflite_model)


def convert_save_model_to_tflite(model_directory: str, tflite_file: str) -> None:
    """
    Convert a saved keras pb-Model to a tflite model and save that model to a file.

    The tflite model will be saved as raw binary data to the file, the file should have an ending of `.tflite`. To
    convert the tflite file to a c array, open a bash shell (normal terminal on Unix-based systems or Git Bash on
    Windows) and execute the following command. `xxd -i model_file.tflite > c_file.h`.
    Alternatively, the script in 4_deploy_model.py can also be used to convert a saved model to tflite c-array.

    :param model_directory: model directory of the keras pb-Model file
    :param tflite_file: the filename or file which the tflite-model should be saved to
    """
    converter = tf.lite.TFLiteConverter.from_saved_model(model_directory)
    tflite_model = converter.convert()

    with open(tflite_file, "wb") as f:
        f.write(tflite_model)
    # to convert a saved tflite model from .tflite to a C array, use the command
    # xxd -i converted_model.tflite > model_data.c
    # in a git bash command shell
    # Alternatively, the script in 4_deploy_model.py can also be used to convert a saved model to tflite c-array.
