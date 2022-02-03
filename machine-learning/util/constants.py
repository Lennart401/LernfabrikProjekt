LUT_MOVEMENT_ID_TO_NAME = {
    1: "On ramp",
    2: "Ready for pickup",
    3: "On moving wagon",
    4: "No movement",
    5: "General movement",
    6: "Throw items into box",
    7: "Put the box down",
    8: "Pick the box up",
}
LUT_INDEX_TO_NAME = {key - 1: value for key, value in LUT_MOVEMENT_ID_TO_NAME.items()}

LUT_MOVEMENT_ID_TO_INDEX = {key: key - 1 for key in LUT_MOVEMENT_ID_TO_NAME}

LUT_MOVEMENT_ID_TO_COLOR = {
    1: "red",
    2: "green",
    3: "blue",
    4: "black",
    5: "yellow",
    6: "darkred",
    7: "indigo",
    8: "darkviolet"
}
