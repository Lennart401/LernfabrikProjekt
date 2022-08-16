import sqlite3
import time
import json
import pandas as pd
import paho.mqtt.client as mqtt


DB_FILE = 'local_5.db'
START_TIME = 1658585765751
# START_TIME = 1648468081000


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
        current_time = millis() - start_time
        box_id = round(next_point.box_id)
        topic = f'lernfabrik/box_{box_id}/movement'
        message = json.dumps({
            'mt': round(next_point.movement_type),
            'dist': next_point.distance
        })

        tabs = '\t\t\t\t\t' * (box_id - 1)
        print(f'{(current_time / 1e3):10.3f}:\t{tabs}box {box_id} -> mtype {round(next_point.movement_type)}, '
              f'dist {next_point.distance:4.2f}')

        client.publish(topic, message, retain=False)
        records_sent += 1

