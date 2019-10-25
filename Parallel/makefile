CC = gcc
CFLAGS =   -std=c99
LDFLAGS = -lpthread
OBJFILES = part_1_parallel.o timing.o
TARGET = part_1_parallel.out
all: $(TARGET)
$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~
