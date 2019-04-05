// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2017, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

RegisterBreakId(INVALID_ENTITY_TYPE)

namespace Entities {
namespace EnemyCarrier {


constant X_POSITION = 180
constant SHIELD_HEALTH = 6
constant SHIELD_RECHARGE = 60 * 4

constant MAX_FRONT_DRONES = 4
constant MAX_REAR_DRONES = 4

constant SPAWN_DRONE_DELAY = 60 * 2

constant N_DEATH_EXPLOSIONS = 8


buildFunctionTable(BaseEntityFunctionTable, EnemyCarrier)
BaseEnemy.useDefaultDestructor()
BaseEnemy.useDefaultDeactivated()

namespace State {
    createEnum(0, 2)
        enum(MOVE_TO_POSITION)
        enum(HOVER)

    constant MASK = 0x1 << 1

    code()
    FunctionTable:
        dw  Process.MoveToPosition
        dw  Process.Hover

        assert(pc() - FunctionTable == MASK + 2)
}

namespace EntityStruct {
    childstruct(BaseEnemy)
        field(state, 2)
        field(shieldHealth, 2)
        field(shieldRechargeDelay, 2)
        field(spawnDroneDelay, 2)

        // Number of active front bay drones for this carrier
        // (This value is automatically incremented/decremented by EnemyCarrierDrone)
        field(frontBayDrones, 2)
        // Number of active rear bay drones for this carrier
        // (This value is automatically incremented/decremented by EnemyCarrierDrone)
        field(rearBayDrones, 2)
    endstruct()
}


// IN: A = parameter (currently undefined)
// OUT: C set = always activate entity
a16()
i16()
code()
function Init {

    // reset drone count
    stz.b   EntityStruct.frontBayDrones
    stz.b   EntityStruct.rearBayDrones

    lda.w   #{ExportOrder}.Animations.Flying_Shield
    jsr     MetaSprite.SetAnimation

    lda.w   #SHIELD_HEALTH
    sta.b   EntityStruct.shieldHealth

    assert(State.MOVE_TO_POSITION == 0)
    stz.b   EntityStruct.state

    jmp     BaseEnemy.Init
}


// A = MetaSprite.Collision.bits
// Y = MetaSprite.Collision.otherEntity
a16()
i16()
code()
function ProcessCollision {
    bit.w   #THIS_SHIELD
    beq     NoShieldCollision

        // otherEntity hit shield
        //
        // decrease sheild health, change animation if destroyed

        lda.b   EntityStruct.shieldHealth
        dec
        bpl     +
            lda.w   #0
        +
        sta.b   EntityStruct.shieldHealth
        bne     Return
            // shield is destroyed

            lda.w   #SHIELD_RECHARGE
            sta.b   EntityStruct.shieldRechargeDelay

            sep     #$20
        a8()

            lda.b   #{ExportOrder}.Animations.DropShield
            sta.b   BaseEnemy.MetaSprite.Animation.nextAnimation

            rep     #$30
        a16()


NoShieldCollision:
    // Handle body/attack collision

    lda.w   MetaSprite.Collision.bits
    jmp     BaseEnemy.ProcessCollision

Return:
    rts
}


// Process entity shield's health and recharge
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
macro ProcessShield() {
    lda.b   EntityStruct.shieldHealth
    bne     EndIf
        // Shield is recharging
        dec.b   EntityStruct.shieldRechargeDelay
        bpl     EndIf
            // shield if returned
            lda.w   #SHIELD_HEALTH
            sta.b   EntityStruct.shieldHealth

            sep     #$20
        a8()

            lda.b   #{ExportOrder}.Animations.RaiseShield
            sta.b   BaseEnemy.MetaSprite.Animation.nextAnimation

            rep     #$30
        a16()
EndIf:
}


// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
a16()
i16()
code()
function Process {
    ProcessShield()

    lda.b   EntityStruct.hp
    bne     +
        // Spawn multiple explosions when HP == 0
        jsr     SpawnMultipleExposions
    +

    lda.b   EntityStruct.state
    and.w   #State.MASK
    tax
    jmp     (State.FunctionTable,x)
}


// Move carrier to desired position on screen and holds it there
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
a16()
i16()
code()
function Process.MoveToPosition {

    // Do not spawn drones until carrier is in position
    stz.b   EntityStruct.spawnDroneDelay

    lda.b   EntityStruct.xPos.px
    sec
    sbc.w   Camera.xPos
    cmp.w   #X_POSITION
    bcs     ContinueMoving

        // in correct position, cancel velocity
        stz.b   EntityStruct.xVecl
        stz.b   EntityStruct.xVecl + 2

        ldx.w   #State.HOVER
        stx.b   EntityStruct.state

ContinueMoving:
    jmp     BaseEnemy.Process
}


// Default Carrier behaviour
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
a16()
i16()
code()
function Process.Hover {
    dec.b   EntityStruct.spawnDroneDelay

    jmp     BaseEnemy.Process
}


// Spawns lots of explosions at the entity position
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
a16()
i16()
function SpawnMultipleExposions {
allocateTmpWord(explosionLocation)
constant explosionLoc.x = explosionLocation
constant explosionLoc.y = explosionLocation + 1

    // Spawn multiple explosions

    lda.w   #N_DEATH_EXPLOSIONS - 1

    Loop:
        pha

        // EXTREMELY BASIC explosion "randomizer"
        //   explosionLoc.x += 13
        //   explosionLoc.y += 37
        //   spawn.xPos = entity.xPos - 24 + (explosionLoc.x & 63)
        //   spawn.yPos = entity.yPos - 16 + (explosionLoc.y & 31)

        lda.w   explosionLoc.x
        // ignore carry
        adc.w   #37 << 8 | 13
        sta.w   explosionLoc.x

        and.w   #63
        sec
        sbc.w   #24
        clc
        adc.b   EntityStruct.xPos.px
        sta.w   Entity.Spawn.xPos

        lda.w   explosionLoc.y
        and.w   #31
        sec
        sbc.w   #16
        clc
        adc.b   EntityStruct.yPos.px
        sta.w   Entity.Spawn.yPos

        ldy.w   #0
        lda.w   #Project.EntityIds.Explosion
        jsr     Entity.Spawn

        pla
        dec
        bne     Loop

    rts
}


// Spawn a EnemyCarrierDrone at the given action point if
//    * spawnDroneDelay is negative or >= SPAWN_DRONE_DELAY, and
//    * frontBayDrones/rearBayDrones < MAX_FRONT_DRONES/MAX_REAR_DRONES
//
// REQUITES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: DP: entity
//         A: action point positionPair (int_ms8 pair)
a16()
i16()
code()
function SpawnDroneActionPoint {

    ldx.b   EntityStruct.functionTable
    cpx.w   #FunctionTable
    beq     +
        // This ActionPoint is only allowed on a EnemyCarrier entity
        break(INVALID_ENTITY_TYPE)
    +

    // Only spawn drone is spawnDroneDelay is negative or >= SPAWN_DRONE_DELAY
    ldx.b   EntityStruct.spawnDroneDelay
    cpx.w   #SPAWN_DRONE_DELAY
    bcc     Return

    tax
    // X = AP positionPair
    // MUST NOT MODIFY X

    // Test if AP is left or right of origin
    and.w   #0xff
    cmp.w   #INT_MS8_OFFSET
    bcs     +
        // front bay drone
        lda.b   EntityStruct.frontBayDrones
        cmp.w   #MAX_FRONT_DRONES
        bcs     Return

        ldy.w   #Project.EntityIds.EnemyCarrierDrone_FrontBay
        bra     ++
    +
        // rear bay drone
        lda.b   EntityStruct.rearBayDrones
        cmp.w   #MAX_REAR_DRONES
        bcs     Return

        ldy.w   #Project.EntityIds.EnemyCarrierDrone_RearBay
    +

    lda.w   #SPAWN_DRONE_DELAY
    sta.b   EntityStruct.spawnDroneDelay

    txa
    // A = AP positionPair
    // Y = entityId
    jmp     Entity.SpawnEntityAtActionPoint

Return:
    rts
}

}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

