#!/bin/bash

# Check if correct number of arguments are provided
if [ "$#" -ne 4 ]; then
    echo "Usage: $0 input output"
    exit 1
fi

# Define input and output variables with slash replacement
in=$(echo "$1" | tr '/' '\\')
out=$(echo "$2" | tr '/' '\\')

# Run the command with substituted variables
dosemu -quiet -dumb -f ./dosemurc -K . -E "build.bat $in $out $3 $4"
