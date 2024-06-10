import socket


def main():
    socket_path = "/tmp/command_socket"

    with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as client_socket:
        try:
            client_socket.connect(socket_path)
            print("Connected to the server.")
        except socket.error as e:
            print(f"Could not connect to the server: {e}")
            return

        while True:
            command = input("Enter command: ")

            client_socket.sendall((command + "\n").encode('utf-8'))
            response = client_socket.recv(1024)
            if not response:
                print("Disconnected from server")
                break
            print("Received:", response.decode('utf-8'))

            if command.lower() == "exit":
                break

if __name__ == "__main__":
    main()
