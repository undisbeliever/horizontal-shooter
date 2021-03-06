// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2019, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

namespace Interface {

constant BUFFER_WIDTH = Text.Console.BUFFER_WIDTH

constant Y_OFFSET = 28 - 3
constant X_OFFSET = 3

allocate(previousHp, wram7e, 2)

// Initialize the interface
//
// MODULES: Text Console module initialized
//
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
a16()
i16()
code()
function Init {
    stz.w   previousHp

    // set text color (grey)

    // ::TODO add resource palette::
    sep     #$20
a8()

    lda.b   #1
    sta.l   CGADD

    lda.b   #ToPalette(16, 16, 16)
    sta.l   CGDATA
    lda.b   #ToPalette(16, 16, 16) >> 8
    sta.l   CGDATA

    jmp     Text.Console.ClearBuffer
}


// Updates the interface
//
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
a16()
i16()
code()
function Update {
constant HP_OFFSET = X_OFFSET + Y_OFFSET * Text.Console.BUFFER_WIDTH
constant MAX_DISPLAY_HP = BUFFER_WIDTH - X_OFFSET * 2

    ldy.w   GameLoop.playerEntity
    lda.w   Entities.Player.EntityStruct.hp,y

    // Only update interface if HP display changed
    cmp.w   previousHp
    beq     ReturnRep

    sta.w   previousHp

    // print player health
    cmp.w   #MAX_DISPLAY_HP + 1
    bcc     +
        lda.w   #MAX_DISPLAY_HP
    +
    sep     #$30
a8()
i8()
    // Print HP icons

    ldx.b   #0
    tay
    beq     SkipHpLoop
        lda.b   #Text.Font.HP_ICON
        HpLoop:
            sta.w   Text.buffer + HP_OFFSET,x

            inx
            dey
            bne     HpLoop
    SkipHpLoop:


    // Clear rest of line

    cpx.b   #MAX_DISPLAY_HP
    bcs     SkipClearLoop
        ClearLoop:
            stz.w   Text.buffer + HP_OFFSET,x

            inx
            cpx.b   #MAX_DISPLAY_HP
            bcc     ClearLoop
    SkipClearLoop:

    // X is non-zero
    stx.w   Text.bufferDirty


    // Print Game Over Message if player has 0 HP
    rep     #$30
a16()
i16()
    lda.w   previousHp
    bne     SkipGameOver
        Text.Console.SetCursor(X_OFFSET - Text.Console.MARGIN_LEFT, Y_OFFSET - Text.Console.MARGIN_TOP)

        ldx.w   #GameOverString
        lda.w   #GameOverString >> 16
        jsr     Text.Console.PrintString
SkipGameOver:
    rts


ReturnRep:
    rep     #$30
a16()
i16()
    rts
}

rodata(rom0)
function GameOverString {
    db  "Game Over", 0
}

}

// vim: ft=bass-65816 ts=4 sw=4 et:

