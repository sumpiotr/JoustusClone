@echo off
set PATH=%PATH%;C:\mingw64\bin
g++.exe *.cpp -o bin\server.exe -L C:\SFML-2.5.1\lib -I C:\SFML-2.5.1\include -L C:\mingw64\lib\ -I C:\mingw64\include\ -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network -pthread
bin\server.exe %1 %2