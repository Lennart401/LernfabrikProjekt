import pandas as pd


v1_sensors_types = {
    0: "uint8_t", 1: "uint16_t", 2: "uint32_t", 3: "uint64_t",
    4: "float_32", 5: "float_64"
}

v2_frequency_types = {
     0:    1,  1:    2,  2:    4,  3:    5,
     4:   10,  5:   20,  6:   40,  7:   50,
     8:  100,  9:  200, 10:  400, 11:  500,
    12: 1000, 13: 2000, 14: 4000, 15: 5000
}


def decode_header(content, length):
    # get header version
    header_version = content[0]
    if header_version == 1:
        return decode_header_v1(content, length)
    elif header_version == 2:
        return decode_header_v2(content, length)


def decode_header_v1(content, length):
    # get device id and number of sensors
    header_device_id = content[1]
    header_num_sensors = int.from_bytes(content[2:4], byteorder='big', signed=False)

    # decode sensors list
    offset = 4  # offset from the first content byte
    sensors_field_length = 5
    sensors_dict = []

    # loop all the sensors and decode their information
    for i in range(header_num_sensors):
        begin = i * sensors_field_length + offset
        sensor = {
            "Type": v1_sensors_types[content[begin]],
            "Name": content[begin + 1:begin + 5].decode('ascii')
        }
        sensors_dict.append(sensor)

    # return 1, header_device_id, offset + header_num_sensors * sensors_field_length, pd.DataFrame(sensors_dict)
    return {
        "version": 1,
        "header_size": offset + header_num_sensors * sensors_field_length,
        "sensors": pd.DataFrame(sensors_dict),
        "metadata": {
            "device_id": header_device_id
        }
    }


def decode_header_v2(content, length):
    # get device id
    device_id = content[1]

    # get metadata: packet id, frequency, movement-type
    packet_id = int.from_bytes(content[2:4], byteorder='little', signed=False)
    frequency = content[4] >> 4
    movement_type = content[4] & 0x7

    # deocde sensors list
    num_sensors = content[5]
    offset = 6
    sensors_field_length = 6
    sensors_dict = []

    # loop over all sensors and decode them
    for i in range(num_sensors):
        begin = i * sensors_field_length + offset
        sensor = {
            "Type": v1_sensors_types[content[begin]],
            "Name": content[begin+1:begin + 6].decode('ascii').strip()
        }
        sensors_dict.append(sensor)

    return {
        "version": 2,
        "header_size": offset + num_sensors * sensors_field_length,
        "sensors": pd.DataFrame(sensors_dict),
        "metadata": {
            "device_id": device_id,
            "packet_id": packet_id,
            "frequency": v2_frequency_types[frequency],
            "movement_type": movement_type
        }
    }
