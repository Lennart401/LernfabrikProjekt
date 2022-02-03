import pandas as pd
import matplotlib.pyplot as plt
import scipy.signal as signal
from scipy import integrate
from deprecated.tools import preprocess

# read csv and convert time/index to datetime
# df = pd.read_csv("data/accel_test/dev_2_2021-07-15_15-21-54.csv", index_col="time", parse_dates=True, header=0)[1:]
# df.index = pd.to_datetime(df.index, unit="ms")

# reindex so that every 10ms there is a datapoint
# start = df.index[0]
# end = df.index[-1]
# new_index = pd.date_range(start=start, end=end, freq="10ms")
# df = df.reindex(new_index)
df = preprocess.read_and_reindex("../data/accel_test/dev_2_2021-07-15_15-21-54.csv", use_reindex=False)
print(df)

# create rolling averages
roll = 25
df["accx_rolling"] = df["accx"].rolling(roll).mean()
df["accy_rolling"] = df["accy"].rolling(roll).mean()
df["accz_rolling"] = df["accz"].rolling(roll).mean()

# define column names
acc_x = "accx"
acc_y = "accy"
acc_z = "accz"
# acc_x = "accx_rolling"
# acc_y = "accy_rolling"
# acc_z = "accz_rolling"

frequency = 200
dx = 1 / frequency


# function to print the mean of the first seconds in the dataset (no movement)
def print_offset_first_seconds():
    first_seconds = df[pd.to_datetime("1970-01-01 00:23:50.041"):pd.to_datetime("1970-01-01 00:24:00")]

    x_mean = first_seconds[acc_x].mean()
    y_mean = first_seconds[acc_y].mean()
    z_mean = first_seconds[acc_z].mean()
    print(f"x_mean: {x_mean}, y_mean: {y_mean}, z_mean: {z_mean}")
    return x_mean, y_mean, z_mean


# correct the z column
xm, ym, zm = print_offset_first_seconds()
# df["racx"] = df["racx"] - xm
# df["racy"] = df["racy"] - ym
# df["racz"] = df["racz"] - zm
print_offset_first_seconds()
print("accz_mean:", df[acc_z].mean())

# fill missing values (only now when the z column got corrected)
df = df.fillna(0.0)

# filter out sensory noise
use_filter = False
use_filter2 = True

if use_filter:
    sos = signal.butter(3, 20, btype="lowpass", output="sos", fs=frequency)
    for col in [acc_x, acc_y, acc_z]:
        df[col] = signal.sosfilt(sos, df[col])

if use_filter2:
    b_filt, a_filt = signal.butter(4, 5, btype="lowpass", fs=frequency)
    for col in [acc_x, acc_y, acc_z]:
        df[col] = signal.filtfilt(b_filt, a_filt, df[col])

# display plots?
plot = False


# function to plot the dataset
def plot_df():
    if plot:
        plt.plot(df.index, df[acc_x])
        plt.plot(df.index, df[acc_y])
        plt.plot(df.index, df[acc_z])
        plt.show()


# plot
plot_df()

# remove outliers
#df = preprocess.process_outlier_detection(df, [acc_x, acc_y, acc_z])
#print("after outliers", df)
plot_df()

# integrate once for velocity
df["velx"] = integrate.cumulative_trapezoid(df[acc_x], dx=dx, initial=0)
df["vely"] = integrate.cumulative_trapezoid(df[acc_y], dx=dx, initial=0)
df["velz"] = integrate.cumulative_trapezoid(df[acc_z], dx=dx, initial=0)
# df["velx"] = integrate.cumulative_trapezoid(df[acc_x], df.index, initial=0)
# df["vely"] = integrate.cumulative_trapezoid(df[acc_y], df.index, initial=0)
# df["velz"] = integrate.cumulative_trapezoid(df[acc_z], df.index, initial=0)

# and once more for distance
df["distx"] = integrate.cumulative_trapezoid(df["velx"], dx=dx, initial=0)
df["disty"] = integrate.cumulative_trapezoid(df["vely"], dx=dx, initial=0)
df["distz"] = integrate.cumulative_trapezoid(df["velz"], dx=dx, initial=0)

# print values at the end
print()
print(f"x at the end: {df[acc_x][-1]} - {df['velx'][-1]} - {df['distx'][-1]}")
print(f"y at the end: {df[acc_y][-1]} - {df['vely'][-1]} - {df['disty'][-1]}")
print(f"z at the end: {df[acc_z][-1]} - {df['velz'][-1]} - {df['distz'][-1]}")

fig, axs = plt.subplots(nrows=3, tight_layout=True, sharex="all")
axs[0].plot(df.index, df[acc_x], label="acceleration")
axs[0].plot(df.index, df["velx"], label="velocity")
axs[0].plot(df.index, df["distx"], label="distance")
axs[0].grid()
axs[0].legend()

axs[1].plot(df.index, df[acc_y], label="acceleration")
axs[1].plot(df.index, df["vely"], label="velocity")
axs[1].plot(df.index, df["disty"], label="distance")
axs[1].grid()
axs[1].legend()

axs[2].plot(df.index, df[acc_z], label="acceleration")
axs[2].plot(df.index, df["velz"], label="velocity")
axs[2].plot(df.index, df["distz"], label="distance")
axs[2].grid()
axs[2].legend()
plt.show()

