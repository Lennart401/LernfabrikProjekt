TOPIC_PREFIX = 'lernfabrik/box_'


def parse_movement_message(topic: str, payload: str):
    if not topic.startswith(TOPIC_PREFIX):
        return None

    box_id = int(topic[len(TOPIC_PREFIX):topic.find('/', len(TOPIC_PREFIX))])
    return box_id, int(payload)
