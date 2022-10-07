#!/bin/bash

# Script for generating reference output
shopt -s extglob

rm -rf results/ref
mkdir -p results/ref

echo "========================================"
echo " Making reference output"


for i in tests/*; do
    BASEFOLDER=$(basename ${i})
    mkdir -p results/ref/$BASEFOLDER

    # Within each subfolder
    # Generate .s files
    for PROGRAM in tests/$BASEFOLDER/!(*driver.c); do
        PROGRAM_NAME=$(basename $PROGRAM .c) # removes .c from the basename
        mips-linux-gnu-gcc -S $PROGRAM -o $PROGRAM_NAME.s
        mips-linux-gnu-gcc -mfp32 -o $PROGRAM_NAME.o -c $PROGRAM_NAME.s
    done

    # Move generated assembly code to base folders
    mv *.s results/ref/$BASEFOLDER
    rm *.o

done

echo "Reference .s files generated!"
