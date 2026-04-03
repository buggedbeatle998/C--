#!/bin/sh    

rm -r build/*
if [ -f bin/g-- ]; then
    rm bin/g--
fi
cd build/
cmake ../
cmake --build .
