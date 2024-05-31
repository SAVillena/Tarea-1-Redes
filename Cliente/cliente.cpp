#include <iostream>
#include <cstring>      // Para memset()
#include <sys/socket.h> // Para socket(), connect()
#include <arpa/inet.h>  // Para sockaddr_in, inet_addr()
#include <unistd.h>     // Para close()

using namespace std;

/**
 * @brief Función que permite interactuar con el servidor a través de un socket.
 *
 * @param socket_fd El descriptor de archivo del socket.
 */
void interactuarConServidor(int socket_fd)
{
    const int buffer_size = 1024;
    char buffer[buffer_size]; // Corrección aquí: Debe ser buffer_size, no buffer32
    fd_set readfds;
    struct timeval tv;

    while (true)
    {
        FD_ZERO(&readfds);
        FD_SET(socket_fd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        // Configuración de timeout opcional
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        // Esperar entrada del socket o entrada estándar
        int activity = select(socket_fd + 1, &readfds, NULL, NULL, &tv);

        if (activity < 0)
        {
            cout << "Error en la selección." << endl;
            break;
        }

        // Verificar si hay algo que leer en el socket
        if (FD_ISSET(socket_fd, &readfds))
        {
            memset(buffer, 0, buffer_size);
            int len = recv(socket_fd, buffer, buffer_size - 1, 0);
            if (len > 0)
            {
                cout << "" << buffer << endl;
            }
            else
            {
                cout << "Conexión cerrada por el servidor." << endl;
                break;
            }
        }

        // Verificar si hay algo que leer en la entrada estándar
        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            cout << " ";
            string input;
            getline(cin, input);

            if (!input.empty())
            {
                send(socket_fd, input.c_str(), input.length(), 0);
                if (input == "exit")
                {
                    cout << "Cerrando la conexión..." << endl;
                    break; // Salir del bucle mientras
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Uso: " << argv[0] << " <IP> <puerto>" << endl;
        return 1;
    }

    int socket_fd;
    struct sockaddr_in server;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        cout << "No se pudo crear el socket." << endl;
        return 1;
    }

    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));

    if (connect(socket_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        cout << "Conexión fallida." << endl;
        return 1;
    }

    cout << "Conectado al servidor en " << argv[1] << ":" << argv[2] << "." << endl;

    // Iniciar la interacción con el servidor
    interactuarConServidor(socket_fd);

    // Cerrar el socket antes de finalizar
    close(socket_fd);
    cout << "Conexión cerrada." << endl;
    return 0;
}
