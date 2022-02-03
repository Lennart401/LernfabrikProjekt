# Lernfabrik Data Collector

This is the data collector and realtime tracker for the Lernfabrik Boxen project. It tracks and records all states of
all boxes, also giving information about the current state of each box, relying on a rule-based state machine.

--> For BA: this could be done with a ML approach, if enough data was available

## The state machine

For the BA scenario, every box follows a fixed pattern around the factory. The nomial pattern looks like this:

| ID  | Type       | Movement types                                                                     | Time                                                | Description                                  |
|-----|------------|------------------------------------------------------------------------------------|-----------------------------------------------------|----------------------------------------------|
| 1   | State      | 1 (On ramp)                                                                        | min 5 sec.                                          | Box sitting with material on the ramp        |
|     | Transition | Any state > 1                                                                      | max 2 records.                                      | Box sliding down to first position           |
| 2   | State      | 1 (On ramp)                                                                        | min 5 sec.                                          | Box sitting on ramp in first position        |
|     | Transition | 8 (Pick up)<br/>5 (General movement)<br/>7 (Put down)                              | min 1 record each                                   | Put the box to pickup rail                   |
| 3   | State      | 2 (Ready for pickup)                                                               | min 5 sec.                                          | Box sitting on the pickup rail               |
|     | Transition | 8 (Pick up)<br/>5 (General movement)<br/>7 (Put down)                              | min 1 record each                                   | Put the box onto the wagon                   |
| 4   | State      | 4 (No movement)                                                                    | optional; but this movement also triggers the state | Box sitting still on the wagon               |
| 4   | State      | 3 (On moving wagon)                                                                | min 3 sec.                                          | Box moving on the wagon (to the warehouse)   |
|     | Transition | 8 (Pick up)<br/>5 (General movement)<br/>6 (Throw items into box)<br/>7 (Put down) | min 3 sec.                                          | Pick the box up and put items into it        |
| 5   | State      | 4 (No movement)                                                                    | optional; but this movement also triggers the state | Box sitting still on the wagon               |
| 5   | State      | 3 (On moving wagon)                                                                | min 3 sec.                                          | Box moving on the wagon (back to production) |
|     | Transition | 8 (Pick up)<br/>5 (General movement)<br/>7 (Put down)                              | min 1 record each                                   | Put the box back onto the ramp               |

This pattern means that every box has essentially 5 different states it can be in + a bunch of transitions.
