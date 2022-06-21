from enum import Enum
from collections import deque
from typing import Dict

import time


# SETTINGS
MOVING_DURATION = 5.0
ON_RAMP_DURATION = 10.0
WAIT_PICKUP_DURATION = 3.0
REFILLING_DURATION = 3.0


class State(Enum):
    SUPPLY_QUEUE = 1
    IN_USE = 2
    READY_FOR_PICKUP = 3
    WAITING_MOVE_TO_WAREHOUSE = 4
    MOVING_TO_WAREHOUSE = 5
    WAITING_FOR_REFILL = 6
    REFILLING = 7
    WAITING_FOR_WAY_BACK = 8
    MOVING_TO_PRODUCTION = 9
    WAITING_MOVE_TO_RAMP = 10


class MovementType(Enum):
    ON_RAMP = 1
    READY_FOR_PICKUP = 2
    ON_MOVING_WAGON = 3
    NO_MOVEMENT = 4
    GENERAL_MOVEMENT = 5
    THROW_ITEMS_INTO_BOX = 6


class Box:
    def __init__(self) -> None:
        self.state = State.SUPPLY_QUEUE
        self.__state_since = 0
        self.__last_movement_types = deque(maxlen=25)
        self.__is_moving = False
        self.__moved_since = 0

    def handle_movement_type(self, type: MovementType) -> None:
        # moving/not moving
        if type == MovementType.GENERAL_MOVEMENT and not self.__is_moving:
            self.__is_moving = True
            self.__moved_since = time.time()
        elif type != MovementType.GENERAL_MOVEMENT:
            self.__is_moving = False

        # handle the type by the given state method
        getattr(self, f'__handle_state_{self.state.value}')(type)

        # add to list of last movement types
        self.__last_movement_types.append(type)

    def __change_state(self, new_state: State):
        self.state = new_state
        self.__state_since = time.time()

    def __in_state_for(self, seconds: float) -> bool:
        return time.time() > self.__state_since + seconds

    def __has_moved_for(self, seconds: float) -> bool:
        return time.time() > self.__moved_since + seconds

    def __handle_state_1(self, type: MovementType) -> None:
        # SUPPLY_QUEUE
        if not self.__in_state_for(ON_RAMP_DURATION):
            return

        if type == MovementType.GENERAL_MOVEMENT:
            self.__change_state(State.IN_USE)
        elif type == MovementType.READY_FOR_PICKUP \
                and self.__has_moved_for(1.5):
            self.__change_state(State.READY_FOR_PICKUP)
        elif type == MovementType.NO_MOVEMENT:
            self.__change_state(State.WAITING_MOVE_TO_WAREHOUSE)

    def __handle_state_2(self, type: MovementType) -> None:
        # IN_USE
        if not self.__in_state_for(ON_RAMP_DURATION):
            return

        if type == MovementType.READY_FOR_PICKUP and self.__has_moved_for(1.5):
            self.__change_state(State.READY_FOR_PICKUP)
        elif type == MovementType.NO_MOVEMENT:
            self.__change_state(State.WAITING_MOVE_TO_WAREHOUSE)

    def __handle_state_3(self, type: MovementType) -> None:
        # READY FOR PICKUP
        if type == MovementType.NO_MOVEMENT \
                and self.__has_moved_for(1.5) \
                and self.__in_state_for(WAIT_PICKUP_DURATION):
            self.__change_state(State.WAITING_MOVE_TO_WAREHOUSE)

    def __handle_state_4(self, type: MovementType) -> None:
        # WAITING MOVE TO WAREHOUSE
        if type == MovementType.ON_MOVING_WAGON:
            self.__change_state(State.MOVING_TO_WAREHOUSE)
        
    def __handle_state_5(self, type: MovementType) -> None:
        # MOVING TO WAREHOUSE
        if type == MovementType.NO_MOVEMENT \
                and self.__in_state_for(MOVING_DURATION):
            self.__change_state(State.WAITING_FOR_REFILL)
    
    def __handle_state_6(self, type: MovementType) -> None:
        # WAITING FOR REFILL
        if type == MovementType.THROW_ITEMS_INTO_BOX:
            self.__change_state(State.REFILLING)
        # fallback: the box has moved for a least 2.5 seconds, then just assume
        # it has been refilled already
        elif type == MovementType.NO_MOVEMENT and self.__has_moved_for(2.5):
            self.__change_state(State.WAITING_FOR_WAY_BACK)
    
    def __handle_state_7(self, type: MovementType) -> None:
        # REFILLING
        if not self.__in_state_for(REFILLING_DURATION):
            return

        if type == MovementType.NO_MOVEMENT:
            self.__change_state(State.WAITING_FOR_WAY_BACK)
        elif type == MovementType.ON_MOVING_WAGON:
            self.__change_state(State.MOVING_TO_PRODUCTION)
    
    def __handle_state_8(self, type: MovementType) -> None:
        # WAITING FOR WAY BACK
        if type == MovementType.ON_MOVING_WAGON:
            self.__change_state(State.MOVING_TO_PRODUCTION)
    
    def __handle_state_9(self, type: MovementType) -> None:
        # MOVING TO PRODUCTION
        if type == MovementType.NO_MOVEMENT \
                and self.__in_state_for(MOVING_DURATION):
            self.__change_state(State.WAITING_MOVE_TO_RAMP)

        if type == MovementType.ON_RAMP and self.__has_moved_for(1):
            self.__change_state(State.SUPPLY_QUEUE)

    def __handle_state_10(self, type: MovementType) -> None:
        # WAITING MOVE TO RAMP
        if type == MovementType.ON_RAMP and self.__has_moved_for(1.5):
            self.__change_state(State.SUPPLY_QUEUE)
        # fallback: already on ramp but in state for more than 10 seconds
        elif type == MovementType.ON_RAMP and self.__in_state_for(10):
            self.__change_state(State.SUPPLY_QUEUE)


class RealtimeTracker2:
    def __init__(self) -> None:
        self.__boxes = {}

    def feed_movement_report(self, box_id: int, movement_type: int) -> None:
        if box_id not in self.__boxes:
            self.__boxes[box_id] = Box()
        
        self.__boxes[box_id].handle_movement_type(MovementType(movement_type))
    
    def get_state(self, box_id: int) -> State:
        if box_id not in self.__boxes:
            return -1
        else:
            return self.__boxes[box_id].state
    
    def get_all_states(self) -> Dict[int, State]:
        all_states = {}
        for id, box in self.__boxes.items():
            all_states[id] = box.state
        return all_states
