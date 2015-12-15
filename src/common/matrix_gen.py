#!/usr/bin/env python

import argparse
from random import getrandbits

parser = argparse.ArgumentParser(description="Generate a test exact cover matrix.")
parser.add_argument("rows")
parser.add_argument("cols")
parser.add_argument("path")

args = parser.parse_args()
rows = int(args.rows)
cols = int(args.cols)

file = open(args.path, "w")

file.write("{0}x{1}\n".format(rows, cols))
for i in range(0, rows):
    for j in range(0, cols):
        if getrandbits(1):
            file.write("({0},{1})\n".format(i, j))

