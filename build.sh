#! /bin/zsh

cmake -B build 
cd build && make
cd ../ && ./bin/.main
