# Cliente TCP en C++

## Descripción
Este programa es un cliente TCP simple escrito en C++ que se conecta a un servidor especificado por IP y puerto. El programa envía y recibe mensajes del servidor.

## Requisitos
Para compilar y ejecutar este programa necesitas un sistema Unix/Linux con el compilador `g++` instalado.

## Compilación
Para compilar el programa, clona este repositorio y en el directorio raíz ejecuta:

```bash
    make
```

## Uso
Para ejecutar el programa:
```bash
    ./cliente <IP> <puerto>
```

Reemplaza `<IP>` y `<puerto>` con la dirección IP y el puerto del servidor al que deseas conectarte.

## Funciones
El programa permite interactuar con un servidor TCP enviando mensajes desde la entrada estándar. Usa `exit` para cerrar la conexión.
