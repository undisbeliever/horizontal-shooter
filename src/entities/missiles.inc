// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2017, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

namespace Entities {
namespace Missile {

// Missile X Velocity
// (0:7:16 fixed point)
constant X_VELOCITY = 0x028000


buildFunctionTable(BaseEntityFunctionTable, Missile)
Entity.useDefaultRenderer()

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

namespace RomData {
    childstruct(BaseEntityData)
        field(initialVelocity, 4)
        field(reflectOffShield, 1)
    endstruct()
}


// IN: A = parameter (address entity)
//    MetaSprite.ActionPoint.address - address of actionPoint that spawned the missile
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
        sbc.l   RomData.initialVelocity,x
        sta.b   EntityStruct.xVecl

        lda.w   #0
        sbc.l   RomData.initialVelocity + 2,x
        sta.b   EntityStruct.xVecl + 2

        lda.w   #MSEO.Missile.Animations.Move_Left

        bra     EndIf

    Right:
        // missile travels right
        ldx.b   EntityStruct.romPtr

        lda.l   RomData.initialVelocity,x
        sta.b   EntityStruct.xVecl
        lda.l   RomData.initialVelocity + 2,x
        sta.b   EntityStruct.xVecl + 2

        lda.w   #MSEO.Missile.Animations.Move_Right

EndIf:

    jsr     MetaSprite.SetAnimation


    assert(State.FLYING == 0)
    stz.b   EntityStruct.state

    rts
}

a16()
i16()
code()
function Destructor {
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
        lda.l   RomData.reflectOffShield,x
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

    // corrupt dpTmp to test if EntityHitbox can handle overridden dpTmp variables
    lda.w   #0xdead
    sta.w   dpTmp0
    sta.w   dpTmp1
    sta.w   dpTmp2
    sta.w   dpTmp3
    sta.w   dpTmp4
    sta.w   dpTmp5
    sta.w   dpTmp6
    sta.w   dpTmp7
    sta.w   dpTmp8
    sta.w   dpTmp9
    sta.w   dpTmp10

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
    jmp     Physics.ProcessEntity
}


a16()
i16()
code()
function Process.Exploding {
    // ::TODO wait for animation to end::

    ldy.w   #Entity.lists.toDelete
    sty.w   Entity.changeList
    rts
}


a16()
i16()
code()
function Process.Reflect {
    assert(State.FLYING == 0)
    stz.b   EntityStruct.state

    // Change missile type

    ldy.w   Entity.currentList
    cpy.w   #Entity.lists.enemyProjectiles
    bne     +
        ldy.w   #Entity.lists.playerProjectiles
        bra     ++
    +
        ldy.w   #Entity.lists.enemyProjectiles
    +
    sty.w   Entity.changeList


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
        lda.w   #MSEO.Missile.Animations.Move_Left
        bra     ++
    +
        // travelling left
        lda.w   #MSEO.Missile.Animations.Move_Right
    +

    jmp     MetaSprite.SetAnimation
}



inline _entity(name, frameSetId, velocity, reflectOffShield) {
    namespace {name} {
        rodata(EN_RomData)
        RomData:
            dw  FunctionTable
            db  0                   // initial List (always null)
            dw  MSFS.{frameSetId}   // frameSetId
            db  0                   // defaultPalette
            dd  {velocity}          // launch velocity
            db  {reflectOffShield}  // reflectOffShield


        rodata(EN_ProjectileList)
        constant PROJECTILE_ID = (pc() - Entity.Data.ProjectileList) / 2
            dw  RomData
    }
}
_entity(PlayerPlasma, PlayerPlasma, 0x028000, 1)
_entity(EnemyPlasma, EnemyPlasma, 0x020000, 1)

}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

