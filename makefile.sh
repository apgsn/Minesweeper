#!/bin/bash

vals=$( cat config.dat | awk '{print $2}' )
g++ Field.cpp main.cpp -lsfml-graphics -lsfml-window -lsfml-system -std=c++11 -o "Minesweeper"
./Minesweeper $vals