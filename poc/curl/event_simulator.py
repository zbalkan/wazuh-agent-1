import sqlite3
import random
import time
from datetime import datetime

def insert_event(event_data, event_type):
    try:
        conn = sqlite3.connect('build/sqlite3_events.db')
        c = conn.cursor()
        c.execute("INSERT INTO events (event_data, event_type) VALUES (?, ?);", (event_data, event_type))
        conn.commit()
    except sqlite3.Error as e:
        print(f"Database error: {e}")
    except Exception as e:
        print(f"Exception in insert_event: {e}")
    finally:
        if conn:
            conn.close()

def simulate_event_insertion(idle_length):
    event_types = ["type1", "type2", "type3"]

    while True:
        burst_length = random.randint(1, 20)
        for _ in range(burst_length):
            event_data = f"event_{random.randint(1, 1000)}"
            event_type = random.choice(event_types)
            insert_event(event_data, event_type)
            print(f"Inserted event: {event_data}, {event_type} at {datetime.now()}")
            time.sleep(random.uniform(0.1, 0.5))

        print(f"idle period for {idle_length} seconds at {datetime.now()}")
        time.sleep(idle_length)

if __name__ == "__main__":
    idle_length = 5
    simulate_event_insertion(idle_length)
