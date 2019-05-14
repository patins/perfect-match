CC = clang++
TARGET = lock_free
CFLAGS = -g -O3 -std=c++11
#-L/usr/local/Cellar/gperftools/2.7/lib/
#CFLAGS = -O3
LDFLAGS =  $(CFLAGS)
OBJECTS = \
	$(TARGET).o \
	../utils/mtxReader.o

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
