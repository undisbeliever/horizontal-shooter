// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2017, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

namespace Entities {
namespace Missile {

// Missile X Velocity
// (0:7:16 fixed point)
constant X_VELOCITY = 0x028000


buildFunctionTable(BaseEntityFunctionTable, Missile)

namespace State {
    createEnum(0, 2)
        enum(FLYING)
        enum(EXPLODING)
        enum(REFLECT)
        // null

    constant MASK = 0x3 << 1

    code()
    FunctionTable:
        dw  Process.Flying
        dw  Process.Exploding
        dw  Process.Reflect
        dw  Process.Null

        assert(pc() - FunctionTable == MASK + 2)
}

namespace EntityStruct {
    childstruct(BaseEntity)
        field(state, 2)
    endstruct()
}


// IN: A = parameter (address entity)
//     MetaSprite.ActionPoint.address - address of actionPoint that spawned the missile
// OUT: C set = always activate entity
a16()
i16()
code()
function Init {
    ldx.w   MetaSprite.ActionPoint.address
    lda.l   MetaSprite.Format.ActionPoints.xPos,x
    and.w   #0x00ff
    cmp.w   #INT_MS8_OFFSET
    bcs     Right

        // Missile travels left
        // load negative velocity
        ldx.b   EntityStruct.romPtr

        sec
        lda.w   #0
        sbc.l   {RomStruct}.initialVelocity,x
        sta.b   EntityStruct.xVecl

        lda.w   #0
        sbc.l   {RomStruct}.initialVelocity + 2,x
        sta.b   EntityStruct.xVecl + 2

        lda.w   #{ExportOrder}.Animations.Move_Left

        bra     EndIf

    Right:
        // missile travels right
        ldx.b   EntityStruct.romPtr

        lda.l   {RomStruct}.initialVelocity,x
        sta.b   EntityStruct.xVecl
        lda.l   {RomStruct}.initialVelocity + 2,x
        sta.b   EntityStruct.xVecl + 2

        lda.w   #{ExportOrder}.Animations.Move_Right

EndIf:

    jsr     MetaSprite.SetAnimation


    assert(State.FLYING == 0)
    stz.b   EntityStruct.state

    // always activate
    sec
    rts
}

a16()
i16()
code()
function Destructor {
    rts
}

a16()
i16()
code()
function Deactivated {
    rts
}


// A = MetaSprite.Collision.bits
// Y = MetaSprite.Collision.otherEntity
a16()
i16()
code()
function ProcessCollision {

    bit.w   #OTHER_SHIELD
    beq     +
        // Missile has collided with a shield
        //
        // If reflectable then reflect back at opponent

        ldx.b   EntityStruct.romPtr
        lda.l   {RomStruct}.reflectOffShield,x
        and.w   #0xff
        beq     CollideWithBody

        ldx.w   #State.REFLECT
        stx.b   EntityStruct.state

        lda.w   MetaSprite.Collision.bits
    +

    // A = collision bits
    bit.w   #OTHER_BODY
    beq     +
        // Missile collided with a body

    CollideWithBody:
        ldx.w   #State.EXPLODING
        stx.b   EntityStruct.state
    +

    rts
}


a16()
i16()
code()
function Process {
    lda.b   EntityStruct.state
    and.w   #State.MASK
    tax
    jmp     (State.FunctionTable,x)
}


a16()
i16()
code()
Process.Null:
function Process.Flying {
    jsr     Physics.ProcessEntity
    jmp     Entity.DeleteIfOutsideOrDrawAndGotoNextEntity
}


a16()
i16()
code()
function Process.Exploding {
    // ::TODO wait for animation to end::

    jmp     Entity.DeleteAndGotoNextEntity
}


a16()
i16()
code()
function Process.Reflect {
    assert(State.FLYING == 0)
    stz.b   EntityStruct.state


    // Invert Velocity
    sec
    lda.w   #0
    sbc.b   EntityStruct.yVecl
    sta.b   EntityStruct.yVecl

    lda.w   #0
    sbc.b   EntityStruct.yVecl + 2
    sta.b   EntityStruct.yVecl + 2


    sec
    lda.w   #0
    sbc.b   EntityStruct.xVecl
    sta.b   EntityStruct.xVecl

    lda.w   #0
    sbc.b   EntityStruct.xVecl + 2
    sta.b   EntityStruct.xVecl + 2

    bpl     +
        // travelling left
        lda.w   #{ExportOrder}.Animations.Move_Left
        bra     ++
    +
        // travelling left
        lda.w   #{ExportOrder}.Animations.Move_Right
    +

    jsr     MetaSprite.SetAnimation


    // Change missile type
    sep     #$20
a8()

    lda.b   EntityStruct.listId
    cmp.b   #EntityLists.EnemyProjectiles
    bne     +
        lda.b   #EntityLists.PlayerProjectiles
        bra     ++
    +
        lda.b   #EntityLists.EnemyProjectiles
    +

    // DrawAndChangeEntityListIdAndGotoNextEntity can be called with an 8 bit A
    jmp     Entity.DrawAndChangeEntityListIdAndGotoNextEntity
}
}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

