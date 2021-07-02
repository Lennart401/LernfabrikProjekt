import numpy as np
import pandas as pd
import glob
import matplotlib.pyplot as plt

path = r'T:\Python\LernfabrikWebserver\saved2'  # use your path
all_files = glob.glob(path + "/dev_2_2021*.csv")

li = []

for filename in all_files:
    df = pd.read_csv(filename, index_col=None, header=0)
    li.append(df)

frame = pd.concat(li, axis=0, ignore_index=True)
frame = frame.iloc[1:]
# frame["time"] = pd.to_datetime(frame["time"])
print(frame)

plt.plot(frame["time"], frame["temp"])
plt.show()
