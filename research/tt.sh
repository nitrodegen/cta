#!/bin/bash
nasm -f elf64 test.nasm -o add.o;
ld -o run add.o;
./run
