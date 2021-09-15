from typing import Dict, List, Tuple
from sklearn.model_selection import train_test_split

import pandas as pd
import numpy as np
import tensorflow as tf
import test_plot_movements as loader

# mnist = tf.keras.datasets.mnist

# (x_train, y_train), (x_test, y_test) = mnist.load_data()
# print(x_train)
# print(y_train)


def convert_and_split_for_tf(features: Dict[int, List[Dict]]) -> Tuple[Tuple[List, List], Tuple[List, List]]:
    x_all = []
    y_all = []
    for movement_id in features:
        for sample in features[movement_id]:
            x_all.append(list(sample.values()))
            y_all.append(movement_id)

    x_train, x_test, y_train, y_test = train_test_split(np.array(x_all), np.array(y_all), train_size=0.7,
                                                        random_state=42)
    return (x_train, y_train - 1), (x_test, y_test - 1)


if __name__ == "__main__":
    movements_path = r"C:\Users\User\Documents\Lernfabrik\LernfabrikDataserver\movements"
    all_movements = loader.load_all_movements(movements_path)
    loader.preprocess_samples(all_movements)
    all_features = loader.generate_features(all_movements)
    (x_train, y_train), (x_test, y_test) = convert_and_split_for_tf(all_features)
    model = tf.keras.Sequential([
        tf.keras.layers.Input(shape=(6,)),
        tf.keras.layers.Dense(10, activation="relu"),
        tf.keras.layers.Dense(4)
    ])
    predictions = model(x_train[:1]).numpy()
    print(predictions)
    print(tf.nn.softmax(predictions).numpy())
    loss_fn = tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True)
    print(loss_fn(y_train[:1], predictions).numpy())

    model.compile(optimizer='adam',
                  loss=loss_fn,
                  metrics=['accuracy'])
    model.fit(x_train, y_train, epochs=10)
    model.evaluate(x_test, y_test, verbose=2)
