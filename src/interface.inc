
scope Interface {

constant BUFFER_WIDTH(Text.Console.BUFFER_WIDTH)

constant Y_OFFSET(28 - 3)
constant X_OFFSET(3)

allocate(previousHp, wram7e, 2)

// Initialize the interface
//
// MODULES: Text Console module initialized
//
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
a16()
i16()
code()
scope Init: {
    stz.w   previousHp

    // set text color (grey)

    lda.w   #16 << Palette.blue.shift | 16 << Palette.green.shift | 16 << Palette.red.shift
    sta.w   Dma.Palette.buffer + 2

    jmp     Text.Console.ClearBuffer
}


// Updates the interface
//
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
a16()
i16()
code()
scope Update: {
constant HP_OFFSET(X_OFFSET + Y_OFFSET * Text.Console.BUFFER_WIDTH)
constant MAX_DISPLAY_HP(BUFFER_WIDTH - X_OFFSET * 2)

    ldy.w   GameLoop.playerEntity
    lda.w   Entities.PlayerEntity.EntityStruct.hp,y

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
scope GameOverString: {
    db  "Game Over", 0
}

}

// vim: ft=asm ts=4 sw=4 et:

