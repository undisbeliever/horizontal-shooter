// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2017, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

namespace Entities {
namespace Particles {

buildFunctionTable(BaseEntityFunctionTable, Particles)
Entity.useDefaultRenderer()

// IN: A = parameter (address entity)
//     MetaSprite.ActionPoint.address - address of actionPoint that spawned the missile
// OUT: C set = always activate entity
a16()
i16()
code()
function Init {
    lda.w   #MSEO.Particle.Animations.Animation
    jsr     MetaSprite.SetAnimation

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


// A = MetaSprite.Collision.bits
// Y = MetaSprite.Collision.otherEntity
a16()
i16()
code()
function ProcessCollision {
    rts
}


a16()
i16()
code()
function Process {
    // Delete entity if animation.id == 0xff

    sep     #$20
a8()
    lda.b   BaseEntity.MetaSprite.Animation.id
    inc
    bne     +
        rep     #$30
    a16()
        jmp     Entity.DeleteAndGotoNextEntity
    +

    rep     #$30
a16()
    jmp     Entity.GotoNextEntity
}


inline _entity(name, frameSetId, palette) {
    namespace {name} {
        rodata(EN_RomData)
        RomData:
            dw  FunctionTable
            db  Entity.ListId.PARTICLE  // initial List
            db  {palette}               // defaultPalette
            dw  MSFS.{frameSetId}       // frameSetId


        rodata(EN_EntityList)
        constant ENTITY_ID = (pc() - Entity.Data.EntityList) / 2
            dw  RomData
    }
}
_entity(Explosion, Explosion, 0)

}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

