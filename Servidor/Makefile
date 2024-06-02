# Definir el compilador
CC=g++

# Opciones de compilación
CFLAGS=-std=c++11 -pthread

# Nombre del ejecutable
TARGET=server

# Dependencias
all: $(TARGET)

$(TARGET): servidor.cpp
	$(CC) $(CFLAGS) servidor.cpp -o $(TARGET)

# Comando para limpiar archivos compilados
clean:
	rm -f $(TARGET)
