CXXFLAGS =	-O3 -Wall -std=c++11

detected_OS := $(shell uname)
ifeq ($(detected_OS),Darwin)
    CXXFLAGS += -stdlib=libc++
endif

OBJS =		pea2plus.o ArrayGraph.o Graph.o ListGraph.o Stopwatch.o

LIBS =		-pthread

TARGET =	pea2plus

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
