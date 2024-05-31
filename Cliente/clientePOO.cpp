#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

class Cliente {
private:
    int socket_fd;
    struct sockaddr_in server;
    const int buffer_size = 1024;

public:
    Cliente(const string &ip, int puerto) {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == -1) {
            throw runtime_error("No se pudo crear el socket.");
        }

        server.sin_addr.s_addr = inet_addr(ip.c_str());
        server.sin_family = AF_INET;
        server.sin_port = htons(puerto);

        if (connect(socket_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
            throw runtime_error("Conexión fallida.");
        }

        cout << "Conectado al servidor en " << ip << ":" << puerto << "." << endl;
    }

    ~Cliente() {
        close(socket_fd);
        cout << "Conexión cerrada." << endl;
    }

    void interactuarConServidor() {
        char buffer[buffer_size];
        fd_set readfds;
        struct timeval tv;

        while (true) {
            FD_ZERO(&readfds);
            FD_SET(socket_fd, &readfds);
            FD_SET(STDIN_FILENO, &readfds);

            tv.tv_sec = 5;
            tv.tv_usec = 0;

            int activity = select(socket_fd + 1, &readfds, NULL, NULL, &tv);

            if (activity < 0) {
                cout << "Error en la selección." << endl;
                break;
            }

            if (FD_ISSET(socket_fd, &readfds)) {
                memset(buffer, 0, buffer_size);
                int len = recv(socket_fd, buffer, buffer_size - 1, 0);
                if (len > 0) {
                    cout << buffer << endl;
                } else {
                    cout << "Conexión cerrada por el servidor." << endl;
                    break;
                }
            }

            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                cout << " ";
                string input;
                getline(cin, input);

                if (!input.empty()) {
                    send(socket_fd, input.c_str(), input.length(), 0);
                    if (input == "exit") {
                        cout << "Cerrando la conexión..." << endl;
                        break;
                    }
                }
            }
        }
    }
};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <IP> <puerto>" << endl;
        return 1;
    }

    try {
        Cliente cliente(argv[1], atoi(argv[2]));
        cliente.interactuarConServidor();
    } catch (const exception &e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
