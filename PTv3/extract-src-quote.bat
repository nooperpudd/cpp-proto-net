@echo off
set SRC=D:\Workspace\GitRepository\PTv3
set MAIN_SRC=%SRC%\QuoteBroadcast
set DEST=%SRC%\quote-broadcast\
set DEST_SRC=%DEST%\src

echo 1. Copy Fast trade station .h .cpp files
copy /y %MAIN_SRC%\*.h %DEST_SRC%\*.h
copy /y %MAIN_SRC%\*.cpp %DEST_SRC%\*.cpp
echo 2. Copy Makefile and config
copy /y %MAIN_SRC%\Makefile %DEST%

pause