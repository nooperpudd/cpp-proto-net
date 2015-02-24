CC = g++
HOME=/home/wzw/local
INC_DIR=-I$(HOME)/include
CFLAGS=$(INC_DIR)
LIB_OPT = -L$(HOME)/lib -Wl,-rpath=$(HOME)/lib
SRC = ./src
OBJ_DIR = ./obj
LIBS = -lboost_thread -lboost_program_options -lboost_filesystem -lboost_chrono -lboost_date_time

_OBJS = stdafx \
		\
		OptionReader QuoteBroadcast TickDataReader

MAIN_OBJS = $(patsubst %, $(OBJ_DIR)/%.o, $(_OBJS))

quote-broadcast: $(MAIN_OBJS)
	$(CC) -o QuoteBroadcast $^ $(LIB_OPT) $(LIBS) -Wall

# Compile main cpp files
$(OBJ_DIR)/%.o: $(SRC)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS) 

.PHONY : clean
clean: 
	rm $(OBJ_DIR)/*.*
