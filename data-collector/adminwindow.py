import tkinter as tk

from typing import Optional


STATES_LUT = {
    1: {
        'bg': 'green',
        'fg': 'white',
        'state_text': 'On ramp, full',
        'transition_text': 'sliding down',
    },
    2: {
        'bg': 'yellow',
        'fg': 'black',
        'state_text': 'On ramp, in use',
        'transition_text': 'moving off ramp',
    },
    3: {
        'bg': 'red',
        'fg': 'white',
        'state_text': 'Waiting for pickup',
        'transition_text': 'loading onto wagon',
    },
    4: {
        'bg': 'purple',
        'fg': 'white',
        'state_text': 'Transporting to warehouse',
        'transition_text': 'refilling',
    },
    5: {
        'bg': 'blue',
        'fg': 'white',
        'state_text': 'Transporting to production line',
        'transition_text': 'lifting onto ramp',
    }
}


class AdminWindow:

    def __init__(self, window):
        self.__window = window
        self.__boxlabels = {}
        self.__statuslabels = {}
        self.__window.update()

    def update(self, box_id: int, state: int, transition: Optional[int]):
        if box_id not in self.__boxlabels:
            boxlabel = tk.Label(text=f'Box #{box_id}', width=15, height=5)
            boxlabel.grid(column=0, row=box_id)
            self.__boxlabels[box_id] = boxlabel

            statuslabel = tk.Label(text='unknown', bg='grey', width=50, height=5)
            statuslabel.grid(column=1, row=box_id)
            self.__statuslabels[box_id] = statuslabel

        state_text = STATES_LUT[state]['state_text']
        transition_text = STATES_LUT[state]['transition_text']
        new_text = f'{state_text}' if transition is None else f'{state_text} ({transition_text}...)'

        fg = STATES_LUT[state]['fg']
        bg = STATES_LUT[state]['bg']
        self.__statuslabels[box_id].config(text=new_text, fg=fg, bg=bg)
        self.__window.update()
