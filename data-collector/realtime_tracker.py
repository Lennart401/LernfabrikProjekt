import time
from typing import Union, Optional, Tuple, Dict

STATE_RULES = {
    1: {
        'label': 'Sitting on ramp',
        'length': 'min 5 seconds',
        'movement_ids': [1],
        'transition': {
            'movement_ids': [2, 3, 4, 5, 6, 7, 8],
            'length': 'max 2 records',
            'next_state': 2
        },
    },
    2: {
        'label': 'Sitting on ramp (already moved)',
        'length': 'min 5 seconds',
        'movement_ids': [1],
        'transition': {
            'movement_ids': [5, 7, 8],
            'length': 'min 3 records',
            'next_state': 3,
        },
    },
    3: {
        'label': 'Waiting for pickup',
        'length': 'min 5 seconds',
        'movement_ids': [2],
        'transition': {
            'movement_ids': [5, 7, 8],
            'length': 'min 2 records',
            'next_state': 4,
        },
    },
    4: {
        'label': 'Transporting to warehouse',
        'length': 'min 3 seconds',
        'movement_ids': [3, 4],
        'transition': {
            'movement_ids': [5, 6, 7, 8],
            'length': 'min 4 records',
            'next_state': 5,
        },
    },
    5: {
        'label': 'Transporting back to production',
        'length': 'min 3 seconds',
        'movement_ids': [3, 4],
        'transition': {
            'movement_ids': [5, 7, 8],
            'length': 'min 3 records',
            'next_state': 1,
        },
    },
}


def _is_state_type(state_id: int, movement_type: int) -> bool:
    return movement_type in STATE_RULES[state_id]['movement_ids']


def _is_transition_type(state_id: int, movement_type: int) -> bool:
    return movement_type in STATE_RULES[state_id]['transition']['movement_ids']


def _get_state_length_string(state_id: int) -> str:
    return STATE_RULES[state_id]['length']


def _get_transition_length_string(state_id: int) -> str:
    return STATE_RULES[state_id]['transition']['length']


def _meets_length_condition(length_string: str, records: int, since: float) -> bool:
    length_array = length_string.split(" ")
    length_array[1] = int(length_array[1])

    # check that the state length conditions have been fulfilled:
    result = False
    if length_array[0] == 'min':
        if (length_array[2] == 'records' and records >= length_array[1]) or \
                (length_array[2] == 'seconds' and time.time() >= since + length_array[1]):
            result = True
    elif length_array[0] == 'max':
        if (length_array[2] == 'records' and records <= length_array[1]) or \
                (length_array[2] == 'seconds' and time.time() <= since + length_array[1]):
            result = True
    return result


def _get_next_state_id(state_id: int) -> int:
    return STATE_RULES[state_id]['transition']['next_state']


class RealtimeTracker:
    def __init__(self):
        self.__dataset = {}
        # 'states and transitions' dataset
        self.__stset = {}

    def feed_movement_report(self, box_id: int, movement_type: int) -> None:
        # record the data for now, we prob don't need it
        if self.__dataset[box_id] is None:
            self.__dataset[box_id] = []

        self.__dataset[box_id].append({
            'time': time.time(),
            'box_id': box_id,
            'movement_type': movement_type,
        })

        self.__update_state(box_id, movement_type)

    def __update_state(self, box_id: int, movement_type: int) -> None:
        # if there is no stset record for this box, create one
        if box_id not in self.__stset:
            self.__stset[box_id] = {
                'state': {
                    'id': 1,
                    'since': 0,
                    'records': 100,
                },
                'transition': None,
                'last_movement': STATE_RULES[1]['movement_ids'][0],
            }

        # get the current values from the stset for ease of use and to rewrite them later
        current_state = self.__stset[box_id]['state']
        current_transition = self.__stset[box_id]['transition']

        # check if are not currently in a transition:
        if current_transition is None:
            current_state, current_transtition = self.__handle_no_transition(movement_type, current_state)

        # check if transition is already ongoing:
        elif current_transition is not None:
            current_state, current_transition = self.__handle_transition(movement_type, current_state,
                                                                         current_transition)

        self.__stset[box_id]['state'] = current_state
        self.__stset[box_id]['transition'] = current_transition
        self.__stset[box_id]['last_movement'] = movement_type

    def __handle_no_transition(self, movement_type: int, current_state: dict) -> Tuple[dict, Optional[dict]]:
        current_state_id = current_state['id']

        result_state = current_state
        result_transition = None

        # if the current movement type is a transition type for our current state, we can initate a transition
        # everything else will be ignored
        if _is_transition_type(current_state_id, movement_type):  # start transition
            length_string = _get_state_length_string(current_state_id)
            if _meets_length_condition(length_string, current_state['records'], current_state['since']):
                # state length condition has been fulfilled, we can create a transition
                result_transition = {
                    'since': time.time(),
                    'records': 1,
                }

        result_state['records'] += 1
        return result_state, result_transition

    def __handle_transition(self, movement_type: int, current_state: dict, current_transition: dict) -> \
            Tuple[dict, Optional[dict]]:
        current_state_id = current_state['id']
        next_state_id = _get_next_state_id(current_state_id)

        result_state = current_state
        result_transition = current_transition

        # type 1: we are still in transition, then count up the record count
        if _is_transition_type(current_state_id, movement_type):
            result_transition['records'] += 1

        # type 2: we are exiting the transition
        elif _is_state_type(next_state_id, movement_type):
            length_string = _get_transition_length_string(current_state_id)
            if _meets_length_condition(length_string, current_transition['records'], current_transition['since']):
                # transition length condition has been fulfilled, we can switch to next state
                result_state = {
                    'id': next_state_id,
                    'since': time.time(),
                    'records': 0,
                }
                result_transition = None

        # type 3: we are back to the original state
        elif _is_state_type(current_state_id, movement_type):
            result_state['records'] += 1
            result_transition = None

        return result_state, result_transition

    def state_summary(self) -> str:
        summary = ''
        for box_id in self.__stset:
            summary += f'---------------------------\n' \
                       f'Box #{box_id}\n' \
                       f'    Current State: {self.get_state(box_id)}\n'
            transition, next_state = self.get_transition(box_id)
            if transition:
                summary += f'    In transition to state {next_state}\n'
            else:
                summary += f'    No transition\n'
            summary += '\n'

    def get_state(self, box_id: int) -> int:
        if box_id not in self.__stset:
            return -1
        else:
            return self.__stset[box_id]['state']['id']

    def get_transition(self, box_id: int) -> Tuple[bool, Optional[int]]:
        if box_id in self.__stset and self.__stset[box_id]['transition'] is not None:
            return True, STATE_RULES[self.get_state(box_id)]['transition']['next_state']
        else:
            return False, -1

    def get_history(self, box_id):
        pass
