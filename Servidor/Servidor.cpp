#include <sys/socket.h> // socket()
#include <arpa/inet.h>  // hton*()
#include <string.h>     // memset()
#include <unistd.h>
#include <iostream>
#include <pthread.h> //hebrass
#include <cstdlib>   // Para rand() y srand()
#include <ctime>     // Para time()
using namespace std;
//
const int filas = 6;
const int columnas = 7;
char tablero[filas][columnas];
//
/**
 * @brief Inicializa el tablero del juego.
 * 
 * Esta función recorre todas las celdas del tablero y las inicializa con un espacio en blanco.
 * 
 * @param None
 * @return None
 */
void inicializarTablero()
{
    for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < columnas; j++)
        {
            tablero[i][j] = ' ';
        }
    }
}
//

/**
 * Comprueba si hay un ganador en el juego de conecta 4.
 * 
 * @param ficha La ficha del jugador a comprobar.
 * @return true si hay un ganador, false en caso contrario.
 */
bool comprobarGanador(char ficha)
{
    // Comprobar horizontal
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
    // Comprobar vertical
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
    // Comprobar diagonal
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
    // Comprobar diagonal inversa
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

//
/**
 * Coloca una ficha del servidor en el tablero de juego.
 * La función genera un número aleatorio para seleccionar una columna y coloca la ficha en la fila más baja disponible en esa columna.
 * Si no hay espacio disponible en la columna seleccionada, la función no coloca la ficha y continúa generando números aleatorios hasta encontrar una columna válida.
 */
void colocarFichaServidor()
{
    srand(time(NULL)); // Inicializar la semilla para números aleatorios
    bool colocado = false;
    while (!colocado)
    {
        int col = rand() % columnas; // Genera un número aleatorio para la columna
        for (int i = filas - 1; i >= 0; i--)
        {
            if (tablero[i][col] == ' ')
            {
                tablero[i][col] = 'S';
                colocado = true;
                break;
            }
        }
    }
}

/**
 * Imprime el tablero en el cliente especificado.
 *
 * @param socket_cliente El socket del cliente al que se enviará el tablero.
 */
void imprimirTablero(int socket_cliente)
{
    // Crear un buffer grande para contener todo el tablero y los mensajes
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

    // Enviar el tablero al cliente
    send(socket_cliente, tablero_str.c_str(), tablero_str.size(), 0);
}

/**
 * Verifica si el tablero está lleno.
 * 
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
//
/**
 * Coloca una ficha en la columna especificada.
 * 
 * @param col La columna en la que se desea colocar la ficha.
 * @return true si se pudo colocar la ficha, false en caso contrario.
 */
bool colocarFicha(int col)
{
    for (int i = filas - 1; i >= 0; i--)
    {
        if (tablero[i][col - 1] == ' ')
        {
            tablero[i][col - 1] = 'C';
            return true;
        }
    }
    return false;
}


/**
 * Verifica si una columna está llena en el tablero.
 * 
 * @param col El número de columna a verificar.
 * @return true si la columna está llena, false de lo contrario.
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

/**
 * Función que representa el juego de conecta 4 entre el servidor y el cliente.
 * 
 * @param socket_client El socket del cliente.
 * @param direccionCliente La dirección del cliente.
 */
void jugar(int socket_client, struct sockaddr_in direccionCliente)
{
    srand(time(NULL)); // Inicializar la semilla para números aleatorios
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int n_bytes = 0;
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(direccionCliente.sin_addr), ip, INET_ADDRSTRLEN);
    cout << "Juego nuevo [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]" << endl;
    inicializarTablero();
    char AlAzar = (rand() % 2 == 0) ? 'S' : 'C';
    string mensaje = "Ingrese un numero para iniciar el juego";
    send(socket_client, mensaje.c_str(), mensaje.size(), 0);
    n_bytes = recv(socket_client, buffer, 1024, 0);
    imprimirTablero(socket_client);
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
        cout << "Error en la conexion, Saliendo dl juego." << endl;
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
                send(socket_client, mensaje.c_str(), mensaje.size(), 0);
                n_bytes = recv(socket_client, buffer, 1024, 0);
                buffer[n_bytes] = '\0';
                if (strncmp(buffer, "exit", 4) == 0)
                {
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                    break;
                }
                columna = atoi(&buffer[0]);
                while (columnaLlena(columna))
                {
                    mensaje = "Columna llena, eliga otra: ";
                    send(socket_client, mensaje.c_str(), mensaje.size(), 0);
                    n_bytes = recv(socket_client, buffer, 1024, 0);
                    if (strncmp(buffer, "exit", 4) == 0)
                    {
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                        break;
                    }
                    buffer[n_bytes] = '\0';
                    columna = atoi(&buffer[0]); 
                }
                while (columna < 0 || columna > 7)
                {
                    mensaje = "Columna invalida, vuelva a ingresar: ";
                    send(socket_client, mensaje.c_str(), mensaje.size(), 0);
                    n_bytes = recv(socket_client, buffer, 1024, 0);
                    if (strncmp(buffer, "exit", 4) == 0)
                    {
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                        break;
                    }
                    buffer[n_bytes] = '\0';
                    columna = atoi(&buffer[0]); 
                }
                colocarFicha(columna);
                cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Cliente juega columna " << columna << endl;
                if (comprobarGanador('C'))
                {
                    imprimirTablero(socket_client);
                    mensaje = "Ganador Cliente, felicitaciones\nFin del Juego.\n";
                    send(socket_client, mensaje.c_str(), mensaje.size(), 0);
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana cliente." << columna << endl;
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: fin del juego." << columna << endl;
                    break;
                }
                if (tableroLleno())
                {
                    imprimirTablero(socket_client);
                    string mensaje = "Tablero lleno, es un empate.";
                    send(socket_client, mensaje.c_str(), mensaje.size(), 0);
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: empate." << endl;
                    break;
                }
                mensaje = "\nEs el turno del Servidor...\n";
                send(socket_client, mensaje.c_str(), mensaje.size(), 0);
                colocarFichaServidor();
                if (comprobarGanador('S'))
                {
                    imprimirTablero(socket_client);
                    mensaje = "Gana el Servidor...\nFin del juego.\n";
                    send(socket_client, mensaje.c_str(), mensaje.size(), 0);
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana Servidor." << columna << endl;
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: fin del juego." << columna << endl;
                    break;
                }
                imprimirTablero(socket_client);
            }
            else if (AlAzar == 'S')
            {
                mensaje = "Juega el servidor\nEspere su turno\n";
                send(socket_client, mensaje.c_str(), mensaje.size(), 0);
                colocarFichaServidor();
                imprimirTablero(socket_client);
                mensaje = "Es su turno, ingrese columna. (1-7): ";
                send(socket_client, mensaje.c_str(), mensaje.size(), 0);
                n_bytes = recv(socket_client, buffer, 1024, 0);
                if (strncmp(buffer, "exit", 4) == 0)
                {
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                    break;
                }
                buffer[n_bytes] = '\0';
                columna = atoi(&buffer[0]);
                while (columnaLlena(columna))
                {
                    mensaje = "Columna llena, eliga otra: ";
                    send(socket_client, mensaje.c_str(), mensaje.size(), 0);
                    n_bytes = recv(socket_client, buffer, 1024, 0);
                    if (strncmp(buffer, "exit", 4) == 0)
                    {
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                        break;
                    }
                    buffer[n_bytes] = '\0';
                    columna = atoi(&buffer[0]); // Convertir de nuevo a entero // habia un -1 lo quite por que no devolvia bien el valor
                }
                while (columna < 1 || columna > 7)
                {
                    mensaje = "Columna invalida, vuelva a ingresar: ";
                    send(socket_client, mensaje.c_str(), mensaje.size(), 0);
                    n_bytes = recv(socket_client, buffer, 1024, 0);
                    if (strncmp(buffer, "exit", 4) == 0)
                    {
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                        break;
                    }
                    buffer[n_bytes] = '\0';
                    columna = atoi(&buffer[0]); // Convertir de nuevo a entero // habia un -1 lo quite por que no devolvia bien el valor
                }
                colocarFicha(columna);
                cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Cliente juega columna " << columna << endl;
                if (comprobarGanador('C'))
                {
                    imprimirTablero(socket_client);
                    mensaje = "Ganador Cliente, felicitaciones\nFin del Juego.\n";
                    send(socket_client, mensaje.c_str(), mensaje.size(), 0);
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana cliente." << columna << endl;
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: fin del juego." << columna << endl;
                    break;
                }
                if (tableroLleno())
                {
                    imprimirTablero(socket_client);
                    string mensaje = "Tablero lleno, es un empate.";
                    send(socket_client, mensaje.c_str(), mensaje.size(), 0);
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: empate." << endl;
                    break;
                }
                if (comprobarGanador('S'))
                {
                    imprimirTablero(socket_client);
                    mensaje = "Gana el Servidor...\nFin del juego.\n";
                    send(socket_client, mensaje.c_str(), mensaje.size(), 0);
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana Servidor." << columna << endl;
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: fin del juego." << columna << endl;
                    break;
                }
                imprimirTablero(socket_client);
            }
        }
    }
}


/**
 * @brief Función envoltorio para la función de juego.
 * 
 * Esta función se utiliza como envoltorio para la función de juego. Recibe un puntero a un par que contiene el socket del cliente y la dirección del cliente.
 * Llama a la función de juego pasando el socket del cliente y la dirección del cliente como argumentos. Luego cierra el socket del cliente y libera la memoria del par.
 * Finalmente, se sale del hilo.
 * 
 * @param arg Puntero al par que contiene el socket del cliente y la dirección del cliente.
 */
void *jugar_wrapper(void *arg)
{

    pair<int, struct sockaddr_in> *data = (pair<int, struct sockaddr_in> *)arg;
    int socket_cliente = data->first;
    struct sockaddr_in direccionCliente = data->second;
    jugar(socket_cliente, direccionCliente);
    close(socket_cliente);
    delete data;
    pthread_exit(NULL);
}

//
int main(int argc, char **argv)
{
    //
    int port = atoi(argv[1]);
    int socket_server = 0;
    // socket address structures.
    struct sockaddr_in direccionServidor, direccionCliente;

    // crea el socket.
    /*
     * domain:
     *      AF_ LOCAL-> processes on the same host.
     *      AF_INET -> processes on different hosts connected by IP (AF_INET->IPv4, AF_INET6->IPv6)
     * type:
     *      SOCK_STREAM: TCP (reliable, connection-oriented)
     *      SOCK_DGRAM: UDP (unreliable, connectionless)
     * protocol:
     *      Protocol value for Internet Protocol(IP), which is 0.
     */
    //cout << "Creating listening socket ...\n";
    if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        //cout << "Error creating listening socket\n";
        exit(EXIT_FAILURE);
    }

    // configuracion de los atributos de la clase sockaddr_in.
    //cout << "Configuring socket address structure ...\n";
    memset(&direccionServidor, 0, sizeof(direccionServidor));
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = htonl(INADDR_ANY);
    direccionServidor.sin_port = htons(port);

    //
    //cout << "Binding socket ...\n";
    if (bind(socket_server, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) < 0)
    {
        cout << "Error calling bind()\n";
        exit(EXIT_FAILURE);
    }

    //
    //cout << "Calling listening ...\n";
    if (listen(socket_server, 1024) < 0)
    {
        cout << "Error calling listen()\n";
        exit(EXIT_FAILURE);
    }

    // para obtener info del cliente.
    socklen_t addr_size;
    addr_size = sizeof(struct sockaddr_in);

    //
    cout << "Esperando conexiones ...\n";
    while (true)
    {
        /*  Wait for a connection, then accept() it  */
        int socket_cliente;

        //
        if ((socket_cliente = accept(socket_server, (struct sockaddr *)&direccionCliente, &addr_size)) < 0)
        {
            cout << "Error calling accept()\n";
            // exit(EXIT_FAILURE);
            continue;
        }

        //
        // jugar(socket_cliente, direccionCliente);

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
            pthread_detach(thread); // Se evitan fugas de memoria, por lo que se desconecta el hilo.
        }
    }

    //
    return 0;
}