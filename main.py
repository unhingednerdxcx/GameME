from Bridge import comm
import json
import traceback
import time
import os

"""
	IN THE JSON FILE in the MainGame folder "x and y this may be changed to 1024, depends on your ADC DELETE THIS LINE",
"""

script_folder = os.path.dirname(os.path.abspath(__file__))
json_path = os.path.join(script_folder, "MainGame", "Others", "data.json")

while True:
    data = comm()
    print("Parsed Data:", data)
    if data:
        print("Data received successfully.")
    else:
        print("No data received or an error occurred.")
        print(data)
    try:
        a = data[0]
        b = data[1]
        c = data[2]
        d = data[3]
        e = data[4]
        f = data[5]
        x = data[6]
        y = data[7]
        end = data[8]

        chunk = {
            "a": a,
            "b": b,
            "c": c,
            "d": d,
            "e": e,
            "f": f,
            "x": x,
            "y": y,
            "end": end
        }
        with open(json_path, "w") as file:
            json.dump(chunk, file, indent=4)
        print("Data saved to data.json")

    except Exception as e:
        print("Error processing data:", e)
        traceback.print_exc()
        
    finally:
        print("Waiting for the next data...")
        time.sleep(1)
        continue

