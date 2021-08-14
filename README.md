# yes/no classifier using audio frequency

Course assignment for the subject Speech Processing (CS 566)

## Assignment 1.cpp
This is the main source code file, where we are calculating the energy and zero crossing rate (zcr) value of 150 samples and printing it to the output file.

Energy (E) = Summation of Xi^2
ZCR is incremented when (prev_xi < 0 and xi >= 0) or (prev_xi >= 0 and xi <0)

we will take above two observation for the 150 samples and then we will repeat the process.

## I/P and O/P
- Input file: assn1.txt
- Output file: op.txt

