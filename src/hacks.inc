
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
        lda.l   Entity.BaseEntityData.functionTable,x
        tax
        jsr     (Entity.BaseEntityFunctionTable.Destructor,x)

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

}

// vim: ft=asm ts=4 sw=4 et:

