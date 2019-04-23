CC = clang++
TARGET = main
CFLAGS = -g -ggdb -O3
LDFLAGS =  $(CFLAGS)
OBJECTS = \
	$(TARGET).o \
	graph_utils.o

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
