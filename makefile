CFLAGS = -Wall -Wextra
OS := $(shell uname -s)
LIBS = -lm


ifeq ($(OS), Darwin)
	CFLAGS += -framework AudioUnit -framework AudioToolbox -framework CoreFoundation
else ifeq ($(OS), Linux)
	LIBS += -ljack
endif

main:
	cc $(CFLAGS) unity.c -o pgrm $(LIBS) 
