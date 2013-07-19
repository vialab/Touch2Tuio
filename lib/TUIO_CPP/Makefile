# should be either OSC_HOST_BIG_ENDIAN or OSC_HOST_LITTLE_ENDIAN
# Apple: OSC_HOST_BIG_ENDIAN
# Win32: OSC_HOST_LITTLE_ENDIAN
# i386 LinuX: OSC_HOST_LITTLE_ENDIAN

ENDIANESS=OSC_HOST_LITTLE_ENDIAN
PLATFORM=$(shell uname)

FRAMEWORKS = -lGL -lGLU -lglut

TUIO_DEMO = TuioDemo
TUIO_DUMP = TuioDump
SIMPLE_SIMULATOR = SimpleSimulator
TUIO_STATIC  = libTUIO.a
TUIO_SHARED  = libTUIO.so

SDL_LDFLAGS := $(shell sdl-config --libs)
SDL_CFLAGS  := $(shell sdl-config --cflags)

INCLUDES = -I./TUIO -I./oscpack
CFLAGS  = -g -Wall -O3 $(SDL_CFLAGS)
CXXFLAGS = $(CFLAGS) $(INCLUDES) -D$(ENDIANESS)
SHARED_OPTIONS = -shared -Wl,-soname,$(TUIO_SHARED)

ifeq ($(PLATFORM), Darwin)
	CC = gcc-4.0
	CXX = g++-4.0
	TUIO_SHARED  = libTUIO.dylib
	FRAMEWORKS =  -framework OpenGL -framework GLUT
	SHARED_OPTIONS = -dynamiclib -Wl,-dylib_install_name,$(TUIO_SHARED)
endif

DEMO_SOURCES = TuioDemo.cpp
DEMO_OBJECTS = TuioDemo.o
DUMP_SOURCES = TuioDump.cpp
DUMP_OBJECTS = TuioDump.o
SIMULATOR_SOURCES = SimpleSimulator.cpp
SIMULATOR_OBJECTS = SimpleSimulator.o

COMMON_TUIO_SOURCES = ./TUIO/TuioTime.cpp ./TUIO/TuioPoint.cpp ./TUIO/TuioContainer.cpp ./TUIO/TuioObject.cpp ./TUIO/TuioCursor.cpp ./TUIO/TuioBlob.cpp ./TUIO/TuioDispatcher.cpp ./TUIO/TuioManager.cpp 
SERVER_TUIO_SOURCES = ./TUIO/TuioServer.cpp ./TUIO/UdpSender.cpp ./TUIO/TcpSender.cpp ./TUIO/FlashSender.cpp ./TUIO/DevSender.cpp
CLIENT_TUIO_SOURCES = ./TUIO/TuioClient.cpp ./TUIO/OscReceiver.cpp ./TUIO/UdpReceiver.cpp ./TUIO/TcpReceiver.cpp ./TUIO/DevReceiver.cpp
OSC_SOURCES = ./oscpack/osc/OscTypes.cpp ./oscpack/osc/OscOutboundPacketStream.cpp ./oscpack/osc/OscReceivedElements.cpp ./oscpack/osc/OscPrintReceivedElements.cpp ./oscpack/ip/posix/NetworkingUtils.cpp ./oscpack/ip/posix/UdpSocket.cpp

COMMON_TUIO_OBJECTS = $(COMMON_TUIO_SOURCES:.cpp=.o)
SERVER_TUIO_OBJECTS = $(SERVER_TUIO_SOURCES:.cpp=.o)
CLIENT_TUIO_OBJECTS = $(CLIENT_TUIO_SOURCES:.cpp=.o)
OSC_OBJECTS = $(OSC_SOURCES:.cpp=.o)

all: dump demo simulator static shared

static:	$(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(SERVER_TUIO_OBJECTS) $(OSC_OBJECTS)
	ar rcs $(TUIO_STATIC) $(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(SERVER_TUIO_OBJECTS) $(OSC_OBJECTS)

shared:	$(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(SERVER_TUIO_OBJECTS)  $(OSC_OBJECTS)
	$(CXX) -lpthread $+ $(SHARED_OPTIONS) -o $(TUIO_SHARED)
	
dump:	$(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(OSC_OBJECTS) $(DUMP_OBJECTS)
	$(CXX) -o $(TUIO_DUMP) $+ -lpthread

demo:	$(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(OSC_OBJECTS) $(DEMO_OBJECTS)
	$(CXX) -o $(TUIO_DEMO) $+ $(SDL_LDFLAGS) $(FRAMEWORKS)

simulator:	$(COMMON_TUIO_OBJECTS) $(SERVER_TUIO_OBJECTS) $(OSC_OBJECTS) $(SIMULATOR_OBJECTS)
	$(CXX) -o $(SIMPLE_SIMULATOR) $+ $(SDL_LDFLAGS) $(FRAMEWORKS)

clean:
	rm -f $(TUIO_DUMP) $(TUIO_DEMO) $(SIMPLE_SIMULATOR) $(TUIO_STATIC) $(TUIO_SHARED) 
	rm -f $(COMMON_TUIO_OBJECTS) $(CLIENT_TUIO_OBJECTS) $(SERVER_TUIO_OBJECTS) $(OSC_OBJECTS) $(DUMP_OBJECTS) $(DEMO_OBJECTS) $(SIMULATOR_OBJECTS)
