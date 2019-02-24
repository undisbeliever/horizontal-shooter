// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2017, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

namespace Entities {
namespace EnemySinusoidal {

constant X_PADDING = 30
constant MISSILE_THREASHOLD = 15
constant MISSILE_ACTION_POINT = 1
constant SECOND_SHOT_COOLDOWN = 30

buildFunctionTable(BaseEntityFunctionTable, EnemySinusoidal)
BaseEnemy.useDefaultDestructor()
BaseEnemy.useDefaultDeactivated()
BaseEnemy.useDefaultProcessCollision()


namespace EntityStruct {
    childstruct(BaseEnemy)
        field(sineTableIndex, 2)
        field(missileCooldown, 2)
    endstruct()
}


// IN: A = parameter (currently undefined)
// OUT: C set = always activate entity
a16()
i16()
code()
function Init {
    lda.w   #{ExportOrder}.Animations.Flying_Left
    jsr     MetaSprite.SetAnimation

    stz.b   EntityStruct.sineTableIndex
    stz.b   EntityStruct.missileCooldown

    jmp     BaseEnemy.Init
}


a16()
i16()
code()
function Process {
    // Change velocity

    lda.b   EntityStruct.sineTableIndex
    clc
    adc.w   #2 * 2
    tax
    cpx.w   #SineTable16.size
    bcc     +
        ldx.w   #0
    +
    stx.b   EntityStruct.sineTableIndex

    lda.l   SineTable16,x
    asl
    sta.b   EntityStruct.yVecl

    // sign extend
    bcs     +
        lda.w   #0
        bra     ++
    +
        lda.w   #0xffff
    +
    sta.b   EntityStruct.yVecl + 2


    namespace TestXVecl {
        // Change x velocity so entity stays on screen


        // if (xVecl > 0 and xPos >= Camera.xPos + DISPLAY_WIDTH - X_PADDING)
        //    or (xVecl < 0 and xPos - X_PADDING <= player.xPos)
        //      xVecl = -xVelocity

        lda.b   EntityStruct.xVecl + 2
        bmi     NegXVecl
            // positive velocity
            lda.w   Camera.xPos
            clc
            adc.w   #Camera.DISPLAY_WIDTH - X_PADDING
            cmp.b   EntityStruct.xPos.px
            bcc     NegateVelocity
            bra     EndIf

        NegXVecl:
            // negative velocity
            ldy.w   GameLoop.playerEntity

            lda.b   EntityStruct.xPos.px
            sec
            sbc.w   #X_PADDING
            cmp.w   BaseEntity.xPos.px,y
            bcs     EndIf

            NegateVelocity:
                sec
                lda.w   #0
                sbc.b   EntityStruct.xVecl
                sta.b   EntityStruct.xVecl

                lda.w   #0
                sbc.b   EntityStruct.xVecl + 2
                sta.b   EntityStruct.xVecl + 2
    EndIf:
    }
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
        ldy.w   #Project.ProjectileIds.EnemyPlasma
        jsr     Entity.SpawnEnemyProjectileFromActionPoint

        ldx.b   EntityStruct.romPtr
        lda.l   {RomStruct}.missileCooldown,x
        sta.b   EntityStruct.missileCooldown


SkipMissileTest:
    jmp     BaseEnemy.Process
}
}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

