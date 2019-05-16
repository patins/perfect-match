CC = clang++
TARGET = lock_free
CFLAGS = -g -O3 -std=c++14 -march=native -m64 -Xpreprocessor -fopenmp
LDFLAGS =  $(CFLAGS) -ltbb -lomp
OBJECTS = \
	$(TARGET).o \
	mtxReader.o

all: $(TARGET) message


$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

message:
	echo "Executable: $(TARGET) has been created"

-include $(OBJS:.o=.d)
