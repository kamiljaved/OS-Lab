# C compiler options
CC = gcc

# Enable extra warnings ( Compiler Flags)
CFLAGS = -Wall -Wno-unused-result -Wno-unused-variable

# Treat warnings as errors
# CFLAGS += -Werror

# Produce debugging information for use by gdb
CFLAGS += -g

# Include header info & define labels
TARGET  = mem
HEADERS = $(TARGET).h
TEST	= main
CLEAN	= clean

# extra
bin_dir ?= bin/

# To build files
all: $(TARGET)          

$(TARGET):
	$(CC) $(CFLAGS) -c  -fpic  $(TARGET).c  -o $(bin_dir)$(TARGET).o
	$(CC) -shared   -o  $(bin_dir)lib$(TARGET).so  $(bin_dir)$(TARGET).o
	$(CC) $(CFLAGS) -c $(TEST).c -O -o $(bin_dir)$(TEST).o
	$(CC) $(CFLAGS) -Wl,-rpath=$(bin_dir) -o $(bin_dir)$(TEST) $(bin_dir)$(TEST).o -O -lmem -L $(bin_dir)   	

# To Remove all the created files
	
$(CLEAN):                       					
	$(RM) $(bin_dir)$(TEST)
	$(RM) $(bin_dir)$(TEST).o
	$(RM) $(bin_dir)$(TARGET).o
	$(RM) $(bin_dir)lib$(TARGET).so

test: $(TARGET)
	./$(bin_dir)$(TEST)

testn: $(CLEAN) test
