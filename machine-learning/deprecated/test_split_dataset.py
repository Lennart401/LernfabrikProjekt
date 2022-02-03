import matplotlib.pyplot as plt
from deprecated.tools import preprocess
import scipy.signal as signal

df = preprocess.read_and_reindex("../data/materialkiste-test-1.csv", use_reindex=False)

acc_x = "accx"
acc_y = "accy"
acc_z = "accz"

b_filt, a_filt = signal.butter(4, 5, btype="lowpass", fs=200)
for col in [acc_x, acc_y, acc_z]:
    df[col] = signal.filtfilt(b_filt, a_filt, df[col])

means = [[], [], []]

index = 0
size = 200
method = "mean"

while index + size < len(df):
    means[0].append(df[acc_x][index:index + size].aggregate(method))
    means[1].append(df[acc_y][index:index + size].aggregate(method))
    means[2].append(df[acc_z][index:index + size].aggregate(method))
    index += size

fig = plt.figure()
ax = fig.add_subplot(projection="3d")

ax.scatter(means[0], means[1], means[2])
plt.show()
