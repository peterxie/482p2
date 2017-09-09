#!/bin/bash
for i in $(ls *.cpp); do
  make clean &> /dev/null
  make VAR="$i" EXEC="p2" debug &> /dev/null
  echo compiling $i
  file_name="${i%.cpp}"
  ./p2 > "${file_name}.out"
  diff "${file_name}.txt" "${file_name}.out" > /dev/null
  if [ $? -eq 0 ] ; then
    rm "${file_name}.out"
  else
    echo error: $file_name
  fi
done
