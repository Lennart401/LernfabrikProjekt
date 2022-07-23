import json


TOPIC_PREFIX = 'lernfabrik/box_'


def parse_movement_message(topic: str, payload: str):
    if not topic.startswith(TOPIC_PREFIX):
        return None

    box_id = int(topic[len(TOPIC_PREFIX):topic.find('/', len(TOPIC_PREFIX))])
    payload_json = json.loads(payload.replace('\'', '\"'))
    movement_type = payload_json['mt']
    distance = payload_json['dist']

    return box_id, movement_type, distance
