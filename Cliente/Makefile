# Makefile para el proyecto cliente

# Compilador
CC=g++

# Opciones de compilación
CFLAGS=-Wall

# Nombre del archivo ejecutable
TARGET=cliente

# Archivo fuente
SOURCE=clientePOO.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)

.PHONY: all clean
