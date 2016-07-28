@echo off
set SRC=D:\Workspace\GitRepository\PTv3
set HIST_SRC=%SRC%\HistoryDataServer
set DEST=%SRC%\hist-data-linux
set DEST_SRC=%DEST%\src
set DEST_OBJ=%DEST%\obj
set DEST_CONF=%DEST%\conf

echo 0 Make dir
if not exist %DEST% (md %DEST%)
if not exist %DEST_SRC% (md %DEST_SRC%)
if not exist %DEST_SRC%\ThostTraderApi (md %DEST_SRC%\ThostTraderApi)
if not exist %DEST_OBJ% (md %DEST_OBJ%)
if not exist %DEST_CONF% (md %DEST_CONF%)
echo 1 Copy ThostTraderApi
copy /y %HIST_SRC%\ThostTraderApi\*.h %DEST_SRC%\ThostTraderApi\
echo 2. Copy History Data Server .h .cpp files
copy /y %HIST_SRC%\*.h %DEST_SRC%\*.h
copy /y %HIST_SRC%\*.cpp %DEST_SRC%\*.cpp
echo 4. Copy Makefile and config
copy /y %HIST_SRC%\Makefile %DEST%
copy /y %HIST_SRC%\hist-log4cpp.property %DEST_CONF%
copy /y %HIST_SRC%\config.ini %DEST_CONF%

pause