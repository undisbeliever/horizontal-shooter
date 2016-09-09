
scope Entities {
scope EnemySinusoidal {

constant MISSILE_THREASHOLD(15)
constant MISSILE_ACTION_POINT(1)
constant SECOND_SHOT_COOLDOWN(30)

buildFunctionTable(BaseEntityFunctionTable, EnemySinusoidal)
Entity.useDefaultRenderer()
BaseEnemy.useDefaultDestructor()
BaseEnemy.useDefaultCalculateAttackValue()


scope EntityStruct: {
    childstruct(BaseEnemy)
        field(sineTableIndex, 2)
        field(missileCooldown, 2)
    endstruct()
}

scope RomData: {
    childstruct(BaseEnemyData)
        field(missileCooldown, 2)
    endstruct()
}


// IN: A = parameter (currently undefined)
a16()
i16()
code()
scope Init: {
    lda.w   #MSEO.Enemy.Animations.Flying_Left
    jsr     MetaSprite.SetAnimation

    stz.b   EntityStruct.sineTableIndex
    stz.b   EntityStruct.missileCooldown

    jmp     BaseEnemy.Init
}


a16()
i16()
code()
scope Process: {
    // Change velocity

    lda.b   EntityStruct.sineTableIndex
    clc
    adc.w   #2 * 2
    tax
    cpx.w   #SineTable.size
    bcc     +
        ldx.w   #0
    +
    stx.b   EntityStruct.sineTableIndex

    lda.l   SineTable,x
    asl
    sta.b   EntityStruct.yVecl

    // sign extend
    sep     #$20
a8()
    bcs     +
        lda.b   #0
        bra     ++
    +
        lda.b   #0xff
    +
    sta.b   EntityStruct.yVecl + 2

    rep     #$20
a16()



    // Spawn missile is near player and cooldown = 0
    lda.b   EntityStruct.missileCooldown
    beq     SpawnMissileTest
        dec
        bpl     +
            lda.w   #0
        +
        sta.b   EntityStruct.missileCooldown

        cmp.w   #SECOND_SHOT_COOLDOWN
        bne     SkipMissileTest


    SpawnMissileTest:
        // Missile test:
        //   if player.xPos < entity.yPos
        //      AND player.yPos - MISSILE_THREASHOLD < entity.yPos
        //      AND player.yPos + MISSILE_THREASHOLD > entity.yPos:
        //
        //      spawn missile
        //      entity.missileCooldown = entity->romData->missileCoolDown

        ldy.w   GameLoop.playerEntity

        lda.b   EntityStruct.xPos.px
        cmp.w   BaseEntity.xPos.px,y
        bcc     SkipMissileTest

        lda.w   BaseEntity.yPos.px,y

        // carry set
        sbc.w   #MISSILE_THREASHOLD
        cmp.b   EntityStruct.yPos.px
        bcs     SkipMissileTest

        // carry clear
        adc.w   #MISSILE_THREASHOLD * 2
        cmp.b   EntityStruct.yPos.px
        bcc     SkipMissileTest

        lda.w   #MISSILE_ACTION_POINT
        ldy.w   #Missile.EnemyPlasma.ENTITY_ID
        jsr     MetaSprite.ActionPoint.SpawnEntityOfType


        ldx.b   EntityStruct.romPtr
        lda.l   RomData.missileCooldown,x
        sta.b   EntityStruct.missileCooldown


SkipMissileTest:
    jmp     BaseEnemy.Process
}


// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
// INPUT: Y = entity attacking
// INPUT: A = attack value
a16()
i16()
code()
scope TakeDamageFromEntity: {
    jmp     BaseEnemy.TakeDamageFromEntity
}


// ::TODO collision behaviour::
a16()
i16()
code()
BlockWithShield:
CollideWithBody:
CollideWithShield:
    rts


macro _entity(name, frameSetId, velocity, missileCooldown) {
    scope {name} {
        rodata(EN_RomData)
        scope RomData: {
            dw  FunctionTable
            db  Entity.ListId.ENEMY // initial List
            dw  MSFS.{frameSetId}   // frameSetId
            db  0                   // defaultPalette
            dw  2                   // health
            dl  {velocity}          // launch velocity
            dw  {missileCooldown}   // missile cooldown
        }

        rodata(EN_RomDataList)
        constant ENTITY_ID((pc() - Entity.Data.List) / 2)
            dw  RomData
    }
}
_entity(Slow,   EnemyFighter,   -0x009876, 60)
_entity(Medium, EnemyFighter,   -0x010000, 50)
_entity(Fast,   EnemyFighter,   -0x012345, 40)

}
}

// vim: ft=asm ts=4 sw=4 et:

