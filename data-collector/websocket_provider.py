"""Server that connects to MQTT, tracks the states of boxes and broadcasts them via websockets.

This server connects to a local MQTT broker and listens for messages of Lernfabrik Boxes. The module dataparser is used to parse
the incoming MQTT messages. Messages are fed into a RealtimeTracker2 instance to track the states of the boxes. Each update is broadcasted
to all connected websockets clients.
"""
import asyncio
import websockets
import paho.mqtt.client as mqtt
import dataparser
import json
import os
import logging

from realtime_tracker_2 import RealtimeTracker2


# keep track of all connected websocket clients
all_clients = []
n_workstations = 0
workstation_assignments = []


async def handler(websocket):
    """Handle a websockets client.

    When connecting, a client is added to the global list of all_clients. Incoming messages of type 'workstations' are used to update
    the number of workstations and the workstation assignments. The update is then broadcasted to all connected websockets clients.

    Args:
        websocket: The connecting websocket client.
    """
    all_clients.append(websocket)

    async for message in websocket:
        event = json.loads(message)
        if event['type'] == 'workstations':
            global n_workstations
            global workstation_assignments
            n_workstations = event['n_workstations']
            workstation_assignments = event['workstation_assignments']
            logging.info(f'Changing: Workstations: {n_workstations}, Assignments: {workstation_assignments}')

            # forward the message to all connected websockets clients
            websockets.broadcast(all_clients, message)

    all_clients.remove(websocket)


async def run_websockets_server():
    """Run the websockets server."""
    async with websockets.serve(handler, '', 8765):
        await asyncio.Future()  # run forever


def on_connect(client, userdata, flags, rc):
    """Handle connection to an MQTT broker.

    When connected to an MQTT broker, subscribe to the topic 'lernfabrik/+/movement'.

    Args:
        client: The client that connected to the broker.
        userdata: The user data.
        flags: The flags.
        rc: The result code.
    """
    logging.info(f'Connected with result code {str(rc)}')
    client.subscribe('lernfabrik/+/movement')


def on_message(client, userdata, msg):
    """Handle an incoming MQTT message.

    When an MQTT message is received, parse it and feed it into the RealtimeTracker2 instance. The updated states are then immediately
    broadcasted to all connected websockets clients.

    Args:
        client: The client that received the message.
        userdata: The user data.
        msg: The message.
    """
    topic = msg.topic
    payload = msg.payload.decode('utf-8')

    cleaned_payload = ' '.join(payload.split())
    logging.info(f'Message received: {topic} - {cleaned_payload}')

    # parse the message and feed it into the RealtimeTracker2 instance
    box_id, movement_type, distance = dataparser.parse_movement_message(topic, payload)
    rtt2.feed_movement_report(box_id, movement_type, distance)

    # broadcast a all_states update to all connected websockets clients
    all_states = rtt2.get_all_states()
    message = {
        'type': 'all_states',
        'states': [{'box_id': box_id, 'state': state} for box_id, state in all_states.items()]
    }
    websockets.broadcast(all_clients, json.dumps(message))


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)

    rtt2 = RealtimeTracker2()
    
    mqtt_host = os.environ.get('MQTT_HOST', 'localhost')
    mqtt_port = int(os.environ.get('MQTT_PORT', '1883'))

    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(mqtt_host, mqtt_port, 60)
    client.loop_start()

    asyncio.run(run_websockets_server())
