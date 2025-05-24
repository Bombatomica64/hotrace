CC = cc
CFLAGS = -Wall -Wextra -Werror -O3
LDFLAGS =
SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = hotrace

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean