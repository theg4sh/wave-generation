# Required libopenal-dev libalut-dev

USE_OPENAL?=
OPENAL_PATH?=

USE_MINIAL?=
MINIAL_PATH?=

INCLUDES := -I./include
LIBRARIES += -lpthread
LIBRARIES += -lboost_system -lboost_program_options -lboost_filesystem
CXX_FLAGS += -Wall -pedantic -std=c++14 -ggdb $(INCLUDES)
CXX = g++ $(CXX_FLAGS)

# src/altoolset files
SOURCES := $(addprefix src/altoolset/, \
			generator.o \
			generator_buffer.o \
			generators/silence_generator.o \
			generators/sin_generator.o \
			generators/floating_sin_generator.o)

TESTS := $(addprefix tests/, \
			test_generator_buffer.o)

TARGETS = 

ifneq ($(USE_OPENAL),)
include mk/openal.mk
endif

ifneq ($(USE_MINIAL),)
include mk/mini_al.mk
endif

.PHONY=show-sources all clean test

all: $(TARGETS)

openal-simple: $(SOURCES) $(OPENAL_SOURCES)
	$(CXX) -o $@ $^ $(LIBRARIES)

minial-simple: $(SOURCES) $(MINIAL_SOURCES)
	$(CXX) -o $@ $^ $(LIBRARIES)

clean:
	find ./src ./tests -iname "*.o" -exec rm {} \;
	rm -f $(TARGETS) ./test

show-sources:
	@echo $(CXX_FLAGS)
	@echo $(OPENAL_SOURCES)
	@echo $(SOURCES)

test: src/altoolset/generator_buffer.o $(TESTS)
	$(CXX) -o $@ $^ $(LIBRARIES)

%.o: %.cpp
	$(CXX) -c -o $@ $^ $(LIBRARIES)

