#!/bin/bash

gcc -o mygit main.c
./mygit init
./mygit add file.tmp
./mygit commit master -m "First commit"