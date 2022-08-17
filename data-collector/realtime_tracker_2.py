import time

from enum import Enum
from collections import deque
from typing import Dict, Optional

# SETTINGS
MOVING_DURATION = 5.0
ON_RAMP_DURATION = 10.0
WAIT_PICKUP_DURATION = 3.0

MIN_MOVING_WAGON = 4.0
MIN_STOPPED_WAGON = 3.0


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
        self.__last_distances = deque(maxlen=25)
        self.__is_moving = False
        self.__moved_since = 0

    # PUBLIC FUNCTIONS

    def handle_movement_type(self, mtype: MovementType, distance: float) -> None:
        # moving/not moving
        current_time = time.time()

        if mtype == MovementType.GENERAL_MOVEMENT and not self.__is_moving:
            self.__is_moving = True
            self.__moved_since = time.time()
        elif mtype != MovementType.GENERAL_MOVEMENT:
            self.__is_moving = False

        # handle the type by the given state method
        getattr(self, f'_Box__handle_state_{self.state.value}')(mtype, distance)

        # add to list of last movement types / last distances
        self.__last_movement_types.append((current_time, mtype))
        self.__last_distances.append((current_time, distance))

    # HELPER FUNCTIONS

    def __change_state(self, new_state: State):
        self.state = new_state
        self.__state_since = time.time()
        self.__moved_since = float('inf')

    def __in_state_for(self, seconds: float) -> bool:
        return time.time() > self.__state_since + seconds

    def __has_moved_for(self, seconds: float) -> bool:
        return time.time() > self.__moved_since + seconds

    def __is_type_without_outliers(self, current_type: MovementType, wanted_type: MovementType, num_history: int = 1):
        if current_type != wanted_type:
            return False

        for i in range(1, min(num_history + 1, len(self.__last_movement_types) + 1)):
            if self.__last_movement_types[-i][1] != wanted_type:
                return False
        return True

    def __is_type_for(self, current_type: MovementType, wanted_type: MovementType, seconds: float) -> bool:
        if current_type != wanted_type:
            return False

        current_time = time.time()

        for i in range(1, len(self.__last_movement_types) + 1):
            if self.__last_movement_types[-i][1] != wanted_type:
                return False
            elif self.__last_movement_types[-i][0] + seconds <= current_time:
                return True
        return False

    def __is_distance_increasing(self, current_distance: float, for_duration: float, tolerance: float = 0.25):
        current_time = time.time()

        last_distance = current_distance
        for i in range(1, len(self.__last_distances) + 1):
            if self.__last_distances[-i][1] >= last_distance - tolerance:
                return False
            elif self.__last_distances[-i][0] + for_duration <= current_time:
                print(current_distance, self.__last_distances)
                return True
            last_distance = self.__last_distances[-i][1]
        return False

    def __is_distance_decreasing(self, current_distance: float, for_duration: float, tolerance: float = 0.25):
        current_time = time.time()

        last_distance = current_distance
        for i in range(1, len(self.__last_distances) + 1):
            if self.__last_distances[-i][1] <= last_distance + tolerance:
                return False
            elif self.__last_distances[-i][0] + for_duration <= current_time:
                return True
            last_distance = self.__last_distances[-i][1]
        return False

    def __is_distance_stable(self, distance: float, for_duration: float, tolerance: float = 0.5) -> bool:
        current_time = time.time()

        for i in range(1, len(self.__last_distances) + 1):
            if abs(self.__last_distances[-i][1] - distance) > tolerance:
                return False
            elif self.__last_distances[-i][0] + for_duration <= current_time:
                return True
        return False

    # STATE SPECIFIC FUNCTIONS

    def __handle_state_1(self, mtype: MovementType, distance: float) -> None:
        # SUPPLY_QUEUE
        if not self.__in_state_for(ON_RAMP_DURATION):
            return

        # Nominal progression 1: the box moves down on the ramp
        if mtype == MovementType.GENERAL_MOVEMENT:
            self.__change_state(State.IN_USE)

        # Nominal progression 2: the distance is increasing, meaning material is being taken out of the box
        elif self.__is_distance_increasing(distance, for_duration=2.0):
            self.__change_state(State.IN_USE)

        # Fallback 1: already ready for pickup
        elif self.__is_type_without_outliers(mtype, MovementType.READY_FOR_PICKUP, num_history=1) \
                and self.__has_moved_for(1.0):
            self.__change_state(State.READY_FOR_PICKUP)

        # Fallback 2: already waiting on the wagon
        elif self.__is_type_without_outliers(mtype, MovementType.READY_FOR_PICKUP, num_history=2):
            self.__change_state(State.WAITING_MOVE_TO_WAREHOUSE)

    def __handle_state_2(self, mtype: MovementType, _) -> None:
        # IN_USE
        if not self.__in_state_for(ON_RAMP_DURATION):
            return

        # Nominal progression: the box is put on the rail for pickup
        if self.__is_type_without_outliers(mtype, MovementType.READY_FOR_PICKUP, num_history=1):
            self.__change_state(State.READY_FOR_PICKUP)

        # Fallback 1: already waiting on the wagon
        elif self.__is_type_without_outliers(mtype, MovementType.NO_MOVEMENT, num_history=2):
            self.__change_state(State.WAITING_MOVE_TO_WAREHOUSE)

        # Fallback 2: already moving on the wagon
        elif self.__is_type_without_outliers(mtype, MovementType.ON_MOVING_WAGON, num_history=2):
            self.__change_state(State.MOVING_TO_WAREHOUSE)

    def __handle_state_3(self, mtype: MovementType, _) -> None:
        # READY FOR PICKUP
        if self.__is_type_without_outliers(mtype, MovementType.NO_MOVEMENT, num_history=1) \
                and self.__in_state_for(WAIT_PICKUP_DURATION):
            self.__change_state(State.WAITING_MOVE_TO_WAREHOUSE)

        # Fallback: wagon is already moving
        elif self.__is_type_without_outliers(mtype, MovementType.ON_MOVING_WAGON, num_history=2):
            self.__change_state(State.MOVING_TO_WAREHOUSE)

    def __handle_state_4(self, mtype: MovementType, _) -> None:
        # WAITING MOVE TO WAREHOUSE
        if self.__is_type_without_outliers(mtype, MovementType.ON_MOVING_WAGON, num_history=1):
            self.__change_state(State.MOVING_TO_WAREHOUSE)
        
    def __handle_state_5(self, mtype: MovementType, distance: float) -> None:
        # MOVING TO WAREHOUSE
        if self.__is_type_without_outliers(mtype, MovementType.NO_MOVEMENT, num_history=1) \
                and self.__in_state_for(MOVING_DURATION):
            self.__change_state(State.WAITING_FOR_REFILL)

        # Fallback 1: the distance is decreasing, meaning material is being put into the box
        elif self.__is_distance_decreasing(distance, for_duration=2.0):
            self.__change_state(State.REFILLING)
    
    def __handle_state_6(self, mtype: MovementType, distance: float) -> None:
        # WAITING FOR REFILL
        if self.__is_type_without_outliers(mtype, MovementType.THROW_ITEMS_INTO_BOX, num_history=1):
            self.__change_state(State.REFILLING)

        # Fallback 1: the distance is decreasing, meaning material is being put into the box
        elif self.__is_distance_decreasing(distance, for_duration=2.0):
            self.__change_state(State.REFILLING)

        # Fallback 2: the box has moved for at least 2.5 seconds, then just assume it has been refilled already
        elif self.__is_type_without_outliers(mtype, MovementType.NO_MOVEMENT, num_history=1) \
                and self.__has_moved_for(2.5):
            self.__change_state(State.WAITING_FOR_WAY_BACK)
    
    def __handle_state_7(self, mtype: MovementType, distance: float) -> None:
        # REFILLING
        if self.__is_distance_stable(distance, for_duration=2.0, tolerance=1.0):
            self.__change_state(State.WAITING_FOR_WAY_BACK)

        # Nominal progression: the box is put back down onto the wagon
        if self.__is_type_without_outliers(mtype, MovementType.NO_MOVEMENT, num_history=1):
            self.__change_state(State.WAITING_FOR_WAY_BACK)

        # Fallback: the wagon immediately starts moving again, skipping state 8
        elif self.__is_type_without_outliers(mtype, MovementType.ON_MOVING_WAGON, num_history=1):
            self.__change_state(State.MOVING_TO_PRODUCTION)
    
    def __handle_state_8(self, mtype: MovementType, _) -> None:
        # WAITING FOR WAY BACK
        if self.__is_type_without_outliers(mtype, MovementType.ON_MOVING_WAGON, num_history=1):
            self.__change_state(State.MOVING_TO_PRODUCTION)
    
    def __handle_state_9(self, mtype: MovementType, _) -> None:
        # MOVING TO PRODUCTION
        # Nominal progression: the wagon has stopped for a certain amount of time,
        # and we have already benn moving for a while
        if self.__is_type_without_outliers(mtype, MovementType.NO_MOVEMENT, num_history=1) \
                and self.__in_state_for(MOVING_DURATION):
            self.__change_state(State.WAITING_MOVE_TO_RAMP)

        # Fallback: the box is directly put on the ramp, skipping state 10
        elif self.__is_type_without_outliers(mtype, MovementType.ON_RAMP, num_history=2):
            self.__change_state(State.SUPPLY_QUEUE)

    def __handle_state_10(self, mtype: MovementType, _) -> None:
        # WAITING MOVE TO RAMP
        if self.__is_type_without_outliers(mtype, MovementType.ON_RAMP, num_history=1) and self.__has_moved_for(1.0):
            self.__change_state(State.SUPPLY_QUEUE)

        # Fallback: already on ramp but in state for more than 10 seconds
        elif mtype == MovementType.ON_RAMP and self.__in_state_for(10):
            self.__change_state(State.SUPPLY_QUEUE)


class RealtimeTracker2:
    def __init__(self) -> None:
        self.__boxes = {}

    def feed_movement_report(self, box_id: int, movement_type: int, distance: float) -> None:
        if box_id not in self.__boxes:
            self.__boxes[box_id] = Box()
        
        self.__boxes[box_id].handle_movement_type(MovementType(movement_type), distance)
    
    def get_state(self, box_id: int) -> Optional[State]:
        if box_id not in self.__boxes:
            return None
        else:
            return self.__boxes[box_id].state
    
    def get_all_states(self) -> Dict[int, State]:
        all_states = {}
        for box_id, box in self.__boxes.items():
            all_states[box_id] = box.state
        return all_states
