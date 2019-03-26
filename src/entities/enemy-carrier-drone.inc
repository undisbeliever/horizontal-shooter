// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2017, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

namespace Entities {
namespace EnemyCarrierDrone {

constant SCREEN_MARGIN_TOP = 24
constant SCREEN_MARGIN_BOTTOM = 24

buildFunctionTable(BaseEntityFunctionTable, EnemyCarrierDrone)
BaseEnemy.useDefaultDeactivated()
BaseEnemy.useDefaultProcessCollision()

namespace State {
    createEnum(0, 2)
        enum(MOVE_TO_POSITION)
        enum(PATROL)

    constant MASK = 0x1 << 1

    code()
    FunctionTable:
        dw  Process.MoveToPosition
        dw  Process.Patrol

        assert(pc() - FunctionTable == MASK + 2)
}

namespace EntityStruct {
    childstruct(BaseEnemy)
        field(state, 2)

        // if non-zero then entity is moving down
        field(movingUpOnZero, 1)

        // frame decrement counter
        field(counter, 2)

        // address of the path
        field(path, 2)

        // the instanceId of the carrier that spawned this drone
        field(carrierEntityId, 2)
    endstruct()
}

rodata(rom0)
constant PathOffset = pc() & 0xff0000

namespace PathStruct {
    // repeated until frames == 0
    struct(PathOffset)
        // (0:7:15 fixed point)
        field(xVecl, 4)
        // (0:7:15 fixed point)
        field(yVecl, 4)
        // number of movement frames
        // (byte - frames)
        field(frames, 1)
    endstruct()
}


// IN: A = parameter (boss address)
//     MetaSprite.ActionPoint.address - address of actionPoint that spawned this entity
// OUT: C set = always activate entity
a16()
i16()
code()
function Init {
    // store the instanceId of the carrier
    // A = carrier address
    tay
    lda.w   EnemyCarrier.EntityStruct.instanceId,y
    sta.b   EntityStruct.carrierEntityId

    // store hp and path
    ldx.b   EntityStruct.romPtr

    lda.l   {RomStruct}.health,x
    sta.b   EntityStruct.hp

    lda.l   {RomStruct}.path,x
    tyx

    // increment carrier drone counter
    // A = path address
    // X = Carrier Entity Address
    sta.b   EntityStruct.path
    cmp.w   #FrontBayPath
    bne     +
        inc.w   EnemyCarrier.EntityStruct.frontBayDrones,x
        bra     ++
    +
        inc.w   EnemyCarrier.EntityStruct.rearBayDrones,x
    +

    // Set animation and state
    lda.w   #{ExportOrder}.Animations.Animation
    jsr     MetaSprite.SetAnimation

    assert(State.MOVE_TO_POSITION == 0)
    stz.b   EntityStruct.state

    sep     #$20
a8()
    // set up-down direction
    lda.w   MetaSprite.ActionPoint.positionPair.yPos
    cmp.b   #INT_MS8_OFFSET
    bcs     +
        // moving up
        lda.b   #0
    +
    sta.b   EntityStruct.movingUpOnZero

    rep     #$30
a16()

    jsr     _LoadNextPath

    // always activate
    sec
    rts
}


a16()
i16()
code()
function Destructor {
    // Get carrier address (if it is still alive)
    lda.b   EntityStruct.carrierEntityId
    Entity.GetEntityWithIdOrBranchIfEmpty_Y(CarrierDead)

        // carrier still alive, decrement drone counter

        ldx.b   EntityStruct.romPtr
        lda.l   {RomStruct}.path,x
        tyx

        // A = path address
        // X = Carrier Entity Address
        cmp.w   #FrontBayPath
        bne     +
            dec.w   EnemyCarrier.EntityStruct.frontBayDrones,x
            bra     ++
        +
            dec.w   EnemyCarrier.EntityStruct.rearBayDrones,x
        +
    CarrierDead:

    rts
}

// Loads the next path into the entity velocity/counter
// INPUT: dp = entity
// OUTPUT: n set if this is the last path
a16()
i16()
code()
function _LoadNextPath {
    // X = path
    // entity.path += PathStruct.size
    //
    // entity.movementFrames = X->path.frames
    //
    // if rs.movingUpOnZero != 0
    //      entity.yVecl = - X->path.yVecl
    // else:
    //      entity.yVecl = X->path.yVecl
    // entity.xVecl = X->path.xVecl

    ldx.b   EntityStruct.path

    txa
    clc
    adc.w   #PathStruct.size
    sta.b   EntityStruct.path

    lda.b   EntityStruct.movingUpOnZero
    and.w   #0xff
    bne     MoveDown
        // moving up
        sec
        lda.w   #0
        sbc.l   PathStruct.yVecl,x
        sta.b   EntityStruct.yVecl

        lda.w   #0
        sbc.l   PathStruct.yVecl + 2,x

        bra     EndIf

    MoveDown:
        // moving down
        lda.l   PathStruct.yVecl,x
        sta.b   EntityStruct.yVecl

        lda.l   PathStruct.yVecl + 2,x

EndIf:
    sta.b   EntityStruct.yVecl + 2

    lda.l   PathStruct.xVecl,x
    sta.b   EntityStruct.xVecl
    lda.l   PathStruct.xVecl + 2,x
    sta.b   EntityStruct.xVecl + 2

    lda.l   PathStruct.frames,x
    and.w   #0xff
    sta.b   EntityStruct.counter

    rts
}


// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
a16()
i16()
code()
function Process {
    // Check if drone has died
    lda.b   EntityStruct.carrierEntityId
    Entity.GetEntityWithIdOrBranchIfEmpty_X(CarrierDead)

    lda.b   EntityStruct.state
    and.w   #State.MASK
    tax
    jmp     (State.FunctionTable,x)


CarrierDead:
    // The carrier is no more, reduce HP to zero and drone explodes
    stz.b   EntityStruct.hp
    jmp     BaseEnemy.Process
}


// Move drone to desired position on screen and holds it there
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
a16()
i16()
code()
function Process.MoveToPosition {
    // entity.counter--
    // if entity.counter == 0:
    //      _LoadNextPath
    //      if no more paths:
    //          entity.state = PATROL

    dec.b   EntityStruct.counter
    bne     +
        jsr     _LoadNextPath
        bne     +
            // no more paths
            ldx.w   #State.PATROL
            stx.b   EntityStruct.state
    +

    jmp     BaseEnemy.Process
}


// Default Carrier behaviour
// REQUIRES: 16 bit A, 16 bit Index, DB = 0x7e
// INPUT: dp = entity
a16()
i16()
code()
function Process.Patrol {
    // Invert Y velocity when hit margins

    lda.b   EntityStruct.yVecl + 2
    bpl     MovingDown
        // Moving Up
        lda.w   Camera.yPos
        clc
        adc.w   #SCREEN_MARGIN_TOP
        cmp.b   EntityStruct.yPos.px
        bcc     EndIf
            bra     InvertYVelocity

    MovingDown:
        // Moving Down
        lda.w   Camera.yPos
        clc
        adc.w   #Camera.DISPLAY_HEIGHT - SCREEN_MARGIN_BOTTOM
        cmp.b   EntityStruct.yPos.px
        bcs     EndIf

        InvertYVelocity:
            sec
            lda.w   #0
            sbc.b   EntityStruct.yVecl
            sta.b   EntityStruct.yVecl

            lda.w   #0
            sbc.b   EntityStruct.yVecl + 2
            sta.b   EntityStruct.yVecl + 2

    EndIf:

    jmp     BaseEnemy.Process
}


rodata(rom0)
assert(PathOffset == pc() & 0xff0000)

FrontBayPath:
    dd  0x000000                // xVecl
    dd  0x007777                // yVecl
    db  45                      // frame count

    dd  -0x07777                // xVecl
    dd  0x003bbb                // yVecl
    db  55                      // frame count

    dd  0                       // xVecl
    dd  0x011000                // yVecl
    db  0

RearBayPath:
    dd  0x006666                // xVecl
    dd  0x000000                // yVecl
    db  50                      // frame count

    dd  0x005555                // xVecl
    dd  0x003bbb                // yVecl
    db  50                      // frame count

    dd  0                       // xVecl
    dd  0x011000                // yVecl
    db  0
}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

