import sqlite3
import time
import json


def open_connection(db_name: str = 'local.db') -> sqlite3.Connection:
    con = sqlite3.connect(db_name)
    cur = con.cursor()
    cur.execute('CREATE TABLE IF NOT EXISTS movement_reports '
                '(id INTEGER PRIMARY KEY AUTOINCREMENT,'
                'time INTEGER,'
                'box_id INTEGER,'
                'movement_type INTEGER);')
    cur.execute('CREATE TABLE IF NOT EXISTS states_and_transitions '
                '(id INTEGER PRIMARY KEY AUTOINCREMENT,'
                'time INTEGER,'
                'stset TEXT);')
    con.commit()
    return con


def close_connection(con: sqlite3.Connection) -> None:
    con.close()


def insert_movement_report(con: sqlite3.Connection, box_id: int, movement_type: int) -> None:
    cur = con.cursor()
    current_time = time.time_ns() / 1e6
    cur.execute(f'INSERT INTO movement_reports (time, box_id, movement_type) '
                f'VALUES ({current_time}, {box_id}, {movement_type});')
    con.commit()


def insert_stset(con: sqlite3.Connection, stset: list) -> None:
    cur = con.cursor()
    current_time = time.time_ns() / 1e6
    stset_json = json.dumps(stset)
    cur.execute(f'INSERT INTO states_and_transitions (time, stset) VALUES ({current_time}, {stset_json});')
    con.commit()
