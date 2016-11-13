
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
        field(initialVelocity, 4)
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
    lda.l   RomData.initialVelocity + 2,x
    sta.b   EntityStruct.xVecl + 2

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
        // Spawn explosion
        lda.b   EntityStruct.xPos.px
        sta.w   Entity.Spawn.xPos

        lda.b   EntityStruct.yPos.px
        sta.w   Entity.Spawn.yPos

        ldy.w   #0
        lda.w   #Entities.Particles.Explosion.ENTITY_ID
        jsr     Entity.Spawn

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

// vim: ft=bass-65816 ts=4 sw=4 et:

