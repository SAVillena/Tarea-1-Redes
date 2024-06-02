#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

const int filas = 6;
const int columnas = 7;

/**
 * @brief Clase que representa un tablero de juego.
 */
class Tablero
{
private:
    char tablero[filas][columnas]; /**< Matriz que representa el tablero de juego. */

public:
    /**
     * @brief Constructor de la clase Tablero.
     * Inicializa el tablero llamando a la función inicializar().
     */
    Tablero()
    {
        inicializar();
    }

    /**
     * @brief Inicializa el tablero con espacios en blanco.
     */
    void inicializar()
    {
        for (int i = 0; i < filas; i++)
        {
            for (int j = 0; j < columnas; j++)
            {
                tablero[i][j] = ' ';
            }
        }
    }

    /**
     * @brief Imprime el tablero en formato de cadena de caracteres y lo envía al cliente.
     * @param socket_cliente El socket del cliente al que se enviará el tablero.
     */
    void imprimir(int socket_cliente)
    {
        string tablero_str = "TABLERO\n";
        for (int i = 0; i < filas; i++)
        {
            tablero_str += to_string(filas - i) + "|";
            for (int j = 0; j < columnas; j++)
            {
                tablero_str += tablero[i][j];
                tablero_str += " ";
            }
            tablero_str += "\n";
        }
        tablero_str += "  -------------\n";
        tablero_str += "  1 2 3 4 5 6 7\n";
        send(socket_cliente, tablero_str.c_str(), tablero_str.size(), 0);
    }

    /**
     * @brief Comprueba si hay un ganador en el tablero.
     * @param ficha La ficha del jugador a comprobar.
     * @return true si hay un ganador, false en caso contrario.
     */
    bool comprobarGanador(char ficha)
    {
        for (int i = 0; i < filas; i++)
        {
            for (int j = 0; j <= columnas - 4; j++)
            {
                if (tablero[i][j] == ficha && tablero[i][j + 1] == ficha && tablero[i][j + 2] == ficha && tablero[i][j + 3] == ficha)
                {
                    return true;
                }
            }
        }
        for (int i = 0; i <= filas - 4; i++)
        {
            for (int j = 0; j < columnas; j++)
            {
                if (tablero[i][j] == ficha && tablero[i + 1][j] == ficha && tablero[i + 2][j] == ficha && tablero[i + 3][j] == ficha)
                {
                    return true;
                }
            }
        }
        for (int i = 0; i <= filas - 4; i++)
        {
            for (int j = 0; j <= columnas - 4; j++)
            {
                if (tablero[i][j] == ficha && tablero[i + 1][j + 1] == ficha && tablero[i + 2][j + 2] == ficha && tablero[i + 3][j + 3] == ficha)
                {
                    return true;
                }
            }
        }
        for (int i = 0; i <= filas - 4; i++)
        {
            for (int j = 3; j < columnas; j++)
            {
                if (tablero[i][j] == ficha && tablero[i + 1][j - 1] == ficha && tablero[i + 2][j - 2] == ficha && tablero[i + 3][j - 3] == ficha)
                {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * @brief Comprueba si el tablero está lleno.
     * @return true si el tablero está lleno, false en caso contrario.
     */
    bool tableroLleno()
    {
        for (int i = 0; i < filas; ++i)
        {
            for (int j = 0; j < columnas; ++j)
            {
                if (tablero[i][j] == ' ')
                {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * @brief Coloca una ficha en la columna especificada.
     * @param col La columna en la que se colocará la ficha.
     * @param ficha La ficha a colocar.
     * @return true si se pudo colocar la ficha, false si la columna está llena.
     */
    bool colocarFicha(int col, char ficha)
    {
        for (int i = filas - 1; i >= 0; i--)
        {
            if (tablero[i][col - 1] == ' ')
            {
                tablero[i][col - 1] = ficha;
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Comprueba si una columna está llena.
     * @param col La columna a comprobar.
     * @return true si la columna está llena, false en caso contrario.
     */
    bool columnaLlena(int col)
    {
        for (int i = 0; i < filas; ++i)
        {
            if (tablero[i][col - 1] == ' ')
            {
                return false;
            }
        }
        return true;
    }
};

class Juego
{
private:
    Tablero tablero;

public:
    /**
     * @brief Función que maneja el juego entre el servidor y el cliente.
     *
     * @param socket_cliente El socket del cliente.
     * @param direccionCliente La dirección del cliente.
     */
    void jugar(int socket_cliente, struct sockaddr_in direccionCliente)
    {
        srand(time(NULL));
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int n_bytes = 0;
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(direccionCliente.sin_addr), ip, INET_ADDRSTRLEN);
        cout << "Juego nuevo [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]" << endl;
        tablero.inicializar();
        char AlAzar = (rand() % 2 == 0) ? 'S' : 'C';
        string mensaje = "Ingrese un numero para iniciar el juego";
        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
        n_bytes = recv(socket_cliente, buffer, 1024, 0);
        tablero.imprimir(socket_cliente);

        if (AlAzar == 'C')
        {
            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: inicia juego el cliente" << endl;
        }
        else
        {
            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: inicia juego el servidor" << endl;
        }

        if (n_bytes <= 0)
        {
            cout << "Error en la conexion, Saliendo del juego." << endl;
        }
        else
        {
            while (true)
            {
                int columna;
                buffer[n_bytes] = '\0';
                if (AlAzar == 'C')
                {
                    mensaje = "Es su turno, ingrese columna. (1-7) ";
                    send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                    n_bytes = recv(socket_cliente, buffer, 1024, 0);
                    buffer[n_bytes] = '\0';
                    if (strncmp(buffer, "exit", 4) == 0)
                    {
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                        break;
                    }
                    columna = atoi(&buffer[0]);
                    while (tablero.columnaLlena(columna))
                    {
                        mensaje = "Columna llena, eliga otra: ";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        n_bytes = recv(socket_cliente, buffer, 1024, 0);
                        if (strncmp(buffer, "exit", 4) == 0)
                        {
                            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                            break;
                        }
                        buffer[n_bytes] = '\0';
                        columna = atoi(&buffer[0]);
                    }
                    while (columna < 1 || columna > 7)
                    {
                        mensaje = "Columna invalida, vuelva a ingresar: ";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        n_bytes = recv(socket_cliente, buffer, 1024, 0);
                        if (strncmp(buffer, "exit", 4) == 0)
                        {
                            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                            break;
                        }
                        buffer[n_bytes] = '\0';
                        columna = atoi(&buffer[0]);
                    }
                    tablero.colocarFicha(columna, 'C');
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Cliente juega columna " << columna << endl;
                    if (tablero.comprobarGanador('C'))
                    {
                        tablero.imprimir(socket_cliente);
                        mensaje = "Ganador Cliente, felicitaciones\nFin del Juego.\n";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana cliente." << endl;
                        break;
                    }
                    if (tablero.tableroLleno())
                    {
                        tablero.imprimir(socket_cliente);
                        mensaje = "Tablero lleno, es un empate.";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: empate." << endl;
                        break;
                    }
                    mensaje = "\nEs el turno del Servidor...\n";
                    send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                    colocarFichaServidor();
                    if (tablero.comprobarGanador('S'))
                    {
                        tablero.imprimir(socket_cliente);
                        mensaje = "Gana el Servidor...\nFin del juego.\n";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana Servidor." << endl;
                        break;
                    }
                    tablero.imprimir(socket_cliente);
                }
                else if (AlAzar == 'S')
                {
                    mensaje = "Juega el servidor\nEspere su turno\n";
                    send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                    colocarFichaServidor();
                    tablero.imprimir(socket_cliente);
                    mensaje = "Es su turno, ingrese columna. (1-7): ";
                    send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                    n_bytes = recv(socket_cliente, buffer, 1024, 0);
                    if (strncmp(buffer, "exit", 4) == 0)
                    {
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                        break;
                    }
                    buffer[n_bytes] = '\0';
                    columna = atoi(&buffer[0]);
                    while (tablero.columnaLlena(columna))
                    {
                        mensaje = "Columna llena, eliga otra: ";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        n_bytes = recv(socket_cliente, buffer, 1024, 0);
                        if (strncmp(buffer, "exit", 4) == 0)
                        {
                            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                            break;
                        }
                        buffer[n_bytes] = '\0';
                        columna = atoi(&buffer[0]);
                    }
                    while (columna < 1 || columna > 7)
                    {
                        mensaje = "Columna invalida, vuelva a ingresar: ";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        n_bytes = recv(socket_cliente, buffer, 1024, 0);
                        if (strncmp(buffer, "exit", 4) == 0)
                        {
                            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                            break;
                        }
                        buffer[n_bytes] = '\0';
                        columna = atoi(&buffer[0]);
                    }
                    tablero.colocarFicha(columna, 'C');
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Cliente juega columna " << columna << endl;
                    if (tablero.comprobarGanador('C'))
                    {
                        tablero.imprimir(socket_cliente);
                        mensaje = "Ganador Cliente, felicitaciones\nFin del Juego.\n";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana cliente." << endl;
                        break;
                    }
                    if (tablero.tableroLleno())
                    {
                        tablero.imprimir(socket_cliente);
                        mensaje = "Tablero lleno, es un empate.";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: empate." << endl;
                        break;
                    }
                    if (tablero.comprobarGanador('S'))
                    {
                        tablero.imprimir(socket_cliente);
                        mensaje = "Gana el Servidor...\nFin del juego.\n";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana Servidor." << endl;
                        break;
                    }
                    tablero.imprimir(socket_cliente);
                }
            }
        }
    }

private:
    /**
     * Coloca una ficha en el tablero del servidor de forma aleatoria.
     * Utiliza la función rand() para generar un número aleatorio que representa la columna en la que se colocará la ficha.
     * Si la ficha se coloca exitosamente, se marca como colocada y se sale del bucle.
     */
    void colocarFichaServidor()
    {
        srand(time(NULL));
        bool colocado = false;
        while (!colocado)
        {
            int col = rand() % columnas;
            if (tablero.colocarFicha(col + 1, 'S'))
            {
                colocado = true;
            }
        }
    }
};

/**
 * @brief Función envoltorio para el hilo de juego.
 *
 * Esta función se utiliza como envoltorio para el hilo de juego. Recibe un puntero a un par que contiene el socket del cliente y la dirección del cliente.
 * Luego, extrae los datos del par y crea un objeto de la clase Juego. A continuación, llama al método jugar() del objeto Juego para iniciar el juego con el cliente.
 * Después de finalizar el juego, se cierra el socket del cliente y se libera la memoria del par. Finalmente, el hilo se termina.
 *
 * @param arg Puntero al par que contiene el socket del cliente y la dirección del cliente.
 * @return void
 */
void *jugar_wrapper(void *arg)
{
    pair<int, struct sockaddr_in> *data = (pair<int, struct sockaddr_in> *)arg;
    int socket_cliente = data->first;
    struct sockaddr_in direccionCliente = data->second;
    Juego juego;
    juego.jugar(socket_cliente, direccionCliente);
    close(socket_cliente);
    delete data;
    pthread_exit(NULL);
}

/**
 * @brief Función principal del programa.
 *
 * @param argc Cantidad de argumentos pasados por línea de comandos.
 * @param argv Arreglo de cadenas que contienen los argumentos pasados por línea de comandos.
 * @return int Valor de retorno del programa.
 */
int main(int argc, char **argv)
{
    int port = atoi(argv[1]);
    int socket_server = 0;
    struct sockaddr_in direccionServidor, direccionCliente;

    if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        exit(EXIT_FAILURE);
    }

    memset(&direccionServidor, 0, sizeof(direccionServidor));
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = htonl(INADDR_ANY);
    direccionServidor.sin_port = htons(port);

    if (bind(socket_server, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) < 0)
    {
        cout << "Error calling bind()\n";
        exit(EXIT_FAILURE);
    }

    if (listen(socket_server, 1024) < 0)
    {
        cout << "Error calling listen()\n";
        exit(EXIT_FAILURE);
    }

    socklen_t addr_size;
    addr_size = sizeof(struct sockaddr_in);

    cout << "Esperando conexiones ...\n";
    while (true)
    {
        int socket_cliente;

        if ((socket_cliente = accept(socket_server, (struct sockaddr *)&direccionCliente, &addr_size)) < 0)
        {
            cout << "Error calling accept()\n";
            continue;
        }

        pthread_t thread;
        pair<int, struct sockaddr_in> *data = new pair<int, struct sockaddr_in>(socket_cliente, direccionCliente);
        if (pthread_create(&thread, NULL, jugar_wrapper, (void *)data) != 0)
        {
            cerr << "Error creating thread\n";
            delete data;
            close(socket_cliente);
        }
        else
        {
            pthread_detach(thread);
        }
    }

    return 0;
}
