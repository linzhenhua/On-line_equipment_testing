CC = g++

ROOT_DIR = $(shell pwd)
SRCS_DIR = $(shell find $(ROOT_DIR) -maxdepth 1 -type d)

TARGET = $(ROOT_DIR)/bin/main

INC_PATH = -I$(ROOT_DIR)/include 
LIB_PATH = -L$(ROOT_DIR)/lib -Wl,-rpath=$(ROOT_DIR)/lib
LIBS = -no-pie -lpcap -lnet -lm -lpthread

SRCS_C = $(foreach dir, $(SRCS_DIR), $(wildcard $(dir)/*.c))
SRCS_CPP = $(foreach dir, $(SRCS_DIR), $(wildcard $(dir)/*.cpp))
OBJS = $(patsubst %.cpp, %.o, $(SRCS_CPP)) $(patsubst %.c, %.o, $(SRCS_C))

CFLAGS = -std=c++11 -g -Wall -O3

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^ $(LIB_PATH) $(LIBS) 

%.o:%.cpp
	$(CC) $(INC_PATH) $(CFLAGS) -c -o $@ $<

%.o:%.c
	$(CC) $(INC_PATH) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	@-rm -fr $(OBJS)
	@-rm -fr $(TARGET)
