import tkinter as tk

from realtime_tracker_2 import State


STATES_LUT = {
    State.SUPPLY_QUEUE: {
        'bg': 'green',
        'fg': 'white',
        'state_text': 'In supply on ramp',
    },
    State.IN_USE: {
        'bg': 'yellow',
        'fg': 'black',
        'state_text': 'In use',
    },
    State.READY_FOR_PICKUP: {
        'bg': 'red',
        'fg': 'white',
        'state_text': 'Waiting for pickup',
    },
    State.WAITING_MOVE_TO_WAREHOUSE: {
        'bg': 'maroon1',
        'fg': 'black',
        'state_text': 'Waiting for transport to warehouse',
    },
    State.MOVING_TO_WAREHOUSE: {
        'bg': 'maroon1',
        'fg': 'white',
        'state_text': 'Transporting to warehouse',
    },
    State.WAITING_FOR_REFILL: {
        'bg': 'dark violet',
        'fg': 'white',
        'state_text': 'In warehouse, waiting for refill',
    },
    State.REFILLING: {
        'bg': 'grey55',
        'fg': 'white',
        'state_text': 'Refilling',
    },
    State.WAITING_FOR_WAY_BACK: {
        'bg': 'grey72',
        'fg': 'white',
        'state_text': 'Refilled, waiting for transport',
    },
    State.MOVING_TO_PRODUCTION: {
        'bg': 'blue',
        'fg': 'white',
        'state_text': 'Transporting to production line',
    },
    State.WAITING_MOVE_TO_RAMP: {
        'bg': 'blue',
        'fg': 'white',
        'state_text': 'Waiting for arrival',
    },
}


class AdminWindow:

    def __init__(self, window):
        self.__window = window
        self.__boxlabels = {}
        self.__statuslabels = {}
        self.__window.update()

    def update(self, box_id: int, state: State):
        if box_id not in self.__boxlabels:
            boxlabel = tk.Label(text=f'Box #{box_id}', width=15, height=5)
            boxlabel.grid(column=0, row=box_id)
            self.__boxlabels[box_id] = boxlabel

            statuslabel = tk.Label(text='unknown', bg='grey', width=50, height=5)
            statuslabel.grid(column=1, row=box_id)
            self.__statuslabels[box_id] = statuslabel

        state_text = STATES_LUT[state]['state_text']
        new_text = f'{state_text}'

        fg = STATES_LUT[state]['fg']
        bg = STATES_LUT[state]['bg']
        self.__statuslabels[box_id].config(text=new_text, fg=fg, bg=bg)
        self.__window.update()
