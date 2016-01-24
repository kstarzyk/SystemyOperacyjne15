CC=gcc
CFLAGS= -c -Wreturn-type -Wundef
LIBS=  -pthread
SOURCES=pracownia.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=pracownia

all: $(SOURCES) $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o  $@ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $< -o  $@



