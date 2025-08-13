CC=gcc
CFLAGS= -g -O2 -march=native
LDFLAGS=
TARGET=easy
SOURCES=./src/interpreter.c ./src/assembler.c ./src/cpu.c
OBJECTS=$(SOURCES:.c=.o)

all: $(TARGET) minimize

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)


%.o: %.c
	$(CC) -o $(CFLAGS) -c $< -o $@

minimize:
	strip $(TARGET)

clean:
	rm -rf $(OBJECTS) $(TARGET) a.out
