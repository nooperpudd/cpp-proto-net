CC=g++
LIB_DIR=-L/usr/local/lib
CFLAGS=-O3 -DBOOST_CHRONO_DONT_PROVIDES_DEPRECATED_IO_SINCE_V2_0_0
SRC=./src
ENTITY_DIR=./src/entity
OBJ_DIR=./obj
LIBS=-lboost_thread -lboost_program_options -lboost_filesystem -lboost_chrono -lboost_date_time -llog4cpp
CTP_TD_LIBS=-lthosttraderapi
CTP_MD_LIBS=-lthostmduserapi
FM_TD_LIBS=-lUSTPtraderapi
FM_MD_LIBS=-lUSTPmduserapi
ZE_TD_LIBS=-lzeusingtraderapi

_HS_OBJS = stdafx \
		\
		FileSystemUtil HistAvatar HistConfiguration HistDataPersistence HistoryDataServer \
		HistPriceBarGen HistQuoteFetcher HistQuoteRepositry HistQuoteStore HistTechDataRepo \
		\
		HistTimeUtil HistTradingTimeSpan LogFactory MarketDataConnection MarketDataUdp \
		Quote Scheduler SymbolQuoting TradingConnection

HS_OBJS = $(patsubst %, $(OBJ_DIR)/%.o, $(_HS_OBJS))

hist-server: $(HS_OBJS)
	$(CC) -o HistDataServer $^ $(LIBS) $(CTP_TD_LIBS) $(CTP_MD_LIBS) -Wall

# Compile pt station CTP main cpp
$(OBJ_DIR)/%.o: $(SRC)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS) 

.PHONY : clean
clean: 
	rm $(OBJ_DIR)/*.*
