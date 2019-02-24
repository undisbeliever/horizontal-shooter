// This file is part of Horizontal Shooter.
// Copyright (c) 2016 - 2017, Marcus Rowe <undisbeliever@gmail.com>.
// Distributed under The MIT License: https://opensource.org/licenses/MIT

namespace Entities {
namespace Particles {

buildFunctionTable(BaseEntityFunctionTable, Particles)

// IN: A = parameter (address entity)
//     MetaSprite.ActionPoint.address - address of actionPoint that spawned the missile
// OUT: C set = always activate entity
a16()
i16()
code()
function Init {
    lda.w   #{ExportOrder}.Animations.Animation
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
    rts
}


a16()
i16()
code()
function Process {
    jmp     Entity.DeleteIfAnimationEndsOrDrawAndGotoNextEntity
}

}
}

// vim: ft=bass-65816 ts=4 sw=4 et:

