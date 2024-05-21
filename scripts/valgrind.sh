#!/bin/bash

# base folder from where execution starts
base_folder="/home/fabio/programming/C/API_finalProject/"
output_folder=$base_folder"outputs/"

# Provide the name of the C file as the first argument
c_file=$base_folder$1
input_file=$base_folder$2

# Extract the filename without the extension
file_name=$(basename "$c_file")
extension="${file_name##*.}"
file_name="${file_name%.*}"

# compile c file
gcc -Wall -Werror -Wmaybe-uninitialized -Wuninitialized -pedantic -g3 -o $base_folder$file_name $c_file

# executes valgrind tool on the executable file produced by the compile.sh script
valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all -s "$base_folder"./$file_name < $input_file > $base_folder'outputs/'$file_name.output.txt

# Removes the executable file
rm $base_folder$file_name