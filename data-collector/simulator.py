import sqlite3
import time
import pandas as pd
import paho.mqtt.client as mqtt


DB_FILE = 'local (1).db'
START_TIME = 1648467794191
# START_TIME = 1648468081000
BOX_ID = 1


def millis():
    return round(time.time_ns() / 1e6)


con = sqlite3.connect(DB_FILE)
df = pd.read_sql(f'SELECT * FROM movement_reports WHERE time >= {START_TIME}', con=con)
con.close()

client = mqtt.Client(client_id='simulator')
client.connect('localhost')
client.loop_start()

offset = df.iloc[0].time
start_time = millis()
records_sent = 0

while records_sent < len(df):
    next_point = df.iloc[records_sent]
    if millis() >= start_time + (next_point.time - offset):
        topic = f'lernfabrik/box_{BOX_ID}/movement'
        message = f'{int(next_point.movement_type)}'
        print(f'publishing point no. {records_sent} with movement type {message}')
        client.publish(topic, message, retain=True)
        records_sent += 1

