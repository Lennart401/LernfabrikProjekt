# Data Exploration for Lernfabrik Data
# This file explains and shows the workflow for the first step in ML: data exploration.
# 
# Before we start, lets import the required modules for our data exploration:
from util import paths, io, plotter, constants
from preprocessing import preprocessing, featureengineering

# ----------------------------------------------------------------------------------------------------------------------
# 1. Preprocessing
# As a first step, we want to load all our samples and normalize the time, so we can plot them:
all_movements = io.load_all_movements(paths.MOVEMENTS_PATH)
all_movements = preprocessing.normalize_time(all_movements)

# The samples are loaded into one huge dictionary. The first level of keys represents the movement types as integers,
# with the corresponding values being a list of pandas DataFrames. Each DataFrame is one samples, loaded from its csv
# file. Let's have a look at this:
print(all_movements[1][0])

# ----------------------------------------------------------------------------------------------------------------------
# 2. Plotting
# The next step is to create various plots to look at the data. To begin, let's plot the raw acceleration of our samples
# as a single large grid:
plotter.plot_movements_raw(all_movements, sensors=['accx', 'accy', 'accz'])

# Next, let's compute the features mean and standard deviation on our samples. This is done by taking each axis of the
# raw acceleration for each sample and computing the mean and the standard deviation of those values. The features will
# be saved into a new dictionary with a similar structure like the raw samples, but instead of the DataFrames, we will
# use dictionaries. The key describes the name of the feature (e.g. 'x_mean') and the value is a single float, the
# computed feature.
all_features = featureengineering.generate_basic_features(all_movements)
print(all_features[1][0])

# Having computed the features, let's start by plotting the mean of x, y, and z in a 3d-plot. To make the plot easier to
# read, we can colorcode each movement type by using the following lookup table:
print(constants.LUT_MOVEMENT_ID_TO_COLOR)
plotter.plot_3_axis_feature(all_features, feature_name='mean', color_lookup_table=constants.LUT_MOVEMENT_ID_TO_COLOR)
plotter.plot_3_axis_feature(all_features, feature_name='std', color_lookup_table=constants.LUT_MOVEMENT_ID_TO_COLOR)

# As for now, this is all we can do in terms of data exploration. In notebook number 3, we will look at training a model
# on the samples we have and exporting the model as a tflite file.
