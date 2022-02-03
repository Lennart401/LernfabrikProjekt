import paho.mqtt.client as mqtt
import dataparser
import dbconnector


def on_connect(connect_client, userdata, flags, rc):
    print(f'Connected with result code: {rc}')
    # any subscribtions in on_connect will be renewed when the client reconnects after a lost connection
    connect_client.subscribe('lernfabrik/#')


def on_message(connect_client, userdata, msg):
    print(f'{msg.topic}: {str(msg.payload)}')
    box_id, movement_type = dataparser.parse_movement_message(msg.topic, str(msg.payload))
    dbconnector.insert_movement_report(db_con, box_id, movement_type)


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

db_con = dbconnector.open_connection()

client.connect('localhost')
client.loop_forever()

dbconnector.close_connection(db_con)
