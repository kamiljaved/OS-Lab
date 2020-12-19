# C compiler options
CC = gcc

# Enable extra warnings ( Compiler Flags)
CFLAGS = -Wall

# Treat warnings as errors
CFLAGS += -Werror

# Produce debugging information for use by gdb
CFLAGS += -g

# Include header info & define labels
TARGET  = mem
HEADERS = $(TARGET).h
TEST	= main
CLEAN	= clean

# To build files
all: $(TARGET)          

$(TARGET): $(HEADERS)
	$(CC) $(CFLAGS) -c  -fpic  $(TARGET).c  
	$(CC) -shared   -o  lib$(TARGET).so  $(TARGET).o
	$(CC) $(CFLAGS) -L. $(TEST).c -o $(TEST) -l $(TARGET)

# To Remove all the created files
	
$(CLEAN):                       					
	$(RM) $(TEST)
	$(RM) $(TARGET).o
	$(RM) lib$(TARGET).so
	
