# LernfabrikML

This repository contains all machine learning code for the LernfabrikBoxen project. In general, the code takes in the samples recorded from the box, then preprocesses them, trains a model and exports the model as a tflite binary file. The only step left to do is converting the model to a c array (so it can be deployed on the Arduino).

In general, the workflow follows the files 1_data_exploration.py to 4_deploy_model.py. The last file will yield a model.h file, which can be copy-and-pasted into the arduino code. When the code is then recompiled and uploaded to a box, the box is equipped with a new model.
