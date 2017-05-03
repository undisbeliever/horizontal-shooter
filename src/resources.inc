
include "../gen/metasprites.inc"
include "../resources/text/text.inc"

scope Text {
    scope Font {
rodata(rom0)
        // ::TODO convert font-fixed tiles to 1bpp::
        insert FixedTiles, "../gen/font-fixed.2bpp"
        constant FixedTiles.bitDepth(2)
    }
}

scope Images {
    scope Stars {
rodata(rom0)
        insert Tiles, "../gen/images/stars.4bpp"
        insert Palette, "../gen/images/stars.clr"
        insert Map, "../gen/images/stars.map"
        constant PaletteOffset(1)
    }
}

// vim: ft=bass-65816 ts=4 sw=4 et:

