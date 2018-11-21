ifneq ($(MINIAL_PATH),)
CXX_INCLUDE_PATH += -I$(MINIAL_PATH)
LIBRARIES += -lm -ldl

MINIAL_SOURCES := $(addprefix src/altoolset/mini_al/, \
			main.o)

TARGETS += minial-simple
endif
