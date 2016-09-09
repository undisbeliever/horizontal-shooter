
scope Entities {
scope BaseEnemy {

scope EntityStruct: {
    basestruct(BaseEnemy, BaseEntity)
        field(hp, 2)
    endstruct()
}

scope RomData: {
    basestruct(BaseEnemyData, BaseEntityData)
        field(health, 2)
        field(initialVelocity, 3)
    endstruct()
}


// IN: A = parameter (currently undefined)
a16()
i16()
code()
scope Init: {
    ldx.b   EntityStruct.romPtr

    lda.l   RomData.initialVelocity,x
    sta.b   EntityStruct.xVecl
    lda.l   RomData.initialVelocity + 1,x
    sta.b   EntityStruct.xVecl + 1

    lda.l   RomData.health,x
    sta.b   EntityStruct.hp

    rts
}


macro useDefaultDestructor() {
    constant Destructor(Entities.BaseEnemy.Destructor)
}

a16()
i16()
code()
scope Destructor: {
    rts
}


a16()
i16()
code()
scope Process: {
    lda.b   EntityStruct.hp
    bne     +
        // Entity is out of HP

        // ::TODO spawn explosion

        ldy.w   #Entity.lists.toDelete
        sty.w   Entity.changeList
        rts
    +

    jmp     Physics.ProcessEntity
}


macro useDefaultCalculateAttackValue() {
    constant CalculateAttackValue(Entities.BaseEnemy.CalculateAttackValue)
}

// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
// INPUT: Y = entity attacking
// OUTPUT: A = attack value
a16()
i16()
code()
scope CalculateAttackValue: {
    lda.w   #1
    rts
}


// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
// INPUT: Y = entity attacking
// INPUT: A = attack value
a16()
i16()
code()
scope TakeDamageFromEntity: {
    // A = clamp(hp - A)

    eor.w   #0xffff
    sec
    adc.b   EntityStruct.hp
    bcs     +
        lda.w   #0
    +
    sta.b   EntityStruct.hp

    rts
}


// ::TODO collision behaviour::
a16()
i16()
code()
BlockWithShield:
CollideWithBody:
CollideWithShield:
    rts

}
}

// vim: ft=asm ts=4 sw=4 et:
