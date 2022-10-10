#!/bin/bash

# Script for automated testing
# USAGE: Supply file path to desired .c file for testing. File path can be either a folder or individual .c file
# If no file path given, all test cases will be executed.

shopt -s extglob

# make compiler
echo "========================================"
echo " Making compiler"

make bin/c_compiler


PASSED=0
CHECKED=0

if [ -z "$1" ]; then
    # create new empty working directory
    rm -rf results/working
    mkdir -p results/working

    echo "========================================"
    echo "No file path provided, running ALL test cases."
    for i in tests/*; do
        BASEFOLDER=$(basename ${i})
        mkdir -p results/working/$BASEFOLDER

        # Within each subfolder
        # Generate .s files using compiler
        for PROGRAM in tests/$BASEFOLDER/!(*driver.c); do
            PROGRAM_NAME=$(basename $PROGRAM .c) # removes .c from the basename
            bin/c_compiler -S $PROGRAM -o $PROGRAM_NAME.s &> $PROGRAM_NAME.compile.output $PROGRAM_NAME.cerr
            mips-linux-gnu-gcc -mfp32 -o $PROGRAM_NAME.o -c $PROGRAM_NAME.s
        done

        # DRIVER is the .cpp file containing the main function
        for DRIVER in tests/$BASEFOLDER/*driver.c; do
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
else
    if [[ -d $1 ]]; then
        echo "========================================"
        echo "$1 is a directory. Running all tests within directory"

        BASEFOLDER=$(basename $1)
        # create new empty working directory
        rm -rf results/working/$BASEFOLDER
        mkdir -p results/working/$BASEFOLDER

        # Generate .s files using compiler
        for PROGRAM in tests/$BASEFOLDER/!(*driver.c); do
            PROGRAM_NAME=$(basename $PROGRAM .c) # removes .c from the basename
            bin/c_compiler -S $PROGRAM -o $PROGRAM_NAME.s &> $PROGRAM_NAME.compile.output $PROGRAM_NAME.cerr
            mips-linux-gnu-gcc -mfp32 -o $PROGRAM_NAME.o -c $PROGRAM_NAME.s
        done

        # DRIVER is the .cpp file containing the main function
        for DRIVER in tests/$BASEFOLDER/*driver.c; do
            PROGRAM_NAME=$(basename $DRIVER _driver.c) # removes _driver.c from the basename
            mips-linux-gnu-gcc -g -mfp32 -static -o $PROGRAM_NAME.result $PROGRAM_NAME.o $DRIVER &> $PROGRAM_NAME.linking.output
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

    elif [[ -f $1 ]]; then
        # Check if file provided is a valid non-driver .c file
        if [[ "$1" == *"driver"* ]]; then
            echo "========================================"
            echo "$1 is a driver file!"
            echo "Please provide path to a non-driver .c file for compilation"
            exit 1
        fi

        echo "========================================"
        echo "$1 is a valid file. Running 1 test"

        # Get BASEFOLDER
        BASEFOLDER=$(basename $(dirname $1))
        # create new empty working directory
        rm -rf results/working/$BASEFOLDER
        mkdir -p results/working/$BASEFOLDER

        # Get PROGRAM_NAME
        PROGRAM_NAME=$(basename $1 .c)

        # Generate .s
        bin/c_compiler -S $1 -o $PROGRAM_NAME.s &> $PROGRAM_NAME.compile.output $PROGRAM_NAME.cerr
        mips-linux-gnu-gcc -mfp32 -o $PROGRAM_NAME.o -c $PROGRAM_NAME.s

        # Link with driver file
        mips-linux-gnu-gcc -mfp32 -static -o $PROGRAM_NAME.result $PROGRAM_NAME.o tests/$BASEFOLDER/$PROGRAM_NAME\_driver.c &> $PROGRAM_NAME.linking.output
        qemu-mips $PROGRAM_NAME.result &> $PROGRAM_NAME.runtime.output
        RESULT=$?;

        if [[ "${RESULT}" -eq "0" ]]; then
            PASSED=$(( $PASSED+1 ));
            echo "Passed: ${PROGRAM_NAME}"
        else
            echo "Failed: ${PROGRAM_NAME}"
        fi

        CHECKED=$(( $CHECKED+1 ));

        # Move generated assembly code to base folders
        mv *.s *.output *.cerr results/working/$BASEFOLDER
        rm -f *.o *.result
    else
        echo "========================================"
        echo "$1 is an invalid path!"
        exit 1
    fi
fi

echo "Passed ${PASSED}/${CHECKED} test cases"
if [[ $PASSED -eq $CHECKED ]]; then
    exit 0
else
    exit 1
fi
