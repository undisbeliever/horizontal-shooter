#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# vim: set fenc=utf-8 ai ts=4 sw=4 sts=4 et:

import math

N_ROTATIONS = 256

def f_to_hex(f):
    i = int(f * 0x7FFF)
    if i < 0:
        i = 0x10000 + i
    if i > 0xFFFF:
        i %= 0xFFFF
    return "$%04X" % i


def sine_table():
    print("// 1:15 fixed point integer")
    print("rodata(rom0)")
    print("scope SineTable: {", end="")

    d = 0.0
    end = 360.0
    step = 360.0 / N_ROTATIONS

    for i in range(N_ROTATIONS):
        f = math.cos(math.radians(d))

        if i % 8 == 0:
            print("\n  dw ", end="")
        else:
            print(", ", end="")

        print(f_to_hex(f), end="")

        d += step

    print("\n\nconstant size(", N_ROTATIONS * 2, ")")
    print("}")


def main():
    sine_table()

if __name__ == '__main__':
    main()
