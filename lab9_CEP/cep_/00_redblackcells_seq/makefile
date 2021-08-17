CC		:= g++
PROJ_BASE	:= .
INCLUDES	:= -I"../include"
LDFLAGS         := -lm -lX11 -lpthread
CFLAGS		:= -O3 -g -Wall

all: clean main compare

main: main.cpp filters.cpp
	$(CC) -o $(PROJ_BASE)/bin/main main.cpp filters.cpp ../src/Timer.cpp $(INCLUDES) $(LIBS) $(CFLAGS) $(LDFLAGS) 

compare: compare.cpp
	$(CC) -o $(PROJ_BASE)/bin/compare compare.cpp $(INCLUDES) $(LIBS) $(CFLAGS) $(LDFLAGS) 

clean:
	rm -f $(PROJ_BASE)/bin/main $(PROJ_BASE)/bin/compare *~