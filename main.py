import socket # This library is used for network communication
import select # This is used to handle multiple connections
import time # This library is used for time-related functions
import json # This library is used for handling JSON data
import os # This library is used for file and directory operations
import traceback # This library is used for printing stack traces and debugging
from typing import List, Union, Optional # This is for readability and type hinting

class Server:

    def __init__(self, host: str = "0.0.0.0", port: int = 3658): # On startup, we set the host and port for the server
        # assigning variables for host and port
        self.host = host
        self.port = port
        self.server = None # this is just a placeholder for the server socket
        self.current_client: Optional[socket.socket] = None # this will hold the current client connection
        self.client_address: Optional[tuple] = None # this will hold the address of the current client (IP, port)
        self.script_folder = os.path.dirname(os.path.abspath(__file__)) # this gets the directory of the current script
        self.json_path = os.path.join(self.script_folder, "MainGame", "Others", "data.json") # this sets the path for the JSON file where data will be saved
        self.last_data_time = 0 # this will be used to track the last time data was received from the client
        self.connection_timeout = 5.0 # this is the timeout for the connection, after which it will be considered lost


    def initialize_server(self) -> socket.socket: # This function initializes the server socket
        server = None # This is a placeholder for the server socket

        while True: # This loop will keep trying to start the server until it succeeds

            try: # Attempt to create a socket and bind it to the host and port handeling exceptions
                server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # Create a TCP socket
                server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) # Allow the socket to be reused immediately after closing
                
                # Enable keepalive options
                server.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1) # Enable keepalive
                server.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPIDLE, 30) # Start sending keepalive probes after 30 seconds of inactivity
                server.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPINTVL, 5) # Interval between keepalive probes
                server.setsockopt(socket.IPPROTO_TCP, socket.TCP_KEEPCNT, 5) # Number of keepalive probes before considering the connection dead
                
                server.bind((self.host, self.port)) # Bind the socket to the host and port
                server.listen(1) # Listen for incoming connections
                server.setblocking(False)  # Non-blocking mode for accept
                print(f"INFO| Server listening on {self.host}:{self.port}")
                return server # Return the initialized server socket
            
            except Exception as e: # If an error occurs, print the error and retry
                print(f"ERROR Failed to start server: {e}")
                traceback.print_exc()
                if server:
                    server.close() # Close the socket if it was created
                time.sleep(3) # Wait before retrying to start the server


    def maintain_connection(self) -> bool: # This function checks if the current client connection is still alive or if a new client has connected
        
        if self.current_client: # If there is an existing client connection
            try: # attempt to send a zero-byte message to check if the connection is still alive handeling exceptions
                # Check if connection is still alive
                self.current_client.send(b"")  # Zero-byte send to test connection
                return True # If send is successful, connection is alive and WE WILL LEAVE the function
            
            except (ConnectionResetError, BrokenPipeError, OSError): # If an error occurs, the connection is lost
                print("WARN| Client connection lost, cleaning up...")
                self.current_client.close() # Close the socket
                self.current_client = None # Reset the current client
                self.client_address = None # Reset the client address
                return False
        
        try: # If there is no current client, check for new connections
            ready = select.select([self.server], [], [], 0.1)  # Non-blocking check

            if ready[0]: # If the server socket is ready to accept a new connection
                sock, addr = self.server.accept() # Accept the new connection
                print(f"INFO| New client connected from {addr}")
                
                # Configure client socket
                sock.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1) # Enable keepalive
                sock.settimeout(0.1)  # Short timeout for recv operations
                sock.setblocking(False)  # Non-blocking mode
                # redeclare the new current client and address
                self.current_client = sock
                self.client_address = addr
                self.last_data_time = time.time()
                return True
            
        except Exception as e: # If an error occurs while accepting a new connection
            traceback.print_exc() # Print the stack trace for debugging
            print(f"ERROR Error accepting connection: {e}")
        
        return False
    

    def data_incoming(self): # This function handles incoming data from the current client
        if not self.current_client: # If there is no current client connection, return
            return # Leave the function
        
        try: # Attempt to receive data from the current client
            ready = select.select([self.current_client], [], [], 0.1) # Non-blocking check for incoming data
            if not ready[0]: # If no data is ready to be received
                if time.time() - self.last_data_time > self.connection_timeout: # Check if connection has timed out
                    print("INFO| Connection timeout, closing...")
                    self.current_client.close() # close the current client connection
                    self.current_client = None # Reset the current client
                return # Leave the function
                
            data = self.current_client.recv(1024).decode().strip() # if we get data, decode it and strip any whitespace
            if not data: # If no data is received, the client has disconnected
                print("INFO| Client disconnected")
                self.current_client.close() # Close the current client connection
                self.current_client = None # Reset the current client
                return # Leave the function

                
            print(f"INFO| Received: {data}")
            processed = self.process_data(data) # Process the received data
            self.last_data_time = time.time() # Update the last data time
            
            # Send acknowledgment
            try:
                self.current_client.sendall(b"M")
            except Exception as e: # If an error occurs while sending acknowledgment
                print(f"WARN| Failed to send ACK: {e}")
                traceback.print_exc() # Print the stack trace for debugging
            self.save_to_json(processed) # Save the processed data to a JSON file
            print(f"INFO| Saved data: {processed}")

        except Exception as e:
            print(f"ERROR Communication error: {e}")
            traceback.print_exc()
            if self.current_client: # If there is a current client connection
                self.current_client.close() # Close the current client connection
                self.current_client = None # Reset the current client


    def process_data(self, data: str) -> List[Union[int, str]]: # This function processes the incoming data string
        parts = data.strip().split(" : ") # divide the data string into parts using " : " as the delimiter

        if len(parts) < 9: # If the number of parts is less than 9, print an error message and return an empty list
            print("ERROR Invalid data format expected 9 packets got:", len(parts))
        processed = [] # This is a placeholder for the processed data

        for item in parts[:9]: # Iterate through the first 9 parts of the data

            try: # Attempt to convert the item to an integer and store it in the processed list
                processed.append(int(item)) # Convert to integer and store in processed
            except ValueError: # If conversion fails, treat it as a string
                processed.append(item.strip()) # Strip whitespace and store as string
        return processed # Returns the processed data 
    

    def save_to_json(self, data: List[Union[int, str]]) -> None: # to save the processed data to a JSON file

        chunk = { # Create a dictionary with the processed data
            "a": data[0], "b": data[1], "c": data[2], "d": data[3],
            "e": data[4], "f": data[5], "x": data[6], "y": data[7],
            "end": data[8]
        }
        
        os.makedirs(os.path.dirname(self.json_path), exist_ok=True) # Ensure the directory exists
        with open(self.json_path, "w") as file: # Open the JSON file for writing
            json.dump(chunk, file, indent=4) # Write the processed data to the JSON file with indentation for readability
    

    def run(self): # This function runs the server
        self.server = self.initialize_server() # Initialize the server socket

        try: # attempt to run the server loop
            while True: # Keep the server running
                self.maintain_connection() # Check if the current client connection is alive or if a new client has connected
                if self.current_client: # If there is a current client connection
                    self.data_incoming() # Handle incoming data from the current client
                time.sleep(0.1) # Sleep for a short time to avoid busy-waiting

        except KeyboardInterrupt: # If the server is interrupted by the user via (Ctrl+C)
            print("\nINFO| Server shutting down...")
        except Exception as e:
            print(f"WARN| Critical server error: {e}")
            traceback.print_exc()

        finally:
            # Cleanup
            print("INFO| Cleaning up resources...")
            if self.current_client: # If there is a current client connection
                self.current_client.close() # Close the current client connection
            if self.server: # If the server socket exists
                self.server.close() # Close the server socket
            print("END| Server stopped")


if __name__ == "__main__": # This is the entry point of the script
    print("INFO| Starting Server...")
    server = Server() # Creates an instance called server for the Server class
    print("INFO| Server initialized, running...")
    server.run() # Runs the server