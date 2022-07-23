# Deploy a trained model to tflite/c-file.
# This file contains the workflow to export, convert and deploy a trained model to tflite and then to a c-file.
#
# First, lets start with some imports
from util import io

# Settings for file name
PROTOCOL_VERSION = 2
REVISION = 2
MODEL_NO = 8
FILE_NAME = f'protocol_v{PROTOCOL_VERSION}_rev_{REVISION}_model_{MODEL_NO}'
NO_INPUT_TENSORS = 28
NO_OUTPUT_TENSORS = 6

# Convert the trained model to a tflite model:
model = io.load_model(f'./models/{FILE_NAME}')
tflite_model = io.convert_model_to_tflite(model)

# Build a c header file that contains the model as an array, its length and the number of input and output tensors:
header_content = """unsigned char tflite_model[] = {
"""

# Every line contains 12 bytes
for i in range(0, len(tflite_model), 12):
    # start with spaces
    header_content += f'  '

    # the number of bytes for this line is either 12 or the remaining bytes
    items = min(12, len(tflite_model) - i)

    # write the bytes to the line
    for j in range(0, items):
        header_content += f'0x{tflite_model[i+j]:02x}, '

    # if this is the last line (i.e. less than 12 bytes), remove the last comma
    if items < 12:
        header_content = header_content[:-2]

    # add the linebreak
    header_content += f'\n'

# add the length of the model and the other variables
header_content += '};\n'
header_content += f'unsigned int tflite_model_len = {len(tflite_model)};\n'
header_content += f"""
#define INPUT_TENSOR_SIZE {NO_INPUT_TENSORS}
#define OUTPUT_TENSOR_SIZE {NO_OUTPUT_TENSORS}
"""

# Add metadata stuff
with open('./data/preprocessed/metadata.txt', 'r') as f:
    metadata = f.read()
    scale_begin = metadata.find('scale:') + len('scale:') + 2
    scale_end = metadata.find(']', scale_begin)
    scales_list = metadata[scale_begin:scale_end].replace('\n', '').split()
    scales = [float(x) for x in scales_list]

    offset_begin = metadata.find('offset:') + len('offset:') + 2
    offset_end = metadata.find(']', offset_begin)
    offsets_list = metadata[offset_begin:offset_end].replace('\n', '').split()
    offsets = [float(x) for x in offsets_list]

    header_content += '\nconst float input_scale[] = {\n'

    for i in range(0, len(scales), 4):
        header_content += f'  '
        items = min(4, len(scales) - i)
        for j in range(0, items):
            header_content += f'{scales[i + j]:f}, '
        if items < 4:
            header_content = header_content[:-2]
        header_content += f'\n'

    header_content += '};\n\nconst float input_offset[] = {\n'

    for i in range(0, len(offsets), 4):
        header_content += f'  '
        items = min(4, len(offsets) - i)
        for j in range(0, items):
            header_content += f'{offsets[i + j]:f}, '
        if items < 4:
            header_content = header_content[:-2]
        header_content += f'\n'

    header_content += '};\n'


# Write the file to the arduino subproject directory as model.h
with open('../arduino/CoreM7/model.h', 'w') as f:
    f.write(header_content)

# Lastly, the only step left to do is to re-compile and re-deploy the M7 code of the arduino.
