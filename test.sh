#!/bin/bash

try(){
    expected="$1"
    input="$2"

    ./ccc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

try 21 "5+20-4"
try 100 "40+80-20"

echo OK
