# Required libopenal-dev libalut-dev

USE_OPENAL?=

INCLUDES := -I./include
LIBRARIES += -lpthread
LIBRARIES += -lboost_system -lboost_program_options -lboost_filesystem
CXX_FLAGS += -Wall -std=c++14 -g $(INCLUDES)
CXX = g++ $(CXX_FLAGS)

# src/altoolset files
SOURCES := $(addprefix src/altoolset/, \
			generator.o \
			generators/silence_generator.o \
			generators/sin_generator.o \
			generators/floating_sin_generator.o)

TARGETS = 

ifneq ($(USE_OPENAL),)
OPENAL_PATH?=

ifneq ($(OPENAL_PATH),)
INCLUDES += -I$(OPENAL_PATH)/include
LIBRARIES += -L$(OPENAL_PATH)/build -lopenal -lalut

OPENAL_SOURCES := $(addprefix src/altoolset/openal/, \
			wav_player.o \
			queue_player.o \
			context.o \
			device.o \
			main.o)

TARGETS += openal-simple
endif

endif

.PHONY=show-sources all clean

all: $(TARGETS)

openal-simple: $(SOURCES) $(OPENAL_SOURCES)
	$(CXX) -o $@ $^ $(LIBRARIES)

clean:
	find ./src -iname "*.o" -exec rm {} \;
	rm -f $(TARGETS)

show-sources:
	@echo $(CXX_FLAGS)
	@echo $(OPENAL_SOURCES)
	@echo $(SOURCES)

%.o: %.cpp
	$(CXX) -c -o $@ $^ $(LIBRARIES)

