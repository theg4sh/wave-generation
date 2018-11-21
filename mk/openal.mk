ifneq ($(OPENAL_PATH),)
CXX_INCLUDE_PATH += -I$(OPENAL_PATH)/include
LIBRARIES += -L$(OPENAL_PATH)/build -lopenal -lalut

OPENAL_SOURCES := $(addprefix src/altoolset/openal/, \
			wav_player.o \
			queue_player.o \
			context.o \
			device.o \
			main.o)

TARGETS += openal-simple
endif


