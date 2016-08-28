#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# vim: set fenc=utf-8 ai ts=4 sw=4 sts=4 et:

import sys

def main(filename, pId):
    pOffset = pId << 2

    with open(filename, "rb") as fp:
        data = bytearray(fp.read())

    for i in range(1, len(data), 2):
        p = (data[i] + pOffset) & 0b00011100
        data[i] = data[i] & 0b11100011 | p

    with open(filename, "wb") as fp:
        fp.write(data)

if __name__ == "__main__":
    if len(sys.argv) == 3:
        main(sys.argv[1], int(sys.argv[2]))
    else:
        print("Usage: %s <file> <palette>" % sys.argv[0])

