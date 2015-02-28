@echo off
set SRC=D:\Workspace\GitRepository\PTv3
set PT_SRC=%SRC%\PTStation
set DEST=%SRC%\pt-station-linux\
set DEST_SRC=%DEST%\src

echo 1. Copy proto files 
copy /y %SRC%\proto\*.proto %DEST%\proto
echo 2.1 Copy ThostTraderApi
copy /y %PT_SRC%\ThostTraderApi\*.h %DEST_SRC%\ThostTraderApi\
echo 2.2 Copy FemasApi
copy /y %PT_SRC%\FemasAPI\*.h %DEST_SRC%\FemasAPI\
echo 2.3 Copy ZeusingAPI
copy /y %PT_SRC%\ZeusingAPI\*.h %DEST_SRC%\ZeusingAPI\
echo 3. Copy Fast trade station .h .cpp files
copy /y %PT_SRC%\*.h %DEST_SRC%\*.h
copy /y %PT_SRC%\*.cpp %DEST_SRC%\*.cpp
echo 4. Copy Makefile and config
copy /y %PT_SRC%\Makefile %DEST%
copy /y %PT_SRC%\log4cpp.property %DEST%\conf
copy /y %PT_SRC%\qs-log4cpp.property %DEST%\conf

pause