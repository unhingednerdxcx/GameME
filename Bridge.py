import socket
import traceback
import time
HOST = "0.0.0.0"
PORT = 3658
while True:
    try:
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server.bind((HOST, PORT))
        server.listen(1)
        print(f"LISTENING Server is listening on {HOST}:{PORT}")
        break  
    except Exception as e:
        print(f"ERROR Could not start server on {HOST}:{PORT} — {e}")
        traceback.print_exc()
        print("RETRY Trying again in 3 seconds...")
        time.sleep(3)

print("LISTENING Server is listening on port", PORT)
flag = False

def comm():
    global flag
    parse_arr = []
    while not flag:
        sock, addr = server.accept()
        print("CONNECTED Client connected from", addr)
        sock.settimeout(3.5)
        try:
            while True:
                data = b""
                try:
                    data = sock.recv(1024).decode().strip()
                    if not data:
                        print("DISCONNECTED Client disconnected")
                        break
                except socket.timeout:
                    print("TIMEOUT No data received, waiting for next message")
                    continue
                parts = data.split(" : ")
                if len(parts) >= 9:
                    parse_arr = parts[:9]
                    flag = True
                else:
                    print("ERROR Received malformed data:", data)
                    continue

        except Exception as e:
            print("ERROR Failed to receive data:", e)
            traceback.print_exc()
        finally:
            sock.close()
            print("CLOSED Connection closed with", addr)
        return parse_arr

