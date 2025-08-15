
def comm():
    import socket # importing the socket module to handle network connections
    import traceback # importing traceback to handle exceptions and print stack traces
    import time # importing time to add delays in the script
    HOST = "0.0.0.0" # binding to all available interfaces
    PORT = 3658 # port to listen on
    while True: # loop to keep trying to start the server until successful
        try: # attempt to create a socket and bind it to the host and port, handling any exceptions that may occur
            server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # create a TCP socket
            server.bind((HOST, PORT)) # bind the socket to the host and port
            server.listen(1) # listen for incoming connections
            print(f"LISTENING Server is listening on {HOST}:{PORT}") # print confirmation that the server is listening
            break  # exit the loop if the server starts successfully
        except Exception as e: # handle any errors that occur during server setup
            print(f"ERROR Could not start server on {HOST}:{PORT} — {e}") # print the error message
            traceback.print_exc() # print the traceback for debugging
            print("RETRY Trying again in 3 seconds...") 
            time.sleep(3) # wait for 3 seconds before retrying to start the server
            continue # continue the loop to retry starting the server

    print("LISTENING Server is listening on port", PORT) # prints which port the server is listening on
    flag = False # flag to indicate if data has been received
    parse_arr = [] # array to store the parsed data

    while not flag: # loop until data is successfully received
        sock, addr = server.accept() # accept an incoming connection
        print("CONNECTED Client connected from", addr) # print the address of the connected client
        sock.settimeout(3.5) # set a timeout for receiving data to avoid blocking with no data
        try: # attempt to receive data from the client with exception handling
            while True: # loop to keep receiving data until the timeout occurs or data is received
                data = b"" # initialize an empty byte string to store received data
                try: # attempt to receive data from the socket with a timeout
                    data = sock.recv(1024).decode().strip() # receive data from the socket, decode it to a string, and strip any whitespace
                    print("RECEIVED Data:", data) # print the received data for debugging
                    if not data: # check if the received data is empty
                        print("DISCONNECTED Client disconnected") # print a message indicating the client has disconnected
                        break # exit the loop if no data is received
                except socket.timeout: # handle the case where the receive operation times out
                    print("TIMEOUT No data received, waiting for next message") # print a timeout message
                    continue # continue to wait for the next message
                parts = data.split(" : ") # split the received data into parts using " : " as the delimiter
                if len(parts) >= 9: # check if the received data has at least 9 parts
                    parse_arr = parts[:9] # take the first 9 parts of the data
                    flag = True # set the flag to True to indicate that data has been successfully received
                else: # if the received data does not have enough parts
                    print("ERROR Received malformed data:", data) # print an error message indicating malformed data for debugging
                    time.sleep(1) # wait for 1 second before trying to receive data again
                    continue # continue to wait for the next message

        except Exception as e: # handle any exceptions that occur during data reception
            print("ERROR Failed to receive data:", e) # print the error message
            traceback.print_exc() # print the traceback for debugging
        finally: # this block will always execute, regardless of whether an exception occurred or not
            sock.close() # close the socket connection
            print("CLOSED Connection closed with", addr) # print a message indicating the connection has been closed
        return parse_arr # return the parsed data array to the caller

