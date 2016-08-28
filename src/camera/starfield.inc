
scope Camera {
scope StarField {

// Camera Velocity
// (integer)
constant VELOCITY_INT(2)


// Load starfield graphics into VRAM and palette to buffer
// REQUIRES: 8 bit A, 16 bit Index, DB = REGISTER_DB, force blank
a8()
i16()
code()
scope LoadGraphics: {
    ldx.w   #VRAM_STARFIELD_TILES_WADDR
    stx.w   VMADD
    Dma.ForceBlank.ToVram(Images.Stars.Tiles)

    ldx.w   #VRAM_STARFIELD_MAP_WADDR
    stx.w   VMADD
    Dma.ForceBlank.ToVram(Images.Stars.Map)

    rep     #$30
a16()
    lda.l   Images.Stars.Palette
    sta.l   Dma.Palette.buffer

    ldx.w   #Images.Stars.Palette.size - 2
    Loop:
        lda.l   Images.Stars.Palette,x
        sta.l   Dma.Palette.buffer + Images.Stars.PaletteOffset * 32,x

        dex
        dex
        bpl     Loop

    sep     #$20
a8()

    rts
}


// Initialize starfield camera module
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
a16()
i16()
code()
scope Init: {
    sta.w   ScrollBuffer.bg1.hOffset
    sta.w   ScrollBuffer.bg1.vOffset

    jmp     Camera.Init
}


// Process a single gameloop frame
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
a16()
i16()
code()
scope ProcessFrame: {
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

// vim: ft=asm ts=4 sw=4 et:

