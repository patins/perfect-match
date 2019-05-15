CC = clang++
TARGET = lock_free
CFLAGS = -g -O3 -std=c++14 -march=native -m64 -Xpreprocessor -fopenmp
#-I/usr/local/Cellar/parallelstl/20190321/include -I/usr/local/Cellar/tbb/2019_U5_1/include
#-L/usr/local/Cellar/gperftools/2.7/lib/
#CFLAGS = -O3
#LDFLAGS =  $(CFLAGS) -L/usr/local/Cellar/tbb/2019_U5_1/lib -ltbb -L/usr/local/Cellar/libomp/7.0.0/lib -lomp
LDFLAGS =  $(CFLAGS) -ltbb -lomp
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
