@echo off
set filename=%1

gcc %filename%.c -o ..\object\%filename%.o -O0 -g3 -Wall -c -ID:\pyharm\SDL2\include
