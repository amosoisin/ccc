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

try 10 "5 +20 /4"
try 17 "2 + 3*5"
try 2 "7+-5"
try 14 "-7*-2"

echo OK
