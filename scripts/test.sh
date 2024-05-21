#!/bin/bash

# base folder from where execution starts
base_folder="/home/fabio/programming/C/API_finalProject/"
output_folder=$base_folder"outputs/"
input_folder=$base_folder"opens/"

total_tests=111
passed=0

# Provide the name of the C file as the first argument
c_file=$base_folder$1
file_name=$(basename "$c_file")
extension="${file_name##*.}"
file_name="${file_name%.*}"

# compile c file
gcc -Wall -Werror -Wmaybe-uninitialized -Wuninitialized -pedantic -g -O0 -o $base_folder$file_name $c_file

# function that compares the output to the given test case
function compare {
  diff -q "$1" "$2" &>/dev/null
    if [ $? -eq 0 ]; then
      echo -e "\e[32mTest $3: PASSED\e[0m"
      return 1
    else    
      echo -e "\e[31mTest $3: FAILED\e[0m"
      return 0
    fi
}


for i in {1..111}; do
  input_file=$input_folder"open_$i.txt"
  expected_output=$input_folder"open_$i.output.txt"

  # Run the executable with input redirection
  "$base_folder"./"$file_name" < $input_file > $output_folder$file_name.output.txt
  if compare $expected_output $output_folder$file_name.output.txt $i; then
    ((passed++))
  fi
done

echo "TEST PASSED: $((passed * 100 / total_tests))%"

# Removes the executable file and its output
rm $output_folder$file_name.output.txt
