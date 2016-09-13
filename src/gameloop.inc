
scope GameLoop {

// address of the current player
allocateTmpWord(playerEntity)

scope Run {
    // Restarts the game if the player pressed start and the player is dead
    //
    // REQUIRES: 16 bit A, 16 bit Index
    macro scope ProcessStartButton() {
        assert16a()
        assert16i()

        lda.w   Controller.Joy1.pressed
        and.w   #JOY.start
        beq     EndIf
            ldy.w   GameLoop.playerEntity
            lda.w   Entities.PlayerEntity.EntityStruct.hp,y
            bne     EndIf
                jmp     Restart
        EndIf:
    }
}

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
    sta.w   Entity.Spawn.xPos
    lda.w   #Camera.DISPLAY_HEIGHT / 2 + Camera.ORIGIN
    sta.w   Entity.Spawn.yPos

    ldy.w   #0
    lda.w   #Entities.PlayerEntity.ENTITY_ID
    jsr     Entity.Spawn
    sty.w   playerEntity

    Spawner.Init()


    // Process the entities
    // --------------------
    Loop:
        Controller.UpdateControllerState()
        ProcessStartButton()

        jsr     MetaSprite.Render.InitLoop

        jsr     Hacks.DeleteOffscreenEntities

        jsr     Entity.ProcessGameLoop
        jsr     Camera.StarField.ProcessFrame

        Spawner.ProcessGameLoop()

        jsr     Entity.RenderLoop

        jsr     MetaSprite.Render.EndLoop

        jsr     Interface.Update

        jsr     Hacks.WaitFrameCpuUsage

        bra     Loop
}


// Resets the PPU registers to initial state
//
// All tests that change PPU registers MUST call this routine upon clean-up
//
// REGISTER OUTPUT: DB = 0x7e, 16 bit A, 16 bit Index
a16()
i16()
macro Init() {
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
    // BG1 - starfield (64x32 tilemap, 16x16px tiles)
    // BG3 - text console buffer (32x32x tilemap, 8x8px tiles)
    // OAM - objects

    lda.b   #INIDISP.force
    sta.w   INIDISP

    lda.b   #BGMODE.mode1Bg3
    sta.w   BGMODE

    lda.b   #((VRAM_STARFIELD_MAP_WADDR / BGXSC.base.walign) << BGXSC.base.shift) | BGXSC.map.s64x32
    sta.w   BG1SC

    lda.b   #(VRAM_STARFIELD_TILES_WADDR / BG12NBA.walign) << BG12NBA.bg1.shift
    sta.w   BG12NBA

    lda.b   #((VRAM_CONSOLE_MAP_WADDR / BGXSC.base.walign) << BGXSC.base.shift) | BGXSC.map.s32x32
    sta.w   BG3SC

    lda.b   #(VRAM_CONSOLE_TILES_WADDR / BG34NBA.walign) << BG34NBA.bg1.shift
    sta.w   BG34NBA

    lda.b   #(VRAM_OBJ_WADDR / OBSEL.base.walign) | OBSEL.size.s8_16
    sta.w   OBSEL

    lda.b   #TM.bg1 | TM.bg3 | TM.obj
    sta.w   TM


    jsr     Camera.StarField.LoadGraphics
    jsr     Text.Console.Init

    jsr     EnableVBlank

    plb
// DB = 0x7e

    rep     #$30
a16()
i16()

    // Setup the modules
    // -----------------
    jsr     Entity.Init
    jsr     Camera.StarField.Init
    jsr     Interface.Init


    // Enable Screen
    // -------------
    sep     #$20
a8()

    // Transfer palette buffer to CGRAM on next frame
    Dma.Palette.MarkDirty(all)

    // Enable screen
    lda.b   #15
    sta.l   INIDISP

    rep     #$30
a16()
i16()
}


// Restarts the game
//
// REQUIRES: none
scope Restart: {
    rep     #$30
a16()
i16()
    Init()
    jmp     Run
}


}

// vim: ft=bass-65816 ts=4 sw=4 et:

