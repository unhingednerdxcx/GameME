from Bridge import comm # in python, you can import functions from other files using the import statement
import json # importing the json module to handle JSON data
import traceback # importing traceback to handle exceptions and print stack traces
import time # importing time to add delays in the script
import os # importing os to handle file paths

"""
	IN THE JSON FILE in the MainGame folder "x and y this may be changed to 1024, depends on your ADC DELETE THIS LINE",
"""

script_folder = os.path.dirname(os.path.abspath(__file__)) # collecting where this folder is stored
json_path = os.path.join(script_folder, "MainGame", "Others", "data.json") # adding the path to the json file

while True: # forever loop to keep the script running
    data = comm() # calling the comm function to get data from the Bridge (check bridge.py for details)
    print("Parsed Data:", data) # print the data received from the Bridge
    if data: # check if data is not empty
        print("Data received successfully.") # print confirmation of data reception
    else: # if data is empty or None
        print("No data received or an error occurred.") # print an error message
        print(data) # print the data for debugging
    try: # attempt to process the received data excption handling
        a = data[0] # condition of button a (1 = pressed, 0 = not pressed)
        b = data[1] # condition of button b (1 = pressed, 0 = not pressed)
        c = data[2] # condition of button c (1 = pressed, 0 = not pressed)
        d = data[3] # condition of button d (1 = pressed, 0 = not pressed)
        e = data[4] # condition of button e (1 = pressed, 0 = not pressed)
        f = data[5] # condition of button f (1 = pressed, 0 = not pressed)
        x = data[6] # x coordinate of the joystick (0-4095)
        y = data[7] # y coordinate of the joystick (0-4095)
        end = data[8] # end condition
        chunk = { # basically storing the data in a variable to dump it into a json file
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
        with open(json_path, "w") as file: # open the json file in write mode
            json.dump(chunk, file, indent=4) # dump the data into the json file with indentation for readability
        print("Data saved to data.json")

    except Exception as e: # handle any exceptions that occur during data processing
        print("Error processing data:", e) # print the error message
        traceback.print_exc() # print the traceback for debugging
        
    finally: # this block will always execute, regardless of whether an exception occurred or not
        print("Waiting for the next data...")
        time.sleep(1) # wait for 1 second before the next iteration to avoid overwhelming the system
        continue # continue the loop to wait for the next data

