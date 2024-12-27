#! /bin/bash

if [ ! -d "./bin" ]; then
  mkdir bin;
fi

if [ -f "./bin/jsh" ]; then
  rm ./bin/jsh
fi

gcc -Wall ./src/main.c -o ./bin/jsh;

