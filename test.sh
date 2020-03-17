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
try 1 "1==1"
try 1 "(1+2) == 3"
try 1 "(1+3) >= 2"
try 0 "(1+2) > (5+6)"
try 1 "(1+2) <= 9"
try 0 "(1+2) < 2"
try 1 "1 != 2"
try 0 "1 > 2"

echo OK
