CC = gcc

# compiler flags
CFLAGS = -lcurl

TARGET = etherprise

all: main.c
	$(CC) main.c daemon.c -o $(TARGET) $(CFLAGS)
clean:
	$(RM) $(TARGET)