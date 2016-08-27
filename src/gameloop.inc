
scope GameLoop {

// Executes the gameloop
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// LOOPS FOREVER
a16()
i16()
code()
scope Run: {
    // Spawn the player
    // ----------------
    lda.w   #Camera.DISPLAY_WIDTH / 2 + Camera.ORIGIN
    sta.w   Entities.Spawn.xPos
    lda.w   #Camera.DISPLAY_HEIGHT / 2 + Camera.ORIGIN
    sta.w   Entities.Spawn.yPos

    ldy.w   #0
    lda.w   #Entities.PlayerEntity.ENTITY_ID
    jsr     Entities.Spawn



    // Process the entities
    // --------------------
    Loop:
        jsr     MetaSprite.Render.InitLoop

        jsr     Entities.ProcessGameLoop
        jsr     Entities.RenderLoop

        jsr     MetaSprite.Render.EndLoop

        jsr     Dma.TransferOnNextVBlank
        jsr     WaitFrame

        bra     Loop
}


// Resets the PPU registers to initial state
//
// All tests that change PPU registers MUST call this routine upon clean-up
//
// REGISTER OUTPUT: DB = 0x7e, 16 bit A, 16 bit Index
a16()
i16()
code()
scope Init: {
    rep     #$30
    sep     #$20
a8()
i16()
    pea     (0x7e << 8) | REGISTER_DB
    plb
// DB = REGISTER_DB

    jsr     ResetRegisters


    // Setup Screen
    //
    // MODE1
    // BG1 - text console buffer
    // OAM - text screen

    lda.b   #INIDISP.force
    sta.w   INIDISP

    stz.w   BGMODE

    lda.b   #((VRAM_CONSOLE_MAP_WADDR / BGXSC.base.walign) << BGXSC.base.shift) | BGXSC.map.s32x32
    sta.w   BG1SC

    lda.b   #(VRAM_CONSOLE_TILES_WADDR / BG12NBA.walign) << BG12NBA.bg1.shift
    sta.w   BG12NBA

    lda.b   #(VRAM_OBJ_WADDR / OBSEL.base.walign) | OBSEL.size.s8_16
    sta.w   OBSEL

    lda.b   #TM.bg1 | TM.obj
    sta.w   TM


    // text text color (white)
    lda.b   #1
    sta.w   CGADD

    lda.b   #0xff
    sta.w   CGDATA
    sta.w   CGDATA


    jsr     EnableVBlank

    jsr     Text.Console.Init

    plb
// DB = 0x7e

    rep     #$30
a16()
i16()

    // Setup the modules
    // -----------------
    jsr     Entities.Init


    // Enable Screen
    // -------------
    sep     #$20
a8()
    // Enable screen
    lda.b   #15
    sta.l   INIDISP

    rep     #$30
a16()
i16()
    rts
}


}


// vim: ft=asm ts=4 sw=4 et:

