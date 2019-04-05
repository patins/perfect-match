CC = clang++
TARGET = bMatching
CFLAGS = -Xpreprocessor -fopenmp -lomp -O3 -g -ggdb
LDFLAGS =  $(CFLAGS)
OBJECTS = \
	mtxReader.o \
	bSuitor.o \
	bSuitorD.o \
	$(TARGET).o

all: $(TARGET) message


$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

message:
	echo "Executable: $(TARGET) has been created"
