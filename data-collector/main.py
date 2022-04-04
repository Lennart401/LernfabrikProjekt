from realtime_tracker import RealtimeTracker
from adminwindow import AdminWindow

import paho.mqtt.client as mqtt
import tkinter as tk
import dataparser
import dbconnector


def on_connect(connect_client, userdata, flags, rc):
    print(f'Connected with result code: {rc}')
    # any subscribtions in on_connect will be renewed when the client reconnects after a lost connection
    connect_client.subscribe('lernfabrik/+/movement')


def on_message(connect_client, userdata, msg):
    topic = msg.topic
    payload = msg.payload.decode('utf-8')

    print(f'Received message: {topic} - {payload}')

    # get and insert
    box_id, movement_type = dataparser.parse_movement_message(topic, payload)
    dbconnector.insert_movement_report(db_con, box_id, movement_type)
    rttracker.feed_movement_report(box_id, movement_type)

    # update window
    state = rttracker.get_state(box_id)
    transition = rttracker.get_transition(box_id)[1]
    ui.update(box_id, state, transition)

    # display states and transitions
    stset = rttracker.get_all_states_and_transition()
    dbconnector.insert_stset(db_con, stset)
    print(rttracker.state_summary())


if __name__ == '__main__':
    rttracker = RealtimeTracker()
    db_con = dbconnector.open_connection()
    root = tk.Tk()
    ui = AdminWindow(root)

    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect('localhost')
    client.loop_forever()

    dbconnector.close_connection(db_con)
