#!/usr/bin/env python
import sys
windowlen = 4
lines = ['A','B','C','D','E','F','G']


for current in range(len(lines)):
    offset = max(0,min(max(0,current-2), len(lines)-windowlen))
    for i in range(windowlen):
        if i+offset == current:
            sys.stdout.write(">")
        else:
            sys.stdout.write(" ")
        print(lines[i+offset])
    print("\n")
