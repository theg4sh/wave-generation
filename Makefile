# Required libopenal-dev libalut-dev

USE_OPENAL?=
OPENAL_PATH?=

USE_MINIAL?=
MINIAL_PATH?=

CXX_INCLUDE_PATH := -I./include
LIBRARIES += -lpthread
LIBRARIES += -lboost_system -lboost_program_options -lboost_filesystem

# src/altoolset files
SOURCES := $(addprefix src/altoolset/, \
			sin.o \
			generator.o \
			generator_buffer.o \
			generators/silence_generator.o \
			generators/sin_generator.o \
			generators/floating_sin_generator.o \
			generators/composite_generator.o \
			)

TESTS := $(addprefix tests/, \
			test_generator_buffer.o)

TARGETS = 

ifneq ($(USE_OPENAL),)
include mk/openal.mk
endif

ifneq ($(USE_MINIAL),)
include mk/mini_al.mk
endif



CXX_FLAGS += -O2 -ffast-math -Wall -pedantic -std=c++14 -ggdb $(CXX_INCLUDE_PATH)
CXX = g++ $(CXX_FLAGS)

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

