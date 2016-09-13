
scope Hacks {

// Hack that deletes all of the inactive entities
//
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
//
// MODIFIES: DP
a16()
i16()
code()
scope DeleteOffscreenEntities: {
    lda.w   Entity.lists.deactivated
    beq     NoneToDelete

    Loop:
        tcd

        MetaSprite.EntityDestructor()

        ldx.b   BaseEntity.romPtr
        lda.l   BaseEntityData.functionTable,x
        tax
        jsr     (BaseEntityFunctionTable.Destructor,x)

        lda.b   BaseEntity.next
        bne     Loop

    // Move the toDelete list into the free list

    lda.w   Entity.lists.free
    sta.b   BaseEntity.next

    lda.w   Entity.lists.deactivated
    sta.w   Entity.lists.free

    stz.w   Entity.lists.deactivated


NoneToDelete:
    rts
}


// Waits until the start of a new frame.
//
// If select is pressed then the CPU usage will be displayed to the user
// using INIDISP.
//
// INPUT: 16 bit A, 16 bit Index, DB = 0x7e, AUTOJOY, NMI
a16()
i16()
code()
scope WaitFrameCpuUsage: {
// state of the cpu usage debugger
// (byte)
allocate(_showCpuUsage, wram7e, 1)

    jsr     Dma.TransferOnNextVBlank

    sep     #$20
a8()

    // invert _showCpuUsage when select pressed

    lda.w   Controller.Joy1.pressed + 1
    bit.b   #JOYH.select
    beq     EndIf

        lda.w   _showCpuUsage
        beq     +
            lda.b   #0
            bra     ++
        +
            dec
        +
        sta.w   _showCpuUsage
EndIf:

    lda.w   _showCpuUsage
    beq     NoCpuUsage
        lda.b   #7
        sta.l   INIDISP

        jsr     WaitFrame

        lda.b   #15
        sta.l   INIDISP

        rep     #$30
    a16()
        rts

NoCpuUsage:
    rep     #$20
a16()
    jmp     WaitFrame
}

}

// vim: ft=bass-65816 ts=4 sw=4 et:

