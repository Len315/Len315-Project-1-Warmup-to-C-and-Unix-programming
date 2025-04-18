#!/bin/bash

# This script runs a series of tests for the 'reverse' program.

# Function to run a single test
run_test() {
    test_num=$1
    # Define file paths for the input, expected output, and actual output
    input_file="../initial-reverse/tests/${test_num}.run"
    expected_output_file="../initial-reverse/tests/${test_num}.out"
    actual_output_file="../initial-reverse/tests/${test_num}.actual"

    # Run the 'reverse' program with the input file and save the output to the actual output file
    ../initial-reverse/reverse "$input_file" "$actual_output_file"

    # Compare the actual output with the expected output
    if ! diff -q "$actual_output_file" "$expected_output_file"; then
	# If the outputs differ, print a failure message and show the differences
        echo "Test $test_num failed."
        diff "$actual_output_file" "$expected_output_file"
    fi

    # Clean up the actual output file
    rm "$actual_output_file"
    
    echo "Test $test_num passed."
    return 0
}

# Run the tests
for test_num in 1 2 3 4 5 6 7 8; do
    run_test "$test_num"
done
