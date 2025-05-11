#!/bin/bash

# Directory where the tests are stored
TEST_DIR="./tests"

# Test program
TEST_PROGRAM="./snake"

# Loop through all the test cases (assumes test cases are numbered, e.g., 01-simple, 02-complex, etc.)
for TEST_CASE in $TEST_DIR/*-in.snk; do
    # Extract test case name (e.g., 01-simple)
    TEST_NAME=$(basename "$TEST_CASE" "-in.snk")

    # Set paths for input and reference files
    INPUT_FILE="$TEST_DIR/$TEST_NAME-in.snk"
    REF_FILE="$TEST_DIR/$TEST_NAME-ref.snk"

    echo "Running Valgrind on test case: $TEST_NAME"

    # Run Valgrind on the test program with input and output flags
    valgrind $TEST_PROGRAM -i $INPUT_FILE -o /dev/null

    # Check if valgrind ran successfully
    if [ $? -eq 0 ]; then
        echo "Test $TEST_NAME completed successfully!"
    else
        echo "Test $TEST_NAME failed!"
    fi

    echo "-------------------------------------"
done