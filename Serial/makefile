CC = gcc
CFLAGS =    -std=c99
LDFLAGS = 
OBJFILES = part_1_serial.o timing.o
TARGET = part_1_serial.out
all: $(TARGET)
$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~
