#!/bin/bash

if [[ $# != 1 ]]; then 
echo "Wrong number of arguments" >&2
#exit the script
exit 1
fi


#for i in "$@"; do #go over all the arguments

if [[ !(-f "$1.txt") ]]; then #search for the course's file name
echo "Course not found" >&2
exit 1
fi


if [[ -d $1"_stat" ]]; then # search direcroty according to argument
rm -r $1"_stat"
fi

mkdir $1"_stat"

cat "$1.txt" | cut -f 2 > grades.txt # insert only the grades into grades file
sed -i 1d grades.txt #remove first line from the grades file (not a grade)
sed -i '/^$/d' grades.txt #remove ampty lines
mv grades.txt $1"_stat" #move "grades" file into the course folder



gcc -g -Wall calc_statistics.c -o prog.exe
./prog.exe "./$1_stat/grades.txt" "./$1_stat/course_statistics.txt"

#done
