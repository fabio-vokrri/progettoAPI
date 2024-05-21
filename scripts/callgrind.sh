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

# executes callgrind tool on the executable file produced by the compile.sh script
valgrind --tool=callgrind "$base_folder"./$file_name < $input_file > $output_folder$file_name.output.txt

# I really don't know
export $(dbus-launch)
kcachegrind

# Removes the executable file
rm $base_folder$file_name

# Removes the callgrind log file
rm $base_folder"scripts/"callgrind.out.* 