
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
    lda.w   Entities.lists.deactivated
    beq     NoneToDelete

    Loop:
        tcd

        MetaSprite.EntityDestructor()

        ldx.b   BaseEntity.romPtr
        lda.l   Entities.BaseEntityData.functionTable,x
        tax
        jsr     (Entities.BaseEntityFunctionTable.Destructor,x)

        lda.b   BaseEntity.next
        bne     Loop

    // Move the toDelete list into the free list

    lda.w   Entities.lists.free
    sta.b   BaseEntity.next

    lda.w   Entities.lists.deactivated
    sta.w   Entities.lists.free

    stz.w   Entities.lists.deactivated


NoneToDelete:
    rts
}

}

// vim: ft=asm ts=4 sw=4 et:

