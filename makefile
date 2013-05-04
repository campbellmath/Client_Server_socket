###############################################################################
# compiler setting
###############################################################################
CC       = gcc
CXX      = g++
CFLAGS   = -g -Wall
CXXFLAGS = $(CFLAGS) -Weffc++ -pedantic-errors
LIBS     = -lm
INCPATH  =

###############################################################################
# source files setting
###############################################################################
C_SOURCES   = $(shell find . -name "*.c")
CXX_SOURCES = $(shell find . -name "*.cpp")
C_OBJS      = $(patsubst %.c,%.o,$(wildcard $(C_SOURCES)))
CXX_OBJS    = $(patsubst %.cpp,%.o,$(wildcard $(CXX_SOURCES)))
OBJS        = $(C_OBJS) $(CXX_OBJS)
EXEC        = server client

###############################################################################
.PHONY : clean clean_all
###############################################################################
all: $(EXEC)

%.o:%.c %.h
	$(CC) -c $(CFLAGS) $(INCPATH) $< -o $@
%.o:%.cpp %.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $< -o $@

server: TCPServer.o server.o
	$(CXX) TCPServer.o server.o -o server $(LIBS)

client: TCPClient.o client.o
	$(CXX) TCPClient.o client.o -o client $(LIBS)

###############################################################################
clean:
	@rm -vfr $(OBJS)
clean_all: clean
	@rm -vfr $(EXEC)
###############################################################################
