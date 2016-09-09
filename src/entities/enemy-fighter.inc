
scope Entities {
scope EnemyFighter {

buildFunctionTable(BaseEntityFunctionTable, EnemyFighter)
Entity.useDefaultRenderer()

scope EntityStruct: {
    childstruct(BaseEntity)
        field(hp, 2)
    endstruct()
}

scope RomData: {
    childstruct(BaseEntityData)
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

    lda.w   #MSEO.Enemy.Animations.Flying_Left
    jsr     MetaSprite.SetAnimation

    rts
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
        // Entity is dead

        // ::TODO spawn explosion

        ldy.w   #Entity.lists.toDelete
        sty.w   Entity.changeList
        rts
    +

    jsr     Physics.ProcessEntity
    rts
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
    bpl     +
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


macro _entity(name, frameSetId, velocity) {
    scope {name} {
        rodata(EN_RomData)
        scope RomData: {
            dw  FunctionTable
            db  Entity.ListId.ENEMY // initial List
            dw  MSFS.{frameSetId}   // frameSetId
            db  0                   // defaultPalette
            dw  3                   // HP
            dl  {velocity}          // launch velocity
        }

        rodata(EN_RomDataList)
        constant ENTITY_ID((pc() - Entity.Data.List) / 2)
            dw  RomData
    }
}
_entity(Slow,   EnemyFighter,   -0x011234)
_entity(Medium, EnemyFighter,   -0x015678)
_entity(Fast,   EnemyFighter,   -0x019abc)

}
}

// vim: ft=asm ts=4 sw=4 et:

