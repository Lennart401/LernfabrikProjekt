import paho.mqtt.client as mqtt
import dataparser
import json

from realtime_tracker_2 import RealtimeTracker2


def on_connect(client, userdata, flags, rc):
    """Handle connection to an MQTT broker.

    When connected to an MQTT broker, subscribe to the topic 'lernfabrik/+/movement'.

    Args:
        client: The client that connected to the broker.
        userdata: The user data.
        flags: The flags.
        rc: The result code.
    """
    print(f'Connected with result code {str(rc)}')
    client.subscribe('lernfabrik/+/movement')


def on_message(client, userdata, msg):
    """Handle an incoming MQTT message.

    When an MQTT message is received, parse it and feed it into the RealtimeTracker2 instance. Then publish the updated state
    of all boxes to the topic 'lernfabrik/all_states'.

    Args:
        client: The client that received the message.
        userdata: The user data.
        msg: The message.
    """
    topic = msg.topic
    payload = msg.payload.decode('utf-8')

    cleaned_payload = ' '.join(payload.split())
    print(f'Message received: {topic} - {cleaned_payload}')

    topic_parts = topic.split('/')
    if topic_parts[0] != 'lernfabrik':
        print(f'Unknown topic: {topic}')
        return

    # parse the message and feed it into the RealtimeTracker2 instance
    try:
        box_id, movement_type, distance = dataparser.parse_movement_message(topic, payload)
        rtt2.feed_movement_report(box_id, movement_type, distance)
    except ValueError as e:
        print(f'Error parsing message: {e}')
        return

    # publish the updated state of the box and the full set of all states to the broker
    all_states = rtt2.get_all_states()
    message = [{'box_id': box_id, 'state': state} for box_id, state in all_states.items()]
    client.publish(f'lernfabrik/box_{box_id}/state', json.dumps(rtt2.get_state(box_id)), retain=True, qos=2)
    client.publish('lernfabrik/all_states', json.dumps(message), retain=True, qos=2)


if __name__ == '__main__':
    # The instance of the RealtimeTracker2 class, which keeps track of the state of all boxes.
    rtt2 = RealtimeTracker2()

    # Create a new MQTT client and connect to the broker
    mqtt_client = mqtt.Client()
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message
    mqtt_client.connect('localhost', 1883, 60)
    mqtt_client.loop_forever()
