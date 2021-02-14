from flask import Flask
from flask import request
import pandas as pd
import decode_header as dh
import decode_payload as dp
from datetime import datetime
import werkzeug


app = Flask(__name__)


@app.route("/")
def root():
    return "Hello, World!", 200


@app.route("/send", methods=["POST"])
def send():
    content = request.get_data()
    length = request.content_length
    print("length", length)

    version, device_id, header_size, sensors = dh.decode_header(content, length)
    print("device_id", device_id)
    print("header_size", header_size)
    print(sensors)

    payload = content[header_size:length]
    df = dp.decode_payload(version, payload, sensors)
    # print(df)

    df.to_csv(datetime.now().strftime(f"./saved2/dev_{device_id}_%Y-%m-%d_%H-%M-%S.csv"), index=False)

    return "OK", 200


@app.errorhandler(werkzeug.exceptions.BadRequest)
def handle_bad_request(e):
    print(e)
    print(request)
    return 'Bad Request', 400


if __name__ == "__main__":
    app.run(host="0.0.0.0")
