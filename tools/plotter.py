import pandas as pd
import matplotlib.pyplot as plt
from scipy import integrate


if __name__ == "__main__":
    df = pd.read_csv("../data/materialkiste-test-1.csv.bck")

    df["velx"] = integrate.cumtrapz(df["accx"], df["time"], initial=0)

    plt.plot(df["time"], df["velx"])

    # plt.plot(df["time"], df["accx"])
    # plt.plot(df["time"], df["accy"])
    # plt.plot(df["time"], df["accz"])
    # plt.legend()
    plt.show()
