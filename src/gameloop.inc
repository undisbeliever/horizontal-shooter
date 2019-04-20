// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2017, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

namespace GameLoop {

// address of the current player
allocateTmpWord(playerEntity)

namespace Run {
    // Restarts the game if the player pressed start and the player is dead
    //
    // REQUIRES: 16 bit A, 16 bit Index, DB access shadow
    macro ProcessStartButton() {
        assert16a()
        assert16i()

        lda.w   Controller.Joy1.pressed
        and.w   #JOY.start
        beq     EndIf
            ldy.w   GameLoop.playerEntity
            lda.w   Entities.Player.EntityStruct.hp,y
            bne     EndIf
                jmp     Restart
        EndIf:
    }

    // Toggles showCpuUsageMeter if the player pressed the select button
    //
    // INPUT: 16 bit A, DB access shadow
    a16()
    i16()
    code()
    macro ToggleCpuUsageOnSelect() {
        assert16a()

        lda.w   Controller.Joy1.pressed
        bit.w   #JOY.select
        beq     EndIf
            sep     #$20
        a8()

            lda.w   showCpuUsageMeter
            beq     +
                lda.b   #0
                bra     ++
            +
                dec
            +
            sta.w   showCpuUsageMeter

            rep     #$30
        a16()
        EndIf:
    }
}

// Executes the gameloop
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// LOOPS FOREVER
a16()
i16()
code()
function Run {
    // Spawn the player
    // ----------------
    lda.w   #Camera.DISPLAY_WIDTH / 5 + Camera.ORIGIN
    sta.w   Entity.Spawn.xPos
    lda.w   #Camera.DISPLAY_HEIGHT / 2 + Camera.ORIGIN
    sta.w   Entity.Spawn.yPos

    ldy.w   #0
    lda.w   #Project.EntityIds.Player
    jsr     Entity.Spawn
    sty.w   playerEntity

    Spawner.Init()


    // Process the entities
    // --------------------
    Loop:
        ProcessStartButton()
        ToggleCpuUsageOnSelect()

        jsr     MetaSprite.Render.InitLoop

        jsr     Camera.StarField.ProcessFrame

        Spawner.ProcessGameLoop()

        Entity.ProcessSpecialLists()

        MetaSprite.EntityHitbox.Begin()
            MetaSprite.EntityHitbox.TestCollisions(Players, Enemies, EnemyProjectiles)
            MetaSprite.EntityHitbox.TestCollisions(PlayerProjectiles, Enemies)
        MetaSprite.EntityHitbox.End()

        Entity.ProcessEntityLists()

        jsr     MetaSprite.Render.EndLoop

        jsr     Interface.Update

        jsr     WaitFrame

        jmp     Loop
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

    Warnings.Reset()

    jsr     ResetRegisters


    // Setup Screen
    //
    // MODE1
    // BG1 - starfield (64x32 tilemap, 16x16px tiles)
    // BG3 - text console buffer (32x32x tilemap, 8x8px tiles)
    // OAM - objects

    jsr     ForceBlank

    lda.b   #BGMODE.mode1Bg3
    sta.w   BGMODE

    lda.b   #((VRAM_STARFIELD_MAP_WADDR / BGXSC.base.walign) << BGXSC.base.shift) | BGXSC.map.s64x32
    sta.w   BG1SC

    lda.b   #(VRAM_STARFIELD_TILES_WADDR / BG12NBA.walign) << BG12NBA.bg1.shift
    sta.w   BG12NBA

    lda.b   #((VRAM_CONSOLE_MAP_WADDR / BGXSC.base.walign) << BGXSC.base.shift) | BGXSC.map.s32x32
    sta.w   BG3SC

    lda.b   #(VRAM_CONSOLE_TILES_WADDR / BG34NBA.walign) << BG34NBA.bg3.shift
    sta.w   BG34NBA

    lda.b   #(VRAM_OBJ_WADDR / OBSEL.base.walign) | OBSEL.size.s8_16
    sta.w   OBSEL

    lda.b   #TM.bg1 | TM.bg3 | TM.obj
    sta.w   TM


    jsr     Camera.StarField.LoadGraphics
    jsr     Text.Console.Init


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

    jsr     EnableDisplay_Full

    rep     #$30
a16()
i16()
}


// Restarts the game
//
// REQUIRES: none
function Restart {
    rep     #$30
a16()
i16()
    Init()
    jmp     Run
}


}

// vim: ft=bass-65816 ts=4 sw=4 et:

