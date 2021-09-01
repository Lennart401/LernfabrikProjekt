from flask import Flask
from flask import request
import pandas as pd
import decode_header as dh
import decode_payload as dp
from datetime import datetime
import werkzeug
import os


app = Flask(__name__)


@app.route("/")
def root():
    return "Hello, World!", 200


@app.route("/send", methods=["POST"])
def send():
    content = request.get_data()
    length = request.content_length
    print("length", length)

    # ------------------------------------------------------------------------------------------------------------------
    # decode the header
    header = dh.decode_header(content, length)
    header_version = header["version"]

    device_id = header["metadata"]["device_id"]
    header_size = header["header_size"]
    sensors = header["sensors"]

    # print debug information
    print("device_id", device_id)
    print("header_size", header_size)
    print("metadata", header["metadata"])
    print(sensors)

    # decode the content
    payload = content[header_size:length]
    df = dp.decode_payload(header["version"], payload, sensors)

    # ------------------------------------------------------------------------------------------------------------------
    # with version 1, just save the contents
    if header_version == 1:
        path = f"./saved4/"
        if not os.path.exists(path):
            os.makedirs(path)

        df.to_csv(datetime.now().strftime(f"{path}dev_{device_id}_%Y-%m-%d_%H-%M-%S.csv"), index=False)

    # with version 2, save the contents with packet id, time and frequency in the corresponding movement folder
    elif header_version == 2:
        # ensure the folder is created
        movement_type = header["metadata"]["movement_type"]
        path = f"./movements/{movement_type}/"
        if not os.path.exists(path):
            os.makedirs(path)

        # write the contents to csv
        packet_id = header["metadata"]["packet_id"]
        frequency = header["metadata"]["frequency"]
        df.to_csv(datetime.now().strftime(f"{path}dev_{device_id}_pkt_{packet_id}_frq_{frequency}_%Y%m%d_%H%M%S.csv"),
                  index=False)

    return "", 200


@app.errorhandler(werkzeug.exceptions.BadRequest)
def handle_bad_request(e):
    print(e)
    print(request)
    return 'Bad Request', 400


if __name__ == "__main__":
    app.run(host="0.0.0.0")
