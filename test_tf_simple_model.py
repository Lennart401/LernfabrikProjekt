from typing import Dict, List, Tuple

from sklearn.model_selection import train_test_split
from sklearn.preprocessing import OneHotEncoder
import numpy as np
import tensorflow as tf
import test_plot_movements as loader
import matplotlib.pyplot as plt


def convert_and_split_for_tf(features: Dict[int, List[Dict]], train_size=0.7) -> Tuple[np.array, np.array, np.array,
                                                                                       np.array]:
    x_all = []
    y_all = []
    for movement_id in features:
        for sample in features[movement_id]:
            # x_all.append([sample["y_mean"], sample["z_mean"], sample["z_std"]])
            x_all.append(list(sample.values()))
            y_all.append(movement_id)

    return train_test_split(np.array(x_all), np.array(y_all) - 1, train_size=train_size, random_state=42)


if __name__ == "__main__":
    movements_path = r"N:\GoogleDrive\Programmieren\LernfabrikML_data\movements"
    all_movements = loader.load_all_movements(movements_path)
    loader.preprocess_samples(all_movements)
    all_features = loader.generate_features(all_movements)

    x_train, x_test, y_train, y_test = convert_and_split_for_tf(all_features)
    # x_train_val, x_test, y_train_val, y_test = convert_and_split_for_tf(all_features)
    # x_train, x_val, y_train, y_val = train_test_split(x_train_val, y_train_val, train_size=0.7, random_state=1)

    EPOCHS = 30

    encoder = OneHotEncoder()
    encoder.fit(y_train.reshape(-1, 1))
    y_train_enc = encoder.transform(y_train.reshape(-1, 1)).toarray()
    # y_val_enc = encoder.transform(y_val.reshape(-1, 1)).toarray()
    y_test_enc = encoder.transform(y_test.reshape(-1, 1)).toarray()

    model = tf.keras.Sequential([
        tf.keras.layers.Input(shape=[len(x_train[0])]),
        tf.keras.layers.Dense(7, activation=tf.keras.activations.relu),
        tf.keras.layers.Dense(4, activation=tf.keras.activations.softmax)
    ])
    # predictions = model(x_train[:1]).numpy()
    # print(predictions)
    # print(tf.nn.softmax(predictions).numpy())
    # loss_fn = tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True)
    # print(loss_fn(y_train[:1], predictions).numpy())

    model.compile(optimizer=tf.keras.optimizers.Adam(0.07),
                  loss=tf.keras.losses.CategoricalCrossentropy(),
                  metrics=["accuracy"])

    checkpoint_callback = tf.keras.callbacks.ModelCheckpoint(
        filepath="./tmp/checkpoint",
        save_weights_only=False,
        save_best_only=True,
        mode="max",
        monitor="accuracy")

    history = model.fit(x_train, y_train_enc, epochs=EPOCHS, callbacks=[checkpoint_callback])
    # history = model.fit(x_train, y_train_enc, epochs=EPOCHS, validation_data=(x_val, y_val_enc))

    print(f"Training -> {model.evaluate(x_train, y_train_enc, verbose=2)}")
    print(f"Test -> {model.evaluate(x_test, y_test_enc, verbose=2)}")

    predictions_train = [np.argmax(x) for x in model.predict(x_train)]
    predictions_test = [np.argmax(x) for x in model.predict(x_test)]
    # predictions_train = model.predict(x_train)
    # predictions_test = model.predict(x_test)

    cm_train = tf.math.confusion_matrix(y_train, predictions_train)
    print(cm_train)
    cm_test = tf.math.confusion_matrix(y_test, predictions_test)
    print(cm_test)

    # Plot losses and accuracies
    losses = history.history['loss']
    # val_losses = history.history['val_loss']
    accuracies = history.history['accuracy']
    # val_accuracies = history.history['val_accuracy']
    epochs = np.arange(0, EPOCHS, 1)
    fig, ax = plt.subplots(2, sharex='all')
    ax[0].plot(epochs, accuracies, label='Training')
    # ax[0].plot(epochs, val_accuracies, label='Validation')
    ax[0].set(title='Accuracy', xlim=(0, EPOCHS))
    ax[0].legend()
    ax[1].plot(epochs, losses, label='Training')
    # ax[1].plot(epochs, val_losses, label='Validation')
    ax[1].set(title='Loss', xlim=(0, EPOCHS))
    ax[1].legend()
    plt.show()

    if input("save model?") == "y":
        tf.saved_model.save(model, "./tmp/final_model")
