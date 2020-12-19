CC		:= g++
debug		:= 0
PROJ_BASE	:= ..
EXEC		:= $(PROJ_BASE)/bin/compare

IMAGE0		:=$(PROJ_BASE)/images/image01.jpg
IMAGE1		:=$(PROJ_BASE)/images/image01.jpg

INCLUDES	:= -I"$(PROJ_BASE)/include"
LDFLAGS         := -lm -lX11 -lpthread

ifneq ($(debug), 1)
	CFLAGS		:= -O3 -g0 -w
else
	CFLAGS		:= -O0 -g3 -Wall
endif

all: clean compare run
 
compare: compare.cpp
	$(CC) -o $(PROJ_BASE)/bin/compare compare.cpp $(INCLUDES) $(LIBS) $(CFLAGS) $(LDFLAGS) 

run: 
	$(EXEC) $(IMAGE0) $(IMAGE1)

clean:
	rm -f $(EXEC) *~

