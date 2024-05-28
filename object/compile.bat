@echo off
gcc gamelogic.o boxes.o drawtools.o constants.o handmade_math.o main.o -O0 -LD:\pyharm\SDL2\lib -ID:\pyharm\SDL2\include -Wall -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -o ..\main.exe
: -I <include> specifies header file directory
: -l <lib> specifies the library to be linked, which should be in the lib-path
: -Wall shows all warning messages
: -o <filename>: specifies the output filename
