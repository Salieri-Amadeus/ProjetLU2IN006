#!/bin/bash

rm -rf mygit
gcc -o mygit main.c
./mygit init
./mygit add hash.c
./mygit commit master -m "First commit"
./mygit add listC.c
./mygit commit master -m "Second commit"
./mygit branch feature
./mygit checkout-branch feature
./mygit add listC.h
./mygit commit feature -m "Third commit"