
scope Images {
rodata(rom0)
    scope Stars {
        insert Tiles, "stars.4bpp"
        insert Palette, "stars.clr"
        insert Map, "stars.map"
        constant PaletteOffset(1)
    }
}

// vim: ft=asm ts=4 sw=4 et:
