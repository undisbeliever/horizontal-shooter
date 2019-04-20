// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2019, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

namespace Camera {
namespace StarField {

// Camera Velocity
// (integer)
constant VELOCITY_INT = 2


// Load starfield graphics into VRAM and palette to buffer
// REQUIRES: 8 bit A, 16 bit Index, DB = REGISTER_DB, force blank
a8()
i16()
code()
function LoadGraphics {
    ldx.w   #VRAM_STARFIELD_TILES_WADDR
    stx.w   VMADD
    Dma.ForceBlank.ToVram(Images.Stars.Tiles)

    ldx.w   #VRAM_STARFIELD_MAP_WADDR
    stx.w   VMADD
    Dma.ForceBlank.ToVram(Images.Stars.Map)


    stz.w   CGADD
    lda.l   Images.Stars.Palette
    sta.w   CGDATA
    lda.l   Images.Stars.Palette + 1
    sta.w   CGDATA


    lda.b   #16
    sta.w   CGADD

    ldx.w   #0
    Loop:
        lda.l   Images.Stars.Palette,x
        sta.w   CGDATA

        inx
        cpx.w   #Images.Stars.Palette.size
        bcc     Loop

    rts
}


// Initialize starfield camera module
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
a16()
i16()
code()
function Init {
    sta.w   ScrollBuffer.bg1.hOffset
    sta.w   ScrollBuffer.bg1.vOffset

    jmp     Camera.Init
}


// Process a single gameloop frame
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
a16()
i16()
code()
function ProcessFrame {
    lda.w   Camera.xPos
    clc
    adc.w   #VELOCITY_INT
    sta.w   Camera.xPos
    sec
    sbc.w   #Camera.ORIGIN
    sta.w   ScrollBuffer.bg1.hOffset

    rts
}

}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

