import number_types as nt
import pandas as pd
import numpy as np
import struct


def decode_payload(version, payload, sensors):
    if version == 1 or version == 2:
        return decode_payload_v1(payload, sensors)


def decode_payload_v1(payload, sensors):
    row_length = get_row_length_v1(sensors)
    num_rows = int(len(payload) / row_length)
    rows = pd.DataFrame(columns=sensors["Name"], index=range(num_rows))
    # only decode full rows (just discard the end if there is any more bytes)
    for i in range(num_rows):
        # this is the begin index for the row, it will be increased with every sensor by its respective length
        begin = i * row_length
        for index, sensor in sensors.iterrows():
            # get the sensor size and value as bytes
            sensor_size = nt.sizes[sensor["Type"]]
            value_bytes = payload[begin:begin + sensor_size]
            rows.loc[i, sensor["Name"]] = convert_raw_bytes(sensor["Type"], value_bytes)
            begin += sensor_size
    return rows


def get_row_length_v1(sensors):
    row_length = 0
    for sensor in sensors["Type"]:
        row_length += nt.sizes[sensor]
    return row_length


def convert_raw_bytes(stype, value):
    if stype.startswith("uint"):
        return int.from_bytes(value, byteorder='little', signed=False)

    elif stype.startswith("int"):
        return int.from_bytes(value, byteorder='little', signed=True)

    elif stype.startswith("float"):
        return struct.unpack("<f", value)[0]
