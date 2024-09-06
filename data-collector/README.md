# Lernfabrik Data Collector / Relay

This is the data collector and realtime tracker for the Lernfabrik Boxen project. It tracks and records all states of all boxes, providing information about the current state of each box, relying on a rule-based state machine. The data collector / realtime tracker relies on a running MQTT broker (e.g. mosquitto) with no authentication. The host of the broker must be adjusted in each respective file.

## Modes of Operation

This folder contains code for different modes of operation, that all rely on trained boxes that report their status during for Lernfabrik operation. The modes are the following:

1. Tracking and Recording: this mode is started by running `main.py`. It will connect to a MQTT broker, listen to box messages and write them into a SQLite database for replaying. Box messages are processed using the realtime tracker and displayed in a simple tkinter window to monitor the current states of the boxes.
2. Tracking and Relaying via MQTT: this mode is started by running `mqtt_provider.py`. It will connect to a MQTT broker, listen for box messages and process them using the realtime tracker. Both the box state and all states are then publish on specific MQTT topics for processing / displaying using other (frontend) applications.
3. Tracking and Relaying via websockets: this mode is start by running `websocket_provider.py`. It will connect to a MQTT broker, listen for box messages and process them using the realtime tracker. It will also open a websocket server and accept connections from client. Once a box messages comes in and is processed, the updated states will be sent to every websocket client connected in JSON format.
4. Simulating boxes by replaying from a database: this mode is start by running `simulator.py`. It reads movement reports from a SQLite database and sends then to a MQTT broker with correct differential timing, essentially replaying what the boxes did. This is useful for testing or recording visualizations.

## Structure

- `adminwindow.py` -- Module for opening a (unreactive) tkinter window for displaying box states.
- `dataparser.py` -- Function for parsing a movement/box message from MQTT (all messages under the lernfabrik/+/movement topics).
- `dbconnector.py` -- Module that contains function for connecting to SQLite database and writing movement reports / realtime tracker states.
- `main.py` -- Runner for "Tracking and Recording" mode of operation (see details in [Modes of Operation](##Modes-of-Operation)).
- `mqtt_provider.py` -- Runner for "Tracking and Relaying via MQTT" mode of operation (see details in [Modes of Operation](##Modes-of-Operation)).
- `realtime_tracker.py` -- Module that contains the original realtime tracker used in by Bachelor Thesis.
- `realtime_tracker_2.py` -- Module that contains the updated realtime tracker used for follow-up projects, e.g. [Rokoss et al. (2023) - KI-Kanban-Behälter: Mobile, dezentrale Logistikdatenerfassung mittels Edge-ML. In: ZWF Zeitschrift für Wissenschaftlichen Fabrikbetrieb 118(1-2), S. 59-63. DOI: 10.1515/zwf-2023-1004](https://www.degruyter.com/document/doi/10.1515/zwf-2023-1004/html).
- `simulator.py` -- Runner for "Simulating boxes by replaying from a database" mode of operation (see details in [Modes of Operation](##Modes-of-Operation)).
- `websocket_provider.py` -- Runner for "Tracking and Relaying via websockets" mode of operation (see details in [Modes of Operation](##Modes-of-Operation)).

## The state machine

For the BA scenario, every box follows a fixed pattern around the factory. The nomial pattern looks like this:

| ID  | Type       | Movement types                                                                     | Time                                                | Description                                  |
|-----|------------|------------------------------------------------------------------------------------|-----------------------------------------------------|----------------------------------------------|
| 1   | State      | 1 (On ramp)                                                                        | min 5 sec.                                          | Box sitting with material on the ramp        |
|     | Transition | Any state > 1                                                                      | min 1 sec.                                          | Box sliding down to first position           |
| 2   | State      | 1 (On ramp)                                                                        | min 5 sec.                                          | Box sitting on ramp in first position        |
|     | Transition | 8 (Pick up)<br/>5 (General movement)<br/>7 (Put down)                              | min 2 sec.                                          | Put the box to pickup rail                   |
| 3   | State      | 2 (Ready for pickup)                                                               | min 5 sec.                                          | Box sitting on the pickup rail               |
|     | Transition | 8 (Pick up)<br/>5 (General movement)<br/>7 (Put down)                              | min 2 sec.                                          | Put the box onto the wagon                   |
| 4   | State      | 4 (No movement)                                                                    | optional; but this movement also triggers the state | Box sitting still on the wagon               |
| 4   | State      | 3 (On moving wagon)                                                                | min 4 sec.                                          | Box moving on the wagon (to the warehouse)   |
|     | Transition | 8 (Pick up)<br/>5 (General movement)<br/>6 (Throw items into box)<br/>7 (Put down) | min 4 sec.                                          | Pick the box up and put items into it        |
| 5   | State      | 4 (No movement)                                                                    | optional; but this movement also triggers the state | Box sitting still on the wagon               |
| 5   | State      | 3 (On moving wagon)                                                                | min 4 sec.                                          | Box moving on the wagon (back to production) |
|     | Transition | 8 (Pick up)<br/>5 (General movement)<br/>7 (Put down)                              | min 2 sec.                                          | Put the box back onto the ramp               |

This pattern means that every box has essentially 5 different states it can be in + a bunch of transitions.
