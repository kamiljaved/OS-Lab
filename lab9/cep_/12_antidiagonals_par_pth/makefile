CC		:= g++
PROJ_BASE	:= .
INCLUDES	:= -I"../include"
LDFLAGS         := -lm -lX11 -lpthread
CFLAGS		:= -O3 -g

all: clean compare mainS main0 main1 main2

mainS: main.cpp filtersS.cpp
	@echo "> $@: Anti-Diagonals Approach - Sequential"
	$(CC) -o $(PROJ_BASE)/bin/mainS main.cpp filtersS.cpp ../src/Timer.cpp $(INCLUDES) $(LIBS) $(CFLAGS) $(LDFLAGS)

main0: main.cpp filters0.cpp
	@echo "> $@: Anti-Diagonals Approach - Parallel Pthreads (mutex lock on diff-update)"
	$(CC) -o $(PROJ_BASE)/bin/main0 main.cpp filters0.cpp ../src/Timer.cpp $(INCLUDES) $(LIBS) $(CFLAGS) $(LDFLAGS)

main1: main.cpp filters1.cpp
	@echo "> $@: Anti-Diagonals Approach - Parallel Pthreads (per-thread diff, no padding)"
	$(CC) -o $(PROJ_BASE)/bin/main1 main.cpp filters1.cpp ../src/Timer.cpp $(INCLUDES) $(LIBS) $(CFLAGS) $(LDFLAGS)

main2: main.cpp filters2.cpp
	@echo "> $@: Anti-Diagonals Approach - Parallel Pthreads (per-thread diff, with padding)"
	$(CC) -o $(PROJ_BASE)/bin/main2 main.cpp filters2.cpp ../src/Timer.cpp $(INCLUDES) $(LIBS) $(CFLAGS) $(LDFLAGS)


compare: compare.cpp
	$(CC) -o $(PROJ_BASE)/bin/compare compare.cpp $(INCLUDES) $(LIBS) $(CFLAGS) $(LDFLAGS) 

clean:
	rm -f $(PROJ_BASE)/bin/main* $(PROJ_BASE)/bin/compare *~



mainonly: cleanmain mainS main0 main1 main2 

cleanmain:
	rm -f $(PROJ_BASE)/bin/main* *~



mainSn: main.cpp filtersS.cpp
	rm -f $(PROJ_BASE)/bin/mainS *~
	@echo "> $@: Anti-Diagonals Approach - Sequential"
	$(CC) -o $(PROJ_BASE)/bin/mainS main.cpp filtersS.cpp ../src/Timer.cpp $(INCLUDES) $(LIBS) $(CFLAGS) $(LDFLAGS)

main0n: main.cpp filters0.cpp
	rm -f $(PROJ_BASE)/bin/main0 *~
	@echo "> $@: Anti-Diagonals Approach - Parallel Pthreads (mutex lock on diff-update)"
	$(CC) -o $(PROJ_BASE)/bin/main0 main.cpp filters0.cpp ../src/Timer.cpp $(INCLUDES) $(LIBS) $(CFLAGS) $(LDFLAGS)

main1n: main.cpp filters1.cpp
	rm -f $(PROJ_BASE)/bin/main1 *~
	@echo "> $@: Anti-Diagonals Approach - Parallel Pthreads (per-thread diff, no padding)"
	$(CC) -o $(PROJ_BASE)/bin/main1 main.cpp filters1.cpp ../src/Timer.cpp $(INCLUDES) $(LIBS) $(CFLAGS) $(LDFLAGS)

main2n: main.cpp filters2.cpp
	rm -f $(PROJ_BASE)/bin/main2 *~
	@echo "> $@: Anti-Diagonals Approach - Parallel Pthreads (per-thread diff, with padding)"
	$(CC) -o $(PROJ_BASE)/bin/main2 main.cpp filters2.cpp ../src/Timer.cpp $(INCLUDES) $(LIBS) $(CFLAGS) $(LDFLAGS)
	
comparen: compare.cpp
	rm -f $(PROJ_BASE)/bin/compare *~
	$(CC) -o $(PROJ_BASE)/bin/compare compare.cpp $(INCLUDES) $(LIBS) $(CFLAGS) $(LDFLAGS) 