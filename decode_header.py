import pandas as pd


v1_sensors_types = {
    0: "uint8_t", 1: "uint16_t", 2: "uint32_t", 3: "uint64_t",
    4: "float_32", 5: "float_64"
}


def decode_header(content, length):
    # get header version
    header_version = content[0]
    if header_version == 1:
        return decode_header_v1(content, length)


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

    return 1, header_device_id, offset + header_num_sensors * sensors_field_length, pd.DataFrame(sensors_dict)
