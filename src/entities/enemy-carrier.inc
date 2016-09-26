
scope Entities {
scope EnemyCarrier {


constant X_POSITION(180)
constant SHIELD_HEALTH(6)
constant SHIELD_RECHARGE(60 * 4)

constant MAX_FRONT_DRONES(4)
constant MAX_REAR_DRONES(4)

constant SPAWN_DRONE_DELAY(60 * 2)
constant FRONT_BAY_ACTION_POINT(1)
constant REAR_BAY_ACTION_POINT(2)


buildFunctionTable(BaseEntityFunctionTable, EnemyCarrier)
Entity.useDefaultRenderer()
BaseEnemy.useDefaultDestructor()
BaseEnemy.useDefaultCalculateAttackValue()

scope State {
    createEnum(0, 2)
        enum(MOVE_TO_POSITION)
        enum(HOVER)

    constant MASK(0x1 << 1)

    code()
    FunctionTable:
        dw  Process.MoveToPosition
        dw  Process.Hover

        assert(pc() - FunctionTable == MASK + 2)
}

scope EntityStruct {
    childstruct(BaseEnemy)
        field(state, 2)
        field(shieldHealth, 2)
        field(shieldRechargeDelay, 2)
        field(spawnDroneDelay, 2)
    endstruct()
}

scope RomData {
    childstruct(BaseEnemyData)
    endstruct()
}


// IN: A = parameter (currently undefined)
a16()
i16()
code()
scope Init: {
    lda.w   #MSEO.EnemyCarrier.Animations.Flying_Shield
    jsr     MetaSprite.SetAnimation

    lda.w   #SHIELD_HEALTH
    sta.b   EntityStruct.shieldHealth

    assert(State.MOVE_TO_POSITION == 0)
    stz.b   EntityStruct.state

    jmp     BaseEnemy.Init
}


// Process entity shield's health and recharge
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
macro scope ProcessShield() {
    lda.b   EntityStruct.shieldHealth
    bne     EndIf
        // Shield is recharging
        dec.b   EntityStruct.shieldRechargeDelay
        bpl     EndIf
            // shield if returned
            lda.w   #SHIELD_HEALTH
            sta.b   EntityStruct.shieldHealth

            lda.w   #MSEO.EnemyCarrier.Animations.RaiseShield
            jsr     MetaSprite.SetAnimation
EndIf:
}


// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
a16()
i16()
code()
scope Process: {
    ProcessShield()

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
scope Process.MoveToPosition: {

    lda.b   EntityStruct.xPos.px
    sec
    sbc.w   Camera.xPos
    cmp.w   #X_POSITION
    bcs     ContinueMoving

        // in correct position, cancel velocity
        stz.b   EntityStruct.xVecl
        stz.b   EntityStruct.xVecl + 1

        stz.b   EntityStruct.spawnDroneDelay

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
scope Process.Hover: {
    lda.b   EntityStruct.spawnDroneDelay
    dec
    bpl     SkipSpawnDrone

        lda.w   #EnemyCarrierDrone.FrontBay.ENTITY_ID
        jsr     Entity.CountEnemiesWithId
        cmp.w   #MAX_FRONT_DRONES - 1
        bcs     +
            lda.w   #FRONT_BAY_ACTION_POINT
            ldy.w   #EnemyCarrierDrone.FrontBay.ENTITY_ID
            jsr     Entity.SpawnAtActionPoint
        +

        lda.w   #EnemyCarrierDrone.RearBay.ENTITY_ID
        jsr     Entity.CountEnemiesWithId
        cmp.w   #MAX_REAR_DRONES - 1
        bcs     +
            lda.w   #REAR_BAY_ACTION_POINT
            ldy.w   #EnemyCarrierDrone.RearBay.ENTITY_ID
            jsr     Entity.SpawnAtActionPoint
        +

        lda.w   #SPAWN_DRONE_DELAY

SkipSpawnDrone:
    sta.b   EntityStruct.spawnDroneDelay

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


// Decrease shield health
// INPUT: dp = entity
// INPUT: Y = entity attacking
// INPUT: C = set if collide with an attack
a16()
i16()
BlockWithShield: {
    lda.b   EntityStruct.MetaSprite.Animation.id
    and.w   #0xff
    cmp.w   #MSEO.EnemyCarrier.Animations.Flying_Shield
    bne     Return

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

        lda.w   #MSEO.EnemyCarrier.Animations.DropShield
        jmp     MetaSprite.SetAnimation
Return:
    rts
}


// ::TODO collision behaviour::
a16()
i16()
code()
CollideWithBody:
CollideWithShield:
    rts


rodata(EN_RomData)
scope CarrierRomData: {
    dw  FunctionTable
    db  Entity.ListId.BOSS  // initial List
    dw  MSFS.EnemyCarrier   // frameSetId
    db  0                   // defaultPalette
    dw  50                  // health
    dl  -0x010000           // launch velocity
}

rodata(EN_RomDataList)
constant ENTITY_ID((pc() - Entity.Data.List) / 2)
    dw  CarrierRomData

}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

