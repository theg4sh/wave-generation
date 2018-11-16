# Required libopenal-dev libalut-dev


#OPENAL_PATH?=
OPENAL_PATH?=$(HOME)/repos/theg4sh/openal-soft

ifneq ($(OPENAL_PATH),)
INCLUDES = -I./include -I$(OPENAL_PATH)/include
LIBRARIES = -L$(OPENAL_PATH)/build
else
INCLUDES = -I./include
endif

LIBRARIES += -lopenal -lalut -lpthread
LIBRARIES += -lboost_system -lboost_program_options -lboost_filesystem
CXX_FLAGS += -Wall -std=c++14 -g $(INCLUDES)
CXX = g++ $(CXX_FLAGS)

# src/altoolset files
SOURCES := $(addprefix src/altoolset/, \
			generator.o \
			generators/silence_generator.o \
			generators/sin_generator.o \
			generators/floating_sin_generator.o \
			openal/wav_player.o \
			openal/queue_player.o \
			openal/context.o \
			openal/device.o)
SOURCES += main.o

all: $(SOURCES)
	$(CXX) -o openal-simple $^ $(LIBRARIES)

clean:
	rm -f ./src/altoolset/openal/*.o ./src/altoolset/*.o ./*.o
	rm -f ./openal-simple

%.o: %.cpp
	$(CXX) -c -o $@ $^ $(LIBRARIES)

