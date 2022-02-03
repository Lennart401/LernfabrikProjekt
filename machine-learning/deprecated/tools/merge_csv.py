import sys
import glob
import pandas as pd


def merge_csv(directory: str) -> None:
    all_files = glob.glob(f"./data/{directory}/*.csv")
    li = []

    for filename in all_files:
        df = pd.read_csv(filename, index_col=None, header=0)
        li.append(df)

    frame = pd.concat(li, axis=0, ignore_index=True)
    frame = frame.iloc[1:]
    frame.to_csv(f"./data/{directory}.csv", index=False)


if __name__ == "__main__":
    if len(sys.argv) <= 1:
        folder_name = input("Please specify the folder holding csv files under /data: ")
    else:
        folder_name = sys.argv[1]

    merge_csv(folder_name)

