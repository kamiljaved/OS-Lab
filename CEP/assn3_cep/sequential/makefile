CC		:= g++
PROJ_BASE	:= ..
EXEC		:= $(PROJ_BASE)/bin/sequential
INCLUDES	:= -I"$(PROJ_BASE)/include"
LDFLAGS         := -lm -lX11 -lpthread
CFLAGS		:= -O3 -g -Wall

all: clean sequential

sequential: main.cpp filters.cpp
	$(CC) -o $(EXEC) main.cpp filters.cpp ../src/Timer.cpp $(INCLUDES) $(LIBS) $(CFLAGS) $(LDFLAGS) 


clean:
	rm -f $(EXEC) *~


