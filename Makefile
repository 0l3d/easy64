CC = gcc
TARGET = easy
SOURCES = ./src/interpreter.c ./src/assembler.c ./src/cpu.c
OBJECTS = $(SOURCES:.c=.o)

BASE_CFLAGS = -g -O2 -march=native
LDFLAGS =

NOR_FLAGS = $(BASE_CFLAGS)

SEC_FLAGS = \
  -g -O2 -march=native \
  -Wall -Wextra -Wpedantic \
  -fstack-protector-strong -D_FORTIFY_SOURCE=2 \
  -Wformat=2 -Wformat-security -Wstringop-overflow -Wnull-dereference \
  -fanalyzer -Werror -Werror=unused-result \
  -fsanitize=address,undefined -fno-omit-frame-pointer

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

minimize:
	strip $(TARGET)

clean:
	rm -rf $(OBJECTS) $(TARGET) a.out

nor: CFLAGS = $(NOR_FLAGS)
nor: $(TARGET)

sec: CFLAGS = $(SEC_FLAGS)
sec: $(TARGET)
