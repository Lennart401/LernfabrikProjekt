import pandas as pd
import numpy as np

df = pd.read_csv('../../machine-learning/data/movements/5/dev_255_pkt_13_frq_100_20211122_130641.csv')

print('\n'.join([f'rows[{i}].timestamp = {10*i}; rows[{i}].acc_x = {df.loc[i, "accx"]:f}; rows[{i}].acc_y = {df.loc[i, "accy"]:f}; rows[{i}].acc_z = {df.loc[i, "accz"]:f}; rows[{i}].acc_abs = {np.sqrt(df.loc[i, "accx"]**2 + df.loc[i, "accy"]**2 + df.loc[i, "accz"]**2):f};' for i in range(0, 128)]))