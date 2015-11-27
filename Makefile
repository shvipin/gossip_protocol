# Single Author info:
# skukret Sarthak Kukreti
# Group info:
# djzager David Joshua Zager
# skukret Sarthak Kukreti
# vsharma5 Vipin Sharma
SRC=.
CC=gcc
#CFLAGS= -O3 -I$(INC) -I$(SRC)
CFLAGS= -g -I$(INC) -I$(SRC) -lpthread
BUILD=.

DEPS = $(wildcard $(SRC)/*.c)
OBJECTS = $(patsubst $(SRC)/%.c, $(BUILD)/%.o, $(DEPS))

default: all

clean:
	rm -f *.txt p4.o p4 endpoints netif.o client.o server.o util.o list* *.txt

$(BUILD)/%.o: $(SRC)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o p4

a4: default
