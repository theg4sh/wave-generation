# Required libopenal-dev libalut-dev

OPENAL_PATH?=

INCLUDES := -I./include
ifneq ($(OPENAL_PATH),)
INCLUDES += -I$(OPENAL_PATH)/include
LIBRARIES = -L$(OPENAL_PATH)/build
else
$(error "OPENAL_PATH is not set")
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
			generators/floating_sin_generator.o)

ifneq ($(OPENAL_PATH),)
OPENAL_SOURCES := $(addprefix src/altoolset/openal/, \
			wav_player.o \
			queue_player.o \
			context.o \
			device.o)

SOURCES := $(SOURCES) $(OPENAL_SOURCES)
endif

SOURCES += main.o

.PHONY=show-sources all clean

all: $(SOURCES)
	$(CXX) -o openal-simple $^ $(LIBRARIES)

clean:
	find ./src -iname "*.o" -exec rm {} \;
	rm -f ./openal-simple

show-sources:
	@echo $(CXX_FLAGS)
	@echo $(OPENAL_SOURCES)
	@echo $(SOURCES)

%.o: %.cpp
	$(CXX) -c -o $@ $^ $(LIBRARIES)

