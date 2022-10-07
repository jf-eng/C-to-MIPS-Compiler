#!/bin/bash

# Script for automated testing
# USAGE: Supply file path to desired .c file for testing. File path can be either a folder or individual .c file
# If no file path given, all test cases will be executed.
set -e # stop execution when there is error
shopt -s extglob

# make compiler
echo "========================================"
echo " Making compiler"

make bin/c_compiler

# create new empty working directory
# rm -rf results/working
mkdir -p results/working

PASSED=0
CHECKED=0

set +e # dont stop execution when theres error
if [ -z "$1" ]; then
    echo "========================================"
    echo "Running ALL custom test cases."
    for i in tests_more/*; do
        BASEFOLDER=$(basename ${i})
        mkdir -p results/working/$BASEFOLDER

        # Within each subfolder
        # Generate .s files using compiler
        for PROGRAM in tests_more/$BASEFOLDER/!(*driver.c); do
            PROGRAM_NAME=$(basename $PROGRAM .c) # removes .c from the basename
            bin/c_compiler -S $PROGRAM -o $PROGRAM_NAME.s &> $PROGRAM_NAME.compile.output $PROGRAM_NAME.cerr
            mips-linux-gnu-gcc -mfp32 -o $PROGRAM_NAME.o -c $PROGRAM_NAME.s
        done

        # DRIVER is the .cpp file containing the main function
        for DRIVER in tests_more/$BASEFOLDER/*driver.c; do
            PROGRAM_NAME=$(basename $DRIVER _driver.c) # removes _driver.c from the basename
            mips-linux-gnu-gcc -mfp32 -static -o $PROGRAM_NAME.result $PROGRAM_NAME.o $DRIVER &> $PROGRAM_NAME.linking.output
            qemu-mips $PROGRAM_NAME.result &> $PROGRAM_NAME.runtime.output
            RESULT=$?;

            if [[ "${RESULT}" -eq "0" ]]; then
                PASSED=$(( $PASSED+1 ));
                echo "Passed: ${PROGRAM_NAME}"
            else
                echo "Failed: ${PROGRAM_NAME}"
            fi

            CHECKED=$(( $CHECKED+1 ));
        done

        # Move generated assembly code to base folders
        mv *.s *.output *.cerr results/working/$BASEFOLDER
        rm -f *.o *.result

    done
fi

# echo "Done."
echo "Passed ${PASSED}/${CHECKED} test cases"